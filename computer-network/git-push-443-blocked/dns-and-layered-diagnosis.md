# 分层排查法与 DNS 诊断

> How a single `git push` failure was diagnosed: the tool used, and the
> bottom-up layered troubleshooting methodology behind it.
>
> 一次 `git push` 失败的排查中，DNS 这一步用的是什么工具、遵循的是什么思路。
> 本篇抽离出**可复用的排障方法**，与具体案例解耦。

---

## 1. 用的工具 (Tool)

```powershell
Resolve-DnsName github.com
```

- **`Resolve-DnsName`** 是 **Windows PowerShell 内置命令**（属于 `DnsClient` 模块），相当于 Linux / macOS 上的 `dig` 或 `nslookup`。
- 它做的是：向系统配置的 DNS 服务器发起一次 **A 记录查询**，把域名（`github.com`）解析成 IP 地址。
- **为什么选它**：输出结构化（对象而非纯文本）、可脚本化、无需额外安装。等价的 `nslookup` / `dig` 也可以，但在 Windows 上不如它好处理。

**输出示例：**

```text
Name        IPAddress        Type
github.com  20.205.243.166   A
```

---

## 2. 排查思路：自下而上的分层排查法 (Bottom-up Layering)

核心逻辑：**沿数据包的路径，从"最底层、最便宜、最先出问题"的环节开始，逐个排除。**

一个 TCP 连接的建立，天然要经过这几步：

```text
① DNS 解析       域名 → IP             先得知道连哪
      ↓
② 三层可达 (ICMP)  IP 能不能到           路通不通
      ↓
③ 四层可达 (TCP)   端口能不能握手         门开不开
      ↓
④ 应用层配置       代理/工具用对没有        有没有走错路
```

每一步只回答一个是非问题，把可能性一步步收敛。

---

## 3. 为什么从 DNS 开始？(Why DNS First)

1. **它是前置条件**：DNS 都解析不出来，后面 IP、端口根本无从谈起——花再多时间查端口也是白搭。
2. **它能一票区分故障类型，避免瞎猜**：

   | DNS 结果 | 说明 | 下一步 |
   |----------|------|--------|
   | 解析失败 / 返回错误 IP | DNS 污染、DNS 服务器挂了 | 先修 DNS |
   | 解析正常 | 域名 → IP 这步没问题 | 继续查连通性 |

3. **成本最低**：一条命令、毫秒级返回，先用最小代价排除掉一大类可能性。

---

## 4. 在本案例中的结论 (Outcome)

```text
Name        IPAddress        Type
github.com  20.205.243.166   A
```

解析成功，且拿到的是 GitHub 亚太节点 IP。所以：

> **DNS 环节没问题 → 故障不在"找路"，而在"路走不走得通"。**

排查范围由此缩小到 ②③④：接着才去 ping（②）和测端口（③），最终定位到 **TCP 443 被拦**，且 **git 没走代理**（④）。

案例全文见同目录下的 [git-push-443-blocked.md](./git-push-443-blocked.md)。

---

## 5. 一句话总结 (Summary)

> **"先确认能不能找到地址，再确认路通不通，最后确认是不是走错了路。"**

自下而上、每层只回答一个是非问题，把可能性一步步收敛。这是网络排障最通用、最不容易漏环节的套路。

---

## 6. 可复用检查清单 (Reusable Checklist)

| 层 | 要回答的问题 | 常用工具 / 命令 |
|----|--------------|-----------------|
| ① DNS | 域名能否解析成正确 IP？ | `Resolve-DnsName`、`nslookup`、`dig` |
| ② 三层可达 | IP 能否到达？ | `ping`、`Test-Connection`、`traceroute`/`tracert` |
| ③ 四层可达 | 端口能否握手？ | `Test-NetConnection -Port`、`curl`、`telnet` |
| ④ 应用层配置 | 代理 / 工具是否走对路径？ | `git config --get http.proxy`、`$env:HTTPS_PROXY` |
