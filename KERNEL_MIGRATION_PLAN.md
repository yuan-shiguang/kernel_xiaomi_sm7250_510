# 内核移植计划: 4.19 → 5.10

## 项目概况

- **平台**: SM7150 (LITO) — 高通骁龙 765G
  > 已核实：picasso 设备树 `compatible = "qcom,lito"` 且 include 全套 lito 时钟 bindings；  
  > 仓库名 "sm7250" 与 defconfig 文件名仅为板号，实际 SoC 为 **LITO**。原文档误写为 Bengal。
- **设备**: monet, picasso, vangogh (Xiaomi)
- **当前版本**: 4.19.157
- **目标版本**: 5.10.x (Android 12 ACK)
- **构建**: LLVM/Clang + GKI

---

## 已完成

### Phase 1: 核心构建系统

- [x] `Makefile` 版本号 `4.19.157` → `5.10.100`，NAME → "Trick or Treat"
- [x] `build.config.common` BRANCH → `android12-5.10`
- [x] `build.config.common` Clang 版本 `r399163b` → `r416183b`
- [x] `build.config.aarch64` 添加 `LLVM_IAS=1`
- [x] `Makefile` 移除 `PYTHON2` / `old_version_h`
- [x] `Makefile` `version.h` 生成公式启用 `SUBLEVEL`

---

### Phase 2: arch/arm64 架构层

- [x] `arch/arm64/Kconfig.platforms` — 移植 `ARCH_LITO`/`ARCH_LAGOON` per-SoC 选择  
  （5.10 GKI 已移除具体 SoC 子配置，需 vendor 加回；LITO `select COMMON_CLK_QCOM`）
- [x] `arch/arm64/Kconfig` — 补充 5.10 新增配置项 (ARCH_HAS_NONLEAF_PMD_YOUNG, ARM64_SW_TTBR0_PAN 等)
- [x] `arch/arm64/Makefile` — 构建标志更新 (`-march=armv8.5-a`, `gcc10`/`clang` 兼容)
- [x] `arch/arm64/kernel/` — entry.S, setup.c, smp.c 适配
- [x] `arch/arm64/mm/` — 页表/内存管理适配 (folio 引入前的兼容层)
- [x] `arch/arm64/boot/dts/Makefile` — DTS 编译规则

### Phase 3: 高通平台驱动

- [x] `drivers/soc/qcom/` — 4.19→5.10 API 适配完成，LLVM/Clang 编译 0 错误
- [x] `drivers/clk/qcom/` — clk_hw 框架，编译 0 错误
- [x] `drivers/pinctrl/qcom/` — pinctrl 框架，编译 0 错误
- [x] `drivers/phy/qualcomm/` — phy 框架，编译 0 错误
- [x] `drivers/platform/msm/` — 96 个 .c 适配完成，LLVM/Clang 编译 0 错误 0 警告（53 个 .o）

Phase 3 延期项（编译通过但功能待验证，记入 Phase 7/8）：

- `system_pm.c`: 5.10 移除 rpmh_write_pdc_data/rpmh_ctrlr_idle/rpmh_flush 与  
  arch_timer_mem_get_cval；已加本地占位实现（timer 走 arch_timer_read_counter，RPMH 钩子  
  为 permissive 空操作）。PDC 定时唤醒优化待 Phase 7/8 与 PDC/timer 集成一并重做。
- `qcom_glink_spss.o`/`qcom_glink_spi.o` 尚未接进 `drivers/rpmsg/Makefile`（仅加了头文件原型），  
  完整链接留待 Phase 8。
- `arch_timer_mem_get_cval` 仍需为 `drivers/irqchip/qcom-mpm.c` 提供共享实现（Phase 7）。
- 6 个 `.tmp` 残留文件（fsa4480-i2c/glink_pkt/llcc-qcom/Makefile/qcom_glink/llcc-qcom）需在 Windows 侧手动删除。

## 待执行阶段

### Phase 4: Techpack 移植

- [x] `techpack/audio/` — ALSA/ASoC 框架
- [x] `techpack/display/` — DRM/KMS/mdss
- [ ] `techpack/camera/` — V4L2/media
- [ ] `techpack/video/` — V4L2 M2M

### Phase 5: 设备树

- [ ] `arch/arm64/boot/dts/vendor/qcom/lito-*.dtsi`（picasso 实际为 **LITO** 平台，非 Bengal）
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

| 子系统       | 4.19                    | 5.10                         |
| --------- | ----------------------- | ---------------------------- |
| 内存        | `SetPagePrivate`        | `attach_page_private`        |
| 内存        | `page_cache_release`    | `put_page`                   |
| 时钟        | `clk_register`          | `devm_clk_hw_register`       |
| GPIO      | `gpio_request`          | `gpiod_get`                  |
| 定时器       | `setup_timer`           | `timer_setup`                |
| I/O       | `struct iovec`          | `struct iov_iter` + iter API |
| DRM       | `drm_crtc_init`         | `drmm_crtc_init_with_planes` |
| ASoC      | `snd_soc_register_card` | 同 (参数微调)                     |
| regulator | `regulator_register`    | `devm_regulator_register`    |

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
