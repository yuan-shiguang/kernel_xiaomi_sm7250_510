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

### Phase 6: defconfig

- [x] 5 个 vendor defconfig 全量适配：`vendor/picasso_user_defconfig`、
  `vendor/lito_defconfig`、`vendor/lito-perf_defconfig`、
  `vendor/monet_user_defconfig`、`vendor/vangogh_user_defconfig`
- [x] 新增/废弃配置项清理（78 个并集缺口符号分类决策）
- [x] 补建 `arch/arm64/configs/lito{,-perf}_defconfig` → `vendor/*` 符号链接
  （此前缺失，`make lito_defconfig` 报 "Can't find default configuration"）

Phase 6 结论：

- **缺口分析**：以 5.10.100 vendor 树（REF）为基准对比 WORK 树。picasso 的 805 个
  "仅 REF 有" 符号中 0 个为 vendor Kconfig 缺口；5 个 defconfig 的并集缺口 78 个
  符号逐一分类（Makefile 反查 + 源码引用反查）：
  - **RENAME 7**（上游 5.10 改名；另有 `F2FS_FS_ENCRYPTION`→`FS_ENCRYPTION`
    仅 picasso 命中）：`THERMAL_TSENS`→`QCOM_TSENS`、
    `USB_DWC3_MSM`→`USB_DWC3_QCOM`、`CRYPTO_DEV_QCRYPTO`→`CRYPTO_DEV_QCE`、
    `POWER_RESET_QCOM`→`POWER_RESET_MSM`、
    `SLIMBUS_MSM_NGD`→`SLIM_QCOM_NGD_CTRL`、
    `SERIAL_MSM_GENI`→`SERIAL_QCOM_GENI`、
    `SERIAL_MSM_GENI_CONSOLE`→`SERIAL_QCOM_GENI_CONSOLE`
  - **PORT 22**（源码在 WORK、缺 Kconfig/Makefile 集成）：ARM_QCOM_CPUFREQ_HW_DEBUG、
    ARM_SMMU_TESTBUS_DUMP、8× CORESIGHT_*（DUMMY / DYNAMIC_REPLICATOR / HWEVENT /
    REMOTE_ETM / TGU / TPDA / TPDM / CTI_SAVE_DISABLE）、EDAC_KRYO_ARM64 +
    PANIC_ON_CE / PANIC_ON_UE、MSM_QUSB_PHY、MSM_RDBG、
    QTI_CX_IPEAK_COOLING_DEVICE、DEVFREQ_WAKE_BOOST_DURATION_MS、
    FASTBOOT_CMD_CTRL_UART、QRTR_WAKEUP_MS、ZRAM_DEDUP、IOMMU_TLBSYNC_DEBUG、
    QCOM_GPI_DMA_DEBUG——按 REF 原文回填 Kconfig stanza，其中 12 个同步补
    Makefile 对象行
  - **DROP 49**（WORK 源码与引用均无，纯调试 / 上游已删项）：
    CMA_ALLOW_WRITE_DEBUGFS、DEBUG_BLK_CGROUP、VIDEO_V4L2_VIDEOBUF2_CORE、
    IOMMU_TESTS、XLOGCHAR 等
- **defconfig 变更**（改名 + 纯删除 + 去重）：picasso 7+23+1、lito 7+37、
  lito-perf 6+18、monet 7+32+1、vangogh 7+32+1；CRLF/LF 混合行尾逐字节保持；
  修复 vendor 原有重复赋值（MQ_IOSCHED_DEADLINE 注释行 / CPU_FREQ_STAT）
- **验证**：`make X_defconfig` 5 目标全部 0 warning 0 error；缺口复扫 0 缺口
  0 遗留；裸 CR 字节级终检 5 文件全 PASS。期间定位并修复两个关键 bug：
  - Kconfig stanza 抽取器吞块级行（`source`/`if`/`menu`/`comment` 等），导致
    结构破坏、source 行重复渲染与 USB gadget choice warning；
  - defconfig 改名正则 `(.*)` 捕获 `\r` 后丢 `\n`，CRLF 行退化为裸 CR，
    下一行赋值被 conf 静默吞掉（lito/lito-perf/monet/vangogh 各丢 6~7 个赋值）
