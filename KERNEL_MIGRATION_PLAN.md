# 内核移植计划: 4.19 → 5.10

## 项目概况
- **平台**: SM7250 (Bengal) — 高通骁龙 765G
- **设备**: monet, picasso, vangogh (Xiaomi)
- **当前版本**: 4.19.157
- **目标版本**: 5.10.x (Android 12 ACK)
- **构建**: LLVM/Clang + GKI

---

## 已完成 (Phase 1: 核心构建系统)
- [x] `Makefile` 版本号 `4.19.157` → `5.10.100`，NAME → "Trick or Treat"
- [x] `build.config.common` BRANCH → `android12-5.10`
- [x] `build.config.common` Clang 版本 `r399163b` → `r416183b`
- [x] `build.config.aarch64` 添加 `LLVM_IAS=1`
- [x] `Makefile` 移除 `PYTHON2` / `old_version_h`
- [x] `Makefile` `version.h` 生成公式启用 `SUBLEVEL`

---

## 待执行阶段

### Phase 2: arch/arm64 架构层
- [ ] `arch/arm64/Kconfig` — 补充 5.10 新增配置项 (ARCH_HAS_NONLEAF_PMD_YOUNG, ARM64_SW_TTBR0_PAN 等)
- [ ] `arch/arm64/Makefile` — 构建标志更新 (`-march=armv8.5-a`, `gcc10`/`clang` 兼容)
- [ ] `arch/arm64/kernel/` — entry.S, setup.c, smp.c 适配
- [ ] `arch/arm64/mm/` — 页表/内存管理适配 (folio 引入前的兼容层)
- [ ] `arch/arm64/boot/dts/Makefile` — DTS 编译规则

### Phase 3: 高通平台驱动
- [ ] `drivers/soc/qcom/` (134 c) — interconnect/clk/pinctrl/regulator/remoteproc/rpmsg API 适配
- [ ] `drivers/platform/msm/` (199 c) — USB/I2C/SPI/thermal/IPC 适配
- [ ] `drivers/clk/qcom/` — clk_hw 框架
- [ ] `drivers/pinctrl/qcom/` — pinctrl 框架
- [ ] `drivers/phy/qualcomm/` — phy 框架

### Phase 4: Techpack 移植
- [ ] `techpack/audio/` — ALSA/ASoC 框架
- [ ] `techpack/display/` — DRM/KMS/mdss
- [ ] `techpack/camera/` — V4L2/media
- [ ] `techpack/video/` — V4L2 M2M

### Phase 5: 设备树
- [ ] `arch/arm64/boot/dts/vendor/qcom/bengal-*.dtsi`
- [ ] `arch/arm64/boot/dts/vendor/xiaomi/*.dtsi`
- [ ] 绑定兼容性 (interrupt-cells, #address-cells 等)

### Phase 6: defconfig
- [ ] `vendor/bengal_defconfig`, `*_user_defconfig`
- [ ] 新增/废弃配置项清理

### Phase 7: 核心 API 修复
- [ ] 内存: page→folio 迁移 (5.10 引入 folio)
- [ ] 时间: timer_setup / ktime / timekeeping
- [ ] I/O: iov_iter, read_iter/write_iter
- [ ] 并发: atomic/refcount/lockdep
- [ ] 设备: gpiod, clk_hw, regulator

### Phase 8: 编译验证
- [ ] LLVM/Clang 编译
- [ ] 迭代修复编译/链接错误
- [ ] 启动验证

---

## 关键 API 变更速查 (4.19 → 5.10)

| 子系统 | 4.19 | 5.10 |
|--------|------|------|
| 内存 | `SetPagePrivate` | `attach_page_private` |
| 内存 | `page_cache_release` | `put_page` |
| 时钟 | `clk_register` | `devm_clk_hw_register` |
| GPIO | `gpio_request` | `gpiod_get` |
| 定时器 | `setup_timer` | `timer_setup` |
| I/O | `struct iovec` | `struct iov_iter` + iter API |
| DRM | `drm_crtc_init` | `drmm_crtc_init_with_planes` |
| ASoC | `snd_soc_register_card` | 同 (参数微调) |
|  regulator | `regulator_register` | `devm_regulator_register` |

---

## 推荐工作流
1. 以 `android12-5.10-stable` 为基准，建立新分支 `kernel-5.10`
2. 将本仓库 vendor 代码 (techpack, dts/vendor, soc/qcom, platform/msm) 移植到 5.10 基准
3. 按 Phase 顺序逐一修复 API 不兼容
4. `make vendor/bengal_defconfig && make -j$(nproc)` 迭代编译

## 构建命令
```bash
make ARCH=arm64 LLVM=1 LLVM_IAS=1 \
  CROSS_COMPILE=aarch64-linux-gnu- \
  CROSS_COMPILE_COMPAT=arm-linux-gnueabi- \
  vendor/bengal_defconfig

make ARCH=arm64 LLVM=1 LLVM_IAS=1 -j$(nproc)
```
