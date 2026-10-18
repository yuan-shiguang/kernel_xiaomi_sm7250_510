/* SPDX-License-Identifier: GPL-2.0 */
/*
 * [VENDOR-PORT Phase7] 4.19 compatibility shim.
 *
 * Linux 5.10 moved the DMA-API debug helpers to kernel/dma/debug.h and
 * deleted this header. The vendor tree still ships the 4.19 copy, whose
 * debug_dma_*() prototypes carry the extra map_single/attrs argument that
 * 5.10 dropped; keeping them would clash with kernel/dma/debug.h in every
 * translation unit that sees both.
 *
 * No caller outside kernel/dma/ uses debug_dma_*() in this tree, so only the
 * few symbols that 5.10 does NOT provide elsewhere are kept here.
 */
#ifndef __DMA_DEBUG_H
#define __DMA_DEBUG_H

#include <linux/types.h>

struct device;
struct bus_type;

#ifdef CONFIG_DMA_API_DEBUG

extern void dma_debug_add_bus(struct bus_type *bus);
extern int dma_debug_resize_entries(u32 num_entries);
extern void debug_dma_dump_mappings(struct device *dev);

#else /* CONFIG_DMA_API_DEBUG */

static inline void dma_debug_add_bus(struct bus_type *bus)
{
}

static inline int dma_debug_resize_entries(u32 num_entries)
{
	return 0;
}

static inline void debug_dma_dump_mappings(struct device *dev)
{
}

#endif /* CONFIG_DMA_API_DEBUG */

#endif /* __DMA_DEBUG_H */