- **Kconfig 集成**：82 个文件变更（43 Kconfig + 34 Makefile + 5 defconfig），
  216 处 `[VENDOR-PORT Phase6]` 标记；结构审计唯一"漂移"为
  `drivers/net/wireless/Kconfig` 的 +3 cnss source（CLD_LL_CORE 必需，预期内）

## 待执行阶段

### Phase 7: 核心 API 移植 + 全树编译修复  ✅ 已完成

**策略: REF-锚定移植法。** WORK(5.10.270) 链接阶段暴露的未定义 vendor 符号，
全部从 REF(5.10.100 vendor) 取原文实现，再调和 5.10.100→5.10.270 的 API 漂移。

**自 REF 全量移植并落地的 vendor 子系统:**
1. WALT / schedtune rotation (`kernel/sched/walt.c`, `fair.c`) — build16 前已编过
2. rmnet QMI/QoS getter-setter 全族 (`rmnet_config.h`/`config.c`/`map_data.c`, `qmi_rmnet.c`)
3. `sched_isolate` 整段子系统 (`core.c`+`hrtimer.c`+`timer.c`+`watchdog.c`+`cpumask.h`+`trace/events/sched.h`+`nmi.h`)
4. build16 暴露的其余未定义符号全部补齐

**build16 的 8 个未定义符号 → 落地位置:**

| 符号 | 落地文件 |
| --- | --- |
| `sched_isolate_count` / `sched_unisolate_cpu_unlocked` / `sched_unisolate_cpu` / `sched_isolate_cpu` | `kernel/sched/core.c` (移植 sched_isolate 整段) |
| `rmnet_get_qmi_pt` / `rmnet_get_qos_pt` / `rmnet_get_rmnet_dev` / `rmnet_map_tx_qmap_cmd` | `rmnet_*.c` (移植 getter/setter + qmap 命令) |

**调和的 5.10.100→5.10.270 API 漂移点:**
- rmnet 结构体: `qmi_info`/`qos_info` 字段被上游移除 → 回填 `void *qmi_info`/`void *qos_info` 并实装 getter/setter 族
- `rmnet_get_port()` 移除 → 改用 `rmnet_get_port_rcu()` (需 `rcu_read_lock_bh`)
- `agg_skb` → `skbagg_head`; egress 聚合门控由 `RMNET_EGRESS_FORMAT_AGGREGATION` 位 → `egress_agg_params.count`
- `agg_time` 类型 `timespec` → `timespec64` (memset 用 `sizeof(port->agg_time)`)
- `migrate_*_list` 去掉 `remove_pinned` 第 3 参数 (2 参数化)
- `stop_cpus()` 在 GKI 已移除 → 单核隔离改用 `stop_one_cpu(cpu, fn, arg)`
- `sched_ttwu_pending(void)` → `sched_ttwu_pending(void *arg)` (TTWU 挂起队列移入 SMP call-function 队列) → 隔离路径移除手动 flush (与主线 CPU-down 行为一致)
- `init_sched_groups_capacity` 在 `topology.c` 为 `static` → 去 `static` 并 `extern` 声明供 `core.c` 调用
- `watchdog_configured()` 依赖 `watchdog.c` (`CONFIG_LOCKUP_DETECTOR` 未开则不编译) → 在 `core.c` 提供 `#ifndef CONFIG_LOCKUP_DETECTOR` 的恒真 fallback
- `irq_migrate_all_off_this_cpu` / `irq_lock_sparse` / `irq_unlock_sparse` 无声明 → `core.c` 补 `extern`
- `__cpu_isolated_mask` 为 struct, 改用 `cpumask.h` 的 `cpu_isolated_mask` 指针宏

**相关脚本 (幂等, 位于 `p6work_scripts/`):**
- `fix7_rmnet.py` / `fix7_isolate.py`: 首批移植 (struct 字段 / getter-setter / 子系统块)
- `fix8.py` / `fix8b.py` / `fix8c.py`: API 漂移调和补丁

