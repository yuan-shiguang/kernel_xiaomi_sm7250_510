// SPDX-License-Identifier: GPL-2.0-only

/*
 * Copyright (c) 2016-2018, The Linux Foundation. All rights reserved.
 */

#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <soc/qcom/rpmh.h>
#include <clocksource/arm_arch_timer.h>
#include <soc/qcom/lpm_levels.h>

#include "rpmh_master_stat.h"

#define PDC_TIME_VALID_SHIFT	31
#define PDC_TIME_UPPER_MASK	0xFFFFFF

static struct device *dev;

/*
 * 5.10 removed the RPMH PDC-write / controller-idle / flush helpers and
 * the memory-mapped arch-timer cval accessor this driver relied on.
 * Provide local equivalents so the driver builds: the timer cval is read
 * from the system counter; the RPMH PDC/idle/flush hooks are deferred
 * (permissive) until the PDC/timer integration is ported.
 */
static void arch_timer_mem_get_cval(uint32_t *lo, uint32_t *hi)
{
	u64 cval = arch_timer_read_counter ? arch_timer_read_counter() : 0;

	*lo = (u32)cval;
	*hi = (u32)(cval >> 32);
}

static int rpmh_write_pdc_data(struct device *dev,
			       const struct tcs_cmd *cmd, u32 n)
{
	return 0;
}

static bool rpmh_ctrlr_idle(struct device *dev)
{
	return true;
}

static int rpmh_flush(struct device *dev)
{
	return 0;
}

static int setup_wakeup(uint32_t lo, uint32_t hi)
{
	struct tcs_cmd cmd[2] = { { 0 } };

	cmd[0].data =  hi & PDC_TIME_UPPER_MASK;
	cmd[0].data |= 1 << PDC_TIME_VALID_SHIFT;
	cmd[1].data = lo;

	return rpmh_write_pdc_data(dev, cmd, ARRAY_SIZE(cmd));
}

static int system_sleep_update_wakeup(bool from_idle)
{
	uint32_t lo = ~0U, hi = ~0U;

	/* Read the hardware to get the most accurate value */
	arch_timer_mem_get_cval(&lo, &hi);

	return setup_wakeup(lo, hi);
}

/**
 * system_sleep_allowed() - Returns if its okay to enter system low power modes
 */
static bool system_sleep_allowed(void)
{
	return rpmh_ctrlr_idle(dev);
}

/**
 * system_sleep_enter() - Activties done when entering system low power modes
 *
 * Returns 0 for success or error values from writing the sleep/wake values to
 * the hardware block.
 */
static int system_sleep_enter(struct cpumask *mask)
{
	return rpmh_flush(dev);
}

/**
 * system_sleep_exit() - Activities done when exiting system low power modes
 */
static void system_sleep_exit(bool success)
{
	if (success)
		msm_rpmh_master_stats_update();
}

static struct system_pm_ops pm_ops = {
	.enter = system_sleep_enter,
	.exit = system_sleep_exit,
	.update_wakeup = system_sleep_update_wakeup,
	.sleep_allowed = system_sleep_allowed,
};

static int sys_pm_probe(struct platform_device *pdev)
{
	dev = &pdev->dev;
	return register_system_pm_ops(&pm_ops);
}

static const struct of_device_id sys_pm_drv_match[] = {
	{ .compatible = "qcom,system-pm", },
	{ }
};

static struct platform_driver sys_pm_driver = {
	.probe = sys_pm_probe,
	.driver = {
		.name = KBUILD_MODNAME,
		.suppress_bind_attrs = true,
		.of_match_table = sys_pm_drv_match,
	},
};
builtin_platform_driver(sys_pm_driver);

/*
 * [VENDOR-PORT Phase7] system PM ops registry + deep-idle latency.
 *
 * Both live in drivers/cpuidle/lpm-levels.c in REF, which is not built in this
 * tree.  system_pm.o currently references register_system_pm_ops() as an
 * undefined symbol, and techpack/audio/{swr-mstr-ctrl,wcd9xxx-irq,wcd_cpe_core,
 * sdm660-cdc-irq}.c read the deep-idle latency to size their timeouts, so the
 * two need real definitions rather than the void stubs they used to hit.
 *
 * Semantics are REF's: a single registry slot (EUSERS when already taken) and
 * a fixed 2 ms deep-idle exit latency.  The PSCI deep-idle path that would
 * consume sys_pm_ops is part of the unported lpm-levels.c, so the registry is
 * effectively write-only today - same as in REF.
 */
static struct system_pm_ops *sys_pm_ops;

uint32_t register_system_pm_ops(struct system_pm_ops *pm_ops)
{
	if (sys_pm_ops)
		return -EUSERS;

	sys_pm_ops = pm_ops;

	return 0;
}
EXPORT_SYMBOL(register_system_pm_ops);

s32 msm_cpuidle_get_deep_idle_latency(void)
{
	return 2;
}
EXPORT_SYMBOL(msm_cpuidle_get_deep_idle_latency);
