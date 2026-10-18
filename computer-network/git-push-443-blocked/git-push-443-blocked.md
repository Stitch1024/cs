# 当 git push 撞上防火墙 —— 一次 443 端口连接失败的原理剖析与排查实录

> A step-by-step teardown of a `git push` failure caused by network-level blocking of
> `github.com:443`, plus the networking background behind the fix.
>
> 一次真实的 `git push` 失败，从现象、排查、根因到原理的完整记录，附带所涉及的知识点地图。

---

## 1. 现象 (Symptom)

```console
$ git push -u origin master
fatal: unable to access 'https://github.com/Stitch1024/artifical-intelligence.git/':
Failed to connect to github.com port 443 after 21102 ms: Couldn't connect to server
```

关键信息：

- 目标 `https://github.com`，端口 **443**。
- 等待约 **21 秒**后超时 → `Couldn't connect to server`。
- 报错发生在**连接阶段**，而不是认证 / 推送阶段。

---

## 2. 排查过程 (Diagnosis)

按层次从下往上查：DNS → 三层可达性 → 四层可达性 → 代理配置。

### 2.1 DNS 解析 —— 正常

```powershell
Resolve-DnsName github.com
# Name          IPAddress        Type
# github.com    20.205.243.166   A
```

域名能正常解析到 GitHub 的亚太节点 IP（`20.205.243.166`），**DNS 环节没问题**。

### 2.2 ICMP 可达性（ping）—— 正常

```powershell
Test-NetConnection github.com -Port 443
# PingSucceeded        : True
# PingReplyDetails RTT : 199 ms
# TcpTestSucceeded     : False   ← 注意这里
```

`ping`（ICMP）能通，但 **`TcpTestSucceeded = False`**：

> **TCP 443 握手失败。**

这就是本次故障最关键的信号——**能 ping 通 ≠ 能建立 TCP 连接**。

### 2.3 代理配置 —— 未设置

```powershell
git config --global --get http.proxy    # (空)
git config --global --get https.proxy   # (空)
$env:HTTP_PROXY                         # (空)
```

说明 git 在**直连** github.com，即使本机有代理软件也没走它。

### 2.4 定位本机代理端口

```powershell
Get-Process | ? ProcessName -match 'clash|verge'
# clash-verge, verge-mihomo ...
Get-NetTCPConnection -State Listen | ? LocalPort -in 7890,7897,1080
# LocalAddress  LocalPort  OwningProcess
# ::            7897       41036        ← Clash Verge 混合端口
```

Clash Verge（内核 `verge-mihomo`）监听在 **`127.0.0.1:7897`**（混合端口 Mixed Port）。

### 2.5 验证代理是否可用

```powershell
curl.exe -x http://127.0.0.1:7897 -s -o NUL -w "HTTP %{http_code}\n" https://github.com
# HTTP 200
```

通过代理访问 GitHub 返回 **200**，证明**代理链路完全可用**。

---

## 3. 根因 (Root Cause)

**不是电脑或 git 的问题，而是到 `github.com:443` 的直连在链路上被拦截了。**

- 处于有网络审查 / 干扰的环境（校园网、公司网或运营商跨境链路），GitHub 属于被针对性干扰的目标。
- **ping 通但 TCP 不通**的原因：`ping` 走的是 ICMP，而 HTTPS / git push 走的是 **TCP 443**。干扰通常只针对 TCP 握手做阻断（丢弃数据包或注入 RST 重置），ICMP 不受影响。
- 解析到的 `20.205.243.166` 是 GitHub 亚太节点，其 443 端口尤其容易被封。
- 因为没有配置任何代理，git 直连 github.com，直接撞墙。

---

## 4. 解决方案 (Fix)

### 4.1 让 git 走本地代理（本次采用）

```bash
git config --global http.proxy  http://127.0.0.1:7897
git config --global https.proxy http://127.0.0.1:7897
```

之后即可正常 `git push`。

> **注意**：代理软件（Clash Verge）必须保持开启，否则 git 找不到 7897 端口又会失败。

**取消代理设置：**

```bash
git config --global --unset http.proxy
git config --global --unset https.proxy
```

### 4.2 备选方案：SSH over 443

当 443 对 `github.com` 被过滤、但到 `ssh.github.com:443` 尚可时可用：

```bash
ssh -T -p 443 git@ssh.github.com
```

`~/.ssh/config`：

```sshconfig
Host github.com
  Hostname ssh.github.com
  Port 443
  User git
```

### 4.3 备选方案：换网络 / 换镜像

- 手机热点：换一条未被干扰的链路。
- 镜像源 / 镜像站：适合只读拉取，不适合推送私有仓库。

---

## 5. 原理：直连 vs 代理的流量拓扑 (Traffic Topology)

### 5.1 直连（出问题时）

```
你的电脑
   |  DNS: github.com → 20.205.243.166     (运营商 DNS 能看到你要访问 github)
   |  TCP 握手: 目标 20.205.243.166:443     (明文，SNI/目标 IP 完全暴露)
   v
家用路由器 / 光猫  (网关 10.146.247.211)
   |
   v
运营商 ISP 骨干网  →  跨境链路出口
   |
   +-- ✗  在此被拦截：识别到目标 = github.com 的 IP:443
   |         → 丢弃 TCP 包 / 注入 RST 重置握手
   |         → 约 21 秒后报 "Couldn't connect to server"
   |
   ✗ (包根本没出去)
   |
github.com (20.205.243.166)   ← 连接从未到达
```

