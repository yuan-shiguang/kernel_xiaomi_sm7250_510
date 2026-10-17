/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 *
 * Compatibility shims for vendor kernel extensions that do not exist in the
 * upstream 5.10 tree.  Everything here is a *local* substitute so that the
 * techpack/display driver can be built without pulling extra Qualcomm
 * changes into the core kernel.
 */

#ifndef __MSM_COMPAT_H__
#define __MSM_COMPAT_H__

#include <linux/types.h>
#include <linux/cpumask.h>
#include <linux/kthread.h>
#include <linux/errno.h>

/*
 * Enum for display driver to provide varying refresh rates.
 * (4.19: include/linux/sched.h)
 */
#ifndef __MSM_COMPAT_FPS_DEFINED
#define __MSM_COMPAT_FPS_DEFINED
enum fps {
	FPS0 = 0,
	FPS30 = 30,
	FPS48 = 48,
	FPS60 = 60,
	FPS90 = 90,
	FPS120 = 120,
	FPS144 = 144,
};
#endif

/*
 * (4.19: include/linux/cpumask.h)
 * The vendor kernel exposed a "prime" cluster mask.  Upstream 5.10 has no
 * such concept, so alias it to the set of possible CPUs.
 */
#define cpu_prime_mask cpu_possible_mask

/*
 * (4.19: include/linux/kthread.h)
 * Create and wake a performance-critical thread.  PF_PERF_CRITICAL and the
 * vendor cpumask helpers are gone in 5.10, so this degrades to kthread_run().
 */
#ifndef kthread_run_perf_critical
#define kthread_run_perf_critical(perfmask, threadfn, data, namefmt, ...) \
	kthread_run(threadfn, data, namefmt, ##__VA_ARGS__)
#endif

/*
 * (4.19: drivers/of/fdt.c + include/linux/of_fdt.h)
 * Returns the DDR type encoded in the /memory node.  Without the vendor DT
 * binding we cannot know it, so report -ENOENT (callers fall back to the
 * default DDR configuration).
 */
static inline int of_fdt_get_ddrtype(void)
{
	return -ENOENT;
}

/*
 * (4.19: drivers/soc/qcom/rpmh.c + include/soc/qcom/rpmh.h)
 * Tells the RSC controller it runs in solver mode.  No-op on this tree.
 */
static inline int rpmh_mode_solver_set(const struct device *dev, bool enable)
{
	return 0;
}

/*
 * (4.19: include/drm/drm_fourcc.h)
 * 5.10 replaced the drm_format_*() accessors with struct drm_format_info.
 * Provide thin wrappers so the vendor code keeps working unchanged.
 */
#include <drm/drm_fourcc.h>

static inline int drm_format_num_planes(uint32_t format)
{
	const struct drm_format_info *info = drm_format_info(format);

	return info ? info->num_planes : 0;
}

static inline int drm_format_plane_cpp(uint32_t format, int plane)
{
	const struct drm_format_info *info = drm_format_info(format);

	if (!info || plane < 0 || plane >= info->num_planes)
		return 0;

	return info->cpp[plane];
}

static inline int drm_format_horz_chroma_subsampling(uint32_t format)
{
	const struct drm_format_info *info = drm_format_info(format);

	return info ? info->hsub : 1;
}

static inline int drm_format_vert_chroma_subsampling(uint32_t format)
{
	const struct drm_format_info *info = drm_format_info(format);

	return info ? info->vsub : 1;
}

/*
 * (4.19: drivers/of/fdt.c)
 * DDR rank / HBB information from the /memory node.  Not available on this
 * tree, so report "unknown" (-ENOENT).
 */

#endif /* __MSM_COMPAT_H__ */

/* ------------------------------------------------------------------ *
 * 5.10 removed the double-underscore atomic helpers that the vendor
 * display code still calls with a pre-fetched state object.
 * ------------------------------------------------------------------ */
#include <drm/drm_atomic.h>
#include <drm/drm_atomic_uapi.h>

static inline int __drm_atomic_helper_disable_plane(struct drm_plane *plane,
		struct drm_plane_state *plane_state)
{
	int ret;

	(void)plane;

	ret = drm_atomic_set_crtc_for_plane(plane_state, NULL);
	if (ret)
		return ret;

	drm_atomic_set_fb_for_plane(plane_state, NULL);
	plane_state->crtc_x = 0;
	plane_state->crtc_y = 0;
	plane_state->crtc_w = 0;
	plane_state->crtc_h = 0;
	plane_state->src_x = 0;
	plane_state->src_y = 0;
	plane_state->src_w = 0;
	plane_state->src_h = 0;

	return 0;
}

static inline int __drm_atomic_helper_set_config(struct drm_mode_set *set,
		struct drm_atomic_state *state)
{
	struct drm_crtc_state *crtc_state;
	int ret;

	crtc_state = drm_atomic_get_crtc_state(state, set->crtc);
	if (IS_ERR(crtc_state))
		return PTR_ERR(crtc_state);

	ret = drm_atomic_set_mode_prop_for_crtc(crtc_state, NULL);
	if (ret < 0)
		return ret;

	crtc_state->active = false;

	return 0;
}

/* 5.10 only ships the HDMI/DP specific colorimetry property helpers. */
static inline int drm_mode_create_colorspace_property(
		struct drm_connector *connector)
{
	return 0;
}

/* Vendor DDR helpers; the real ones live in the downstream DT parser. */
static inline int of_fdt_get_ddrrank(int channel)
{
	return 0;
}

static inline int of_fdt_get_ddrhbb(int channel, int rank)
{
	return 0;
}

/* rpmh_flush() takes a struct rpmh_ctrlr which techpack cannot reach. */
static inline void rpmh_flush(void *ctrlr)
{
}
