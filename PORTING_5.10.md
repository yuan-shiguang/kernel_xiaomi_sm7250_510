# 4.19 → 5.10 基线嫁接说明（Phase 2）

## 组成
| 层 | 来源 | 说明 |
|----|------|------|
| 核心基线 | `common-android12-5.10-lts/common` | ACK android12-5.10，5.10.270，Clang r416183b |
| vendor overlay | 4.19 树（`kernel_xiaomi_sm7250_510`） | 规则：**GKI 已有的一律保留 GKI 版**，仅补 GKI 缺失的 vendor 文件 |
| 手工合并 | build.config.common / build.config.aarch64 / arch/arm64/boot/dts/Makefile | 见下 |

## overlay 带入的 vendor 区（GKI 完全没有，整体带入）
- `techpack/`（audio 517 / camera 388 / display 284 / video 52 / data 63 / stub 3）
- `arch/arm64/boot/dts/vendor/`（qcom bengal + xiaomi monet/picasso/vangogh + bindings）
- `arch/arm64/configs/vendor/`（17 个 defconfig，Phase 1 已做 CFQ→BFQ_CGROUP、IOSCHED_DEADLINE→MQ_*）
- `drivers/platform/msm/`（QCOM vendor 平台驱动）
- `drivers/staging/qcacld-3.0/`（WLAN 主体，~4500 文件）
- `android/`、`Android.mk`、`AndroidKernel.mk`、`Androidbp`、`build.sh`、`kernel_headers.py`、`gen_headers_*.bp`

## dts 构建说明
ACK common 删掉了 DTB 构建（GKI 是通用镜像），本树已在
`arch/arm64/boot/dts/Makefile` 补回 `subdir-y += vendor` 入口；
`dtbs` 目标由顶层 Makefile 提供（`$(MAKE) $(build)=$(dtstree)`）。
`build.sh link_all_dtb_files` 依赖 `$OUT/arch/arm64/boot/dts/vendor/qcom/*.dtb`。

## 已知待办（Phase 3 起）
1. 共享目录（`drivers/soc/qcom`、`clk/qcom`、`pinctrl/qcom`、`phy/qualcomm`、`interconnect/qcom` …）
   GKI 版优先，4.19 的 QCOM 特化 .c **未被** 放入 → 这些模块当前不在 5.10 构建内，
   需逐模块把 4.19 实现移植到 5.10 API 后并入对应 Makefile/Kconfig。
2. techpack / qcacld 的 4.19 代码需要在 5.10 API 下编译修复
   （ALSA/ASoC、DRM/KMS、V4L2、cfg80211 均有 4.19→5.10 变更）。
3. 17 个 defconfig 仍含 4.19 专有 CONFIG，首次 `make savedefconfig` 会报未知符号，需清理。
4. `arch/arm64/Makefile` 未恢复 4.19 的 `Image.gz-dtb`/`dtbo.img` 目标；
   需要时从 4.19 树取回对应行。