### 5.2 走代理（解决后）

```
你的电脑 --(本地明文)--> Clash(127.0.0.1:7897) --(加密隧道)--> 海外节点 --(干净直连)--> github.com
```

- 你电脑实际连接的目标是 `127.0.0.1:7897`（本机），审查看不到你要访问 GitHub。
- 跨境那段是**加密且不含 github 明文目标**的流量，审查无法识别、无法针对性重置。
- 真正连 GitHub 的动作由**海外节点**完成，那条链路不在审查范围内。

### 5.3 对比表

| 维度 | 直连（之前） | 走代理（之后） |
|------|--------------|----------------|
| 你电脑实际连的目标 | `github.com:443`（暴露） | `127.0.0.1:7897`（本机） |
| 传输内容 | 明文，审查能看到目标 | 加密隧道，审查看不到内容 |
| 谁去连 GitHub | 你自己（撞墙） | 海外节点（干净直连） |
| 拦截点行为 | 跨境链路入口直接掐断 | 看不到真实目标，放行 |
| 结果 | `Couldn't connect to server` | HTTP 200 / push 成功 |

---

## 6. 本质 (Essence)

一句话：**尝试明文直连 GitHub，流量在跨境出口被 GFW / 防火墙识别并拦截。**

拆成三个关键词：

1. **明文**：直连时 TCP 握手的目标 IP、SNI 里的 `github.com` 在链路上**可见**，能被识别出"在访问 GitHub"。
2. **在 ISP / 跨境出口拦截**：识别后不是 GitHub 拒绝你，而是审查节点在**自己的网络里把连接掐掉**（丢包 / RST）。所以是 `Couldn't connect to server`，不是 `403`。
3. **GFW**：准确说是这条链路上的**审查系统**，不一定是 ISP 独有——校园网 / 公司网防火墙也能做同样的事。

> **常见误解纠正**：绕过的关键是**"让目标不可识别"**，而不只是"加密"。
> 代理有效是因为 GFW 看到的目标是 `127.0.0.1:7897`、跨境段是加密且无明文目标的流量。
> 反例：如果代理协议本身被识别 / 节点 IP 被整条墙掉，一样会连不上——此时要换节点或换协议。

---

## 7. 知识点地图 (Knowledge Map)

| 层次 | 知识点 | 与本例的关系 |
|------|--------|--------------|
| 计算机网络基础 | OSI / TCP-IP 分层 | 为什么 ICMP 与 TCP 待遇不同 |
| | TCP 三次握手 | `Couldn't connect` = 握手未完成 |
| | DNS 解析 | 域名 → IP 的过程 |
| | 端口 / Socket | 443、7897 的含义 |
| 应用层安全 | HTTPS / TLS 握手 | 加密连接如何建立 |
| | **SNI（Server Name Indication）** | "明文目标"被看见的关键 |
| 网络安全 / 审查 | DNS 污染、IP 封锁、SNI 检测 + RST 注入 | GFW 的常见手段 |
| | 加密隧道 / 混淆 obfuscation | 绕过审查的原理 |
| 代理与网络架构 | 正向 / 反向代理、HTTP / SOCKS 代理 | Clash 混合端口 |
| | 代理链 / 流量转发拓扑 | `本机 → 代理 → 节点 → 目标` |
| | VPN vs 代理 | 概念区别 |
| 工程实践 | git `http.proxy` / `https.proxy` | 本次实际修改 |
| | 环境变量 `HTTP_PROXY` / `HTTPS_PROXY` | 另一种配置方式 |
| | Clash mixed port、规则分流 | 代理软件使用 |

**学习路线建议**：`TCP/IP 分层 + TCP 握手 + DNS` → `HTTPS / TLS 与 SNI` → `GFW 工作原理与代理隧道`。

---

## 8. 命令速查 (Cheatsheet)

```bash
# 查看当前代理配置
git config --global --get http.proxy
git config --global --get https.proxy

# 设置 / 取消代理
git config --global http.proxy  http://127.0.0.1:7897
git config --global https.proxy http://127.0.0.1:7897
git config --global --unset http.proxy
git config --global --unset https.proxy
```

```powershell
# 网络排查三连
Resolve-DnsName github.com                                # DNS 是否正常
Test-NetConnection github.com -Port 443                   # ICMP + TCP 是否可达
curl.exe -x http://127.0.0.1:7897 https://github.com       # 代理是否可用

# 找本机代理端口
Get-Process | ? ProcessName -match 'clash|verge'
Get-NetTCPConnection -State Listen | ? LocalPort -in 7890,7897,1080
```

---

## 9. 经验总结 (Takeaways)

1. **能 ping 通 ≠ 能上网**：ICMP 通不代表 TCP 通，排查网络必须分层看。
2. **报错 `Couldn't connect to server`** 优先怀疑链路阻断 / 防火墙，而非目标服务故障。
3. **本机有代理软件 ≠ git 走代理**：git 默认直连，需显式配置 `http.proxy`。
4. **代理软件要保持开启**，否则 git 指向的本地端口失效，又会失败。
5. **绕过的关键是"目标不可识别"**，加密只是手段之一。

---

*Recorded from an interactive troubleshooting session. Companion note for the
`computer-network/` section of this repo.*
