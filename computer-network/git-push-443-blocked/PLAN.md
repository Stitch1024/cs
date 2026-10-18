# 学习计划：逐步复现 443 连接失败排查 (Learning Plan)

> Goal: reproduce the `git push` 443 failure diagnosis **one layer at a time**, learn
> by doing, and turn each verified step into its own note.
>
> 目标：按排查思路**逐层复现**，边跑边学，每层沉淀成一篇文档。

---

## 规则 (Ground Rules)

| 项 | 选择 |
|----|------|
| 文档组织 | **每层一个独立 md**（`01-*.md`、`02-*.md` …） |
| 命令执行 | **用户手动跑**，把输出贴回来，AI 逐行解读后写文档 |
| 存放位置 | `cs/computer-network/git-push-443-blocked/` |

**每层的固定流程：**

```text
选一层 → 用户跑命令 → 贴回输出 → AI 解读 → 得出结论/下一步 → 写成该层 md
```

**每篇 md 的固定结构：** 目的 → 命令 →（真实）输出 → 解读 → 结论 → 下一步。

---

## 分层路线 (The Layers)

自下而上：`DNS → 三层可达 → 四层可达 → 应用层配置`。

### [ ] Step 1 — DNS 解析 (`01-dns.md`)

- **问题**：能不能把 `github.com` 解析成 IP？
- **命令**：
  ```powershell
  Resolve-DnsName github.com -Type A
  Resolve-DnsName github.com -Type A -Server 8.8.8.8   # 对比是否被污染
  ```
- **关注**：拿到什么 IP？本地 DNS 与 8.8.8.8 是否一致？
- **预期结论**：解析正常 → 故障不在"找路"，排除 DNS 环节。

### [ ] Step 2 — 三层可达性 / ICMP (`02-icmp.md`)

- **问题**：IP 层能不能到达 github.com？
- **命令**：
  ```powershell
  ping github.com
  Test-Connection github.com
  ```
- **关注**：能否收到回包？RTT 多少？
- **陷阱**：**ping 通 ≠ TCP 通**。这层通只说明三层可达。

### [ ] Step 3 — 四层可达性 / TCP 端口 (`03-tcp-port.md`)

- **问题**：443 端口能不能握手？
- **命令**：
  ```powershell
  Test-NetConnection github.com -Port 443
  ```
- **关注**：`TcpTestSucceeded` 是 True 还是 False？`PingSucceeded` 与它是否矛盾？
- **预期结论**：`PingSucceeded=True` 但 `TcpTestSucceeded=False` → **四层被拦**，问题定性。

### [ ] Step 4 — 代理配置检查 + 定位代理端口 (`04-proxy-config.md`)

- **问题**：git 有没有走代理？本机代理监听在哪个端口？
- **命令**：
  ```powershell
  git config --global --get http.proxy
  git config --global --get https.proxy
  $env:HTTP_PROXY; $env:HTTPS_PROXY
  Get-Process | ? ProcessName -match 'clash|verge'
  Get-NetTCPConnection -State Listen | ? LocalPort -in 7890,7897,1080
  ```
- **关注**：git 代理是否为空？Clash 监听端口是多少？

### [ ] Step 5 — 代理可用性验证 (`05-proxy-verify.md`)

- **问题**：通过代理能否真正访问 GitHub？
- **命令**：
  ```powershell
  curl.exe -x http://127.0.0.1:7897 -s -o NUL -w "HTTP %{http_code}\n" https://github.com
  ```
- **关注**：返回 200？耗时多少？
- **预期结论**：代理链路可用 → 配置 git 走代理即可修复。

### [ ] Step 6 — 汇总收敛 (`06-summary.md`)

- 把 1–5 的结论串起来，形成完整因果链与流量拓扑。
- 与已有文档交叉引用：
  - [git-push-443-blocked.md](./git-push-443-blocked.md)（案例总览）
  - [dns-and-layered-diagnosis.md](./dns-and-layered-diagnosis.md)（方法论）

---

## 进度 (Progress)

- [ ] Step 1 — DNS 解析
- [ ] Step 2 — 三层可达性
- [ ] Step 3 — 四层可达性 / TCP 443
- [ ] Step 4 — 代理配置 + 端口定位
- [ ] Step 5 — 代理可用性验证
- [ ] Step 6 — 汇总

**下次从这里开始**：Step 1 的 `Resolve-DnsName` 命令（含 8.8.8.8 对比）。