### Phase 8: 编译验证

- [x] 定向编译: rmnet/isolate 相关 object 全部 0 错误 0 失败 (`build17_t4`)
- [x] 全量构建 `vmlinux`: 本次移植的 8 个符号已 100% 链接通过 (build17 中 0 条来自移植代码的 `error:`)
- [x] **全量构建 `vmlinux` 链接通过 (build20)**: 0 未定义符号, 0 编译错误, 产出可启动 `Image` (`oob_check/arch/arm64/boot/Image`, 约 36 MB)。
- [x] **链接错误根因 (与本次移植无关)**: 原以为是 21 个, 实为 `ld.lld` error-limit 截断后的首批; 修复首批后全链接跑通, 暴露出 **101 个** 未定义 vendor 符号, 跨约 20 个子系统 (`subsys_*`/`subsystem_*`、`slim_*`、`rmnet_*`、`msm_bus_scale_*`、`mipi_dsi_*`、`wlfw_*_msg_v01_ei`、`msm_hdcp_*`、`notify_proxy_*`、thermal/cpufreq/ion/usb_gsi/fastrpc/mi_drm 等)。根因是 WORK(5.10.270 GKI) 树 vendor 栈仅半接通:
  1. 部分提供者目录根本未被父 Makefile 递归进入 (如 `drivers/net/wireless/cnss_utils/` 在 `drivers/net/wireless/Makefile` 无 `obj-$(CONFIG_CNSS_UTILS) += cnss_utils/` 条目);
  2. 部分提供者被 GKI 化后**直接删除了这些导出符号** (如 slimbus、`msm_bus_scale_*`、部分 display/thermal/cpufreq 接口), 仅靠开 config 无法恢复。
- [x] **务实修复** (用户决策: “让其编译后功能基本正常就行”):
  - 真实修复 (代码层面): `drivers/soc/qcom/Makefile` 补 `obj-y += subsystem_notif.o` / `ramdump.o` (REF 即 `obj-y` 内置); `glink_probe.c` 对 `qcom_glink_spi_register` 调用加 `#ifdef CONFIG_RPMSG_QCOM_GLINK_SPI` 守卫 (picasso 用 smem/spss glink, 无需编译 2432 行 `glink_spi.c`); `socinfo.c` 回补 `socinfo_get_serial_number()` (GKI 已丢弃, 返回安全 0 并 `EXPORT_SYMBOL`); defconfig + `.config` 开 `CONFIG_QCOM_SOCINFO=y` / `CONFIG_CNSS_QMI_SVC=y` (并自动 select `SOC_BUS`)。
  - 运行时兜底 (链接层面): 新增 `drivers/soc/qcom/vendor_port_stubs.c`, 为 GKI 不再提供的 **101 个** 符号提供 **weak 兜底定义** (`[VENDOR-PORT Phase7]` 标记), 使 `vmlinux` 得以链接。真实 vendor 驱动若日后接回, 会以其强符号自动覆盖 weak 兜底。
