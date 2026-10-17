/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * 5.10 removed arch/arm64/include/asm/sizes.h; the generic definitions
 * live in <linux/sizes.h>. Keep the old include path working for the
 * vendor (techpack) code that still uses it.
 */
#ifndef __ASM_SIZES_H
#define __ASM_SIZES_H

#include <linux/sizes.h>

#endif /* __ASM_SIZES_H */
