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

---

### Phase 3: 高通平台驱动

- [x] `drivers/soc/qcom/` — 4.19→5.10 API 适配完成，LLVM/Clang
- [x] `drivers/clk/qcom/` — clk_hw 框架
- [x] `drivers/pinctrl/qcom/` — pinctrl 框架
- [x] `drivers/phy/qualcomm/` — phy 框架
- [x] `drivers/platform/msm/` — 96 个 .c 适配完成，LLVM/Clang 

---

### Phase 4: Techpack 移植
- [x] `techpack/audio/` — ALSA/ASoC 框架
- [x] `techpack/display/` — DRM/KMS/mdss
- [x] `techpack/camera/` — V4L2/media
- [x] `techpack/video/` — V4L2 M2M

---

### Phase 5: 设备树

- [x] `arch/arm64/boot/dts/vendor/qcom/lito-*.dtsi`（picasso 实际为 **LITO** 平台，非 Bengal）
- [x] `arch/arm64/boot/dts/vendor/xiaomi/*.dtsi`
- [x] 绑定兼容性 (interrupt-cells, #address-cells 等)

Phase 5 结论：

- `make dtbs` 全部通过 —— 21 个 dtb 产出，含 `picasso-sm7250.dtb`(575KB)、
  `monet-sm7250.dtb`、`vangogh-sm7250.dtb`、`lito-*` 全系列、`lagoon-*` 全系列。
- 失败迭代：`dt01`(1 处 thermal/lagoon 语法错) → `dt02`(2 处 ADC) → `dt03`(1 处 ADC_SBUx) → `dt04` 0 错误。
- 根因全部是 **5.10 上游删/改了 dt-binding 常量**，vendor DTS 一字未动：
  - `include/dt-bindings/thermal/thermal.h`：恢复 `THERMAL_MAX_LIMIT`
    （5.10 只留 `THERMAL_NO_LIMIT`，上游删掉了它）。
  - `include/dt-bindings/clock/qcom,rpmh.h`：从 15 起追加 LITO 专用时钟
    `RPMH_RF_CLKD3/D4/CLK4/CLK5`、`RPMH_QLINK_CLK`、`RPMH_LN_BB3` 共 12 个。
    ⚠️ 未改动上游已有编号（0–14），因为 `drivers/clk/qcom/clk-rpmh.c` 的
    `rpmh_clocks[]` 按下标寻址。
  - `include/dt-bindings/iio/qcom,spmi-vadc.h`：上游把通道命名从 `ADC_XXX`
    改成了 `ADC5_XXX` 并删掉了 `ADC_SCALE_*` 组。按枚举值等价补了 30 条
    `ADC_XXX -> ADC5_XXX` 兼容别名（`_PU1/_PU2/_PU3` → `_30K_PU/_100K_PU/_400K_PU`），
    并把 `ADC_SCALE_*` 组整块补回。
- C 侧影响面已回归：`drivers/iio/adc/`、`drivers/clk/qcom/`、`drivers/soc/qcom/`
  编译 0 错误 0 失败。
- 剩余 1505 条 dtc 告警（`avoid_default_addr_size` 506 / `graph_port` 390 /
  `gpios_property` 290 / `reg_format` 259 / `spi_bus_reg` 20 /
  `pci_device_bus_num` 20）**不是移植引入**：两棵树的 DTS 源码逐字节一致，
  4.19 用的是更老的 dtc。已反编译 `picasso-sm7250.dtb` 抽查，`reg`
  （如 `tx-macro@3220000` 的 `<0x3220000 0x0>`）在 dtb 中忠实还原，语义未变；
  这些是 QCOM vendor DTS 惯用写法，留待 Phase 8 按需清理。

## 待执行阶段

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

已验证的各子系统编译命令（结果均 0 错误 0 失败）：

```bash
# techpack（Phase 4）
make -k ... techpack/audio/ techpack/display/ techpack/video/ techpack/camera/
# 设备树（Phase 5）
make -k ... dtbs
# 回归：dt-binding 改动的 C 侧影响面
make -k ... drivers/iio/adc/ drivers/clk/qcom/ drivers/soc/qcom/
```