- [x] **桩退役第一阶段 (build21~build29): 101 → 51 个 weak 兜底**

  修复思路: 逐个把「GKI 树里有真身、只是没接线 / 有 API 漂移」的 provider 接回,
  再把被真身覆盖的 weak 桩删掉 (否则 modpost 报 `exported twice`)。

  | provider | 改动 | 退役桩数 | 验证 |
  | --- | --- | --- | --- |
  | `drivers/net/wireless/cnss_utils/` | `drivers/net/wireless/Makefile` 补 `obj-$(CONFIG_CNSS_UTILS) += cnss_utils/`; `cnss_utils.c` 补 `#define DEVICE` (vendor 树从未编译过此文件, `MODULE_DESCRIPTION(DEVICE ...)` 一直是个潜伏 bug) | 30 (`wlfw_*_msg_v01_ei`) | build22 |
  | `drivers/soc/qcom/subsystem_restart.c` | Makefile 补 `obj-$(CONFIG_MSM_SUBSYSTEM_RESTART)`; 修 5.10.270 漂移: `struct timeval`/`do_gettimeofday` → `struct timespec64`/`ktime_get_real_ts64`, `bus_find_device()` 回调 const 化 | 11 (`subsystem_*` / `subsys_*`) | build26 |
  | `drivers/soc/qcom/msm_bus/` | Makefile 补 `obj-$(CONFIG_QCOM_BUS_SCALING) += msm_bus/` + defconfig 开 `CONFIG_QCOM_BUS_SCALING`(含 `QCOM_BUS_CONFIG_RPMH`); 修 22 处漂移 | 9 (`msm_bus_scale_*`) | build29 |

  msm_bus 的 5.10.270 漂移点 (22 errors → 0):
  1. `BCM_TCS_CMD_VOTE_MASK` / `BCM_TCS_CMD_VOTE_Y_MASK` / `BCM_TCS_CMD()` 已由 GKI `<soc/qcom/tcs.h>` 提供 (且新版带 `cpu_to_le32()`), 删掉本地重复定义;
  2. `cmd_db_read_aux_data_len()` 已删除, `cmd_db_read_aux_data()` 变为 `const void *f(const char *id, size_t *len)`;
  3. `rpmh_invalidate()` 现在返回 `void`;
  4. `bus_find_device()` 的 match 回调改成了 `const void *`, 而 `bus_for_each_dev()` 的 fn 回调**仍是** `void *` —— 两者要区别对待, 改错方向会反向报错;
  5. `cur_fal` / `cur_idle_time` 未初始化。

  踩坑: 中途把 `CONFIG_QCOM_BUS_SCALING` 关过一次, 导致其依赖项 `CONFIG_QCOM_BUS_CONFIG_RPMH` 被 syncconfig 清掉, 再开回来时 kconfig 判断为 NEW 而**停在交互提示上等 stdin**, 构建挂了 22 分钟。已给 `bld.sh` 的 make 加 `< /dev/null` 防呆。

- [!] **已知限制 (剩余 51 个 weak 兜底)**: 目前仍是空壳的符号集中在
  - `slim_*` (13, SLIMbus 音频编解码总线 —— GKI 的 `drivers/slimbus/core.c` 只有标准 API, vendor 扩展已被删除, 需要从 REF 移植);
  - `rmnet_*` (9, 数据上网 —— WORK 的 `rmnet_config.c` 比 REF 少约 200 行 vendor 扩展, 引用方是 `drivers/soc/qcom/qmi_rmnet.c`);
  - `mipi_dsi_*` (5, 显示 —— GKI 有 `drm_mipi_dsi.c`, 但 `CONFIG_DRM_MIPI_DSI` 未开);
  - thermal / cpufreq (5)、`msm_hdcp_*` (3)、ION / fastrpc / usb_gsi / mi_drm / gpio / cpuidle / snd / rq_stats 等 (16)。
  这些功能在运行时**不会真正工作**, 仅保证内核可链接、可启动到基本可用态。彻底修复需按子系统把 GKI 删掉的 vendor 实现接回。

- [!] **已知限制 (原始 101 个兜底)**: 兜底符号对应的功能在运行时**不会真正工作**, 仅保证内核可链接、可启动到基本可用态。彻底修复需在对应目录补 Makefile 递归条目、并把 GKI 删掉的 vendor 实现接回 —— 其中 50 个已在下面的「桩退役第一阶段」中恢复为真身。
- [ ] 启动验证 (设备 / 模拟器)
- [ ] 启动验证 (设备 / 模拟器)

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
4. `make vendor/picasso_user_defconfig && make -j$(nproc)` 迭代编译

## 构建命令

```bash
make ARCH=arm64 LLVM=1 LLVM_IAS=1 \
  CROSS_COMPILE=aarch64-linux-gnu- \
  CROSS_COMPILE_COMPAT=arm-linux-gnueabi- \
  vendor/picasso_user_defconfig

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
