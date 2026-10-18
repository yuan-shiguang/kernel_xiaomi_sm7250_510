// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012 ARM Ltd.
 * Author: Catalin Marinas <catalin.marinas@arm.com>
 */

#include <linux/gfp.h>
#include <linux/cache.h>
#include <linux/dma-map-ops.h>
#include <linux/dma-iommu.h>
#include <xen/xen.h>
#include <xen/swiotlb-xen.h>
#include <trace/hooks/iommu.h>

#include <asm/cacheflush.h>

void arch_sync_dma_for_device(phys_addr_t paddr, size_t size,
		enum dma_data_direction dir)
{
	__dma_map_area(phys_to_virt(paddr), size, dir);
}

void arch_sync_dma_for_cpu(phys_addr_t paddr, size_t size,
		enum dma_data_direction dir)
{
	__dma_unmap_area(phys_to_virt(paddr), size, dir);
}

void arch_dma_prep_coherent(struct page *page, size_t size)
{
	__dma_flush_area(page_address(page), size);
}

#ifdef CONFIG_IOMMU_DMA
void arch_teardown_dma_ops(struct device *dev)
{
	dev->dma_ops = NULL;
}
#endif

void arch_setup_dma_ops(struct device *dev, u64 dma_base, u64 size,
			const struct iommu_ops *iommu, bool coherent)
{
	int cls = cache_line_size_of_cpu();

	WARN_TAINT(!coherent && cls > ARCH_DMA_MINALIGN,
		   TAINT_CPU_OUT_OF_SPEC,
		   "%s %s: ARCH_DMA_MINALIGN smaller than CTR_EL0.CWG (%d < %d)",
		   dev_driver_string(dev), dev_name(dev),
		   ARCH_DMA_MINALIGN, cls);

	dev->dma_coherent = coherent;
	if (iommu) {
		iommu_setup_dma_ops(dev, dma_base, size);
		trace_android_vh_iommu_setup_dma_ops(dev, dma_base, size);
		trace_android_rvh_iommu_setup_dma_ops(dev, dma_base, size);
	}

#ifdef CONFIG_XEN
	if (xen_initial_domain())
		dev->dma_ops = &xen_swiotlb_dma_ops;
#endif
}


/* [VENDOR-PORT Phase7] dummy_dma_ops from 5.10.100 REF */
static void *__dummy_alloc(struct device *dev, size_t size,
			   dma_addr_t *dma_handle, gfp_t flags,
			   unsigned long attrs)
{
	WARN(1, "dma alloc failure, device may be missing a call to arch_setup_dma_ops");
	return NULL;
}

static void __dummy_free(struct device *dev, size_t size,
			 void *vaddr, dma_addr_t dma_handle,
			 unsigned long attrs)
{
}

static int __dummy_mmap(struct device *dev,
			struct vm_area_struct *vma,
			void *cpu_addr, dma_addr_t dma_addr, size_t size,
			unsigned long attrs)
{
	return -ENXIO;
}

static dma_addr_t __dummy_map_page(struct device *dev, struct page *page,
				   unsigned long offset, size_t size,
				   enum dma_data_direction dir,
				   unsigned long attrs)
{
	return 0;
}

static void __dummy_unmap_page(struct device *dev, dma_addr_t dev_addr,
			       size_t size, enum dma_data_direction dir,
			       unsigned long attrs)
{
}

static int __dummy_map_sg(struct device *dev, struct scatterlist *sgl,
			  int nelems, enum dma_data_direction dir,
			  unsigned long attrs)
{
	return 0;
}

static void __dummy_unmap_sg(struct device *dev,
			     struct scatterlist *sgl, int nelems,
			     enum dma_data_direction dir,
			     unsigned long attrs)
{
}

static void __dummy_sync_single(struct device *dev,
				dma_addr_t dev_addr, size_t size,
				enum dma_data_direction dir)
{
}

static void __dummy_sync_sg(struct device *dev,
			    struct scatterlist *sgl, int nelems,
			    enum dma_data_direction dir)
{
}

static int __dummy_dma_supported(struct device *hwdev, u64 mask)
{
	return 0;
}

const struct dma_map_ops dummy_dma_ops = {
	.alloc                  = __dummy_alloc,
	.free                   = __dummy_free,
	.mmap                   = __dummy_mmap,
	.map_page               = __dummy_map_page,
	.unmap_page             = __dummy_unmap_page,
	.map_sg                 = __dummy_map_sg,
	.unmap_sg               = __dummy_unmap_sg,
	.sync_single_for_cpu    = __dummy_sync_single,
	.sync_single_for_device = __dummy_sync_single,
	.sync_sg_for_cpu        = __dummy_sync_sg,
	.sync_sg_for_device     = __dummy_sync_sg,
	.dma_supported          = __dummy_dma_supported,
};
EXPORT_SYMBOL(dummy_dma_ops);

