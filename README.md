# douyin-algorithm

## 说明

仓库不再更新了，有我开源的这些，有需求的自己分析算法也没啥问题了。

另外 douyin 和 tiktok 算法都差不多，分析一份，改改就能适配另一个。

仓库代码中 sm3 的算法有点问题，某些情况计算的不对，可以改用 digestpp。

## com.ss.android.ugc.aweme 23.2.0

当前分析的版本是 douyin_23.2.0_arm64

这是最后一个 4 神版本 `X-Ladon`, `X-Argus`, `X-Gorgon`, `X-Khronos`

- [x] `X-Ladon`（生成）
- [x] `X-Argus`（生成与解包，包括 protobuf 中的加密字段，26.2 的 8 种算法）
- [x] `X-Gorgon`
- [x] `X-Khronos`

## com.ss.android.ugc.aweme 23.3.0

从 douyin_23.3.0 开始变成 6 神了，新版本多了两个参数 `X-Helios`, `X-Medusa`

- [ ] `X-Helios`
- [ ] `X-Medusa`
