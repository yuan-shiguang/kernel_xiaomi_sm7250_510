/* SPDX-License-Identifier: GPL-2.0 */
/*
 * [VENDOR-PORT Phase7] 5.10 merged the CMA / DMA-coherency declarations into
 * <linux/dma-map-ops.h> and removed this header.  The vendor tree still
 * includes it from a number of drivers, so keep it as a forwarding shim.
 *
 * The two vendor-only helpers that never made it upstream
 * (dma_declare_contiguous(), dma_contiguous_set_default()) had no users
 * anywhere in this tree and are intentionally dropped.
 */
#ifndef __LINUX_CMA_H
#define __LINUX_CMA_H

#include <linux/dma-map-ops.h>

#endif /* __LINUX_CMA_H */
