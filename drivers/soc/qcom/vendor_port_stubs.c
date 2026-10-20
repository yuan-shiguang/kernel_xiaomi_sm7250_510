// SPDX-License-Identifier: GPL-2.0
/*
 * [VENDOR-PORT Phase7] Fallback stub providers for vendor symbols that the
 * 5.10.270 GKI tree no longer supplies (provider directories never built,
 * or the implementation was dropped during GKI-ization). These weak stubs
 * let vmlinux link so the kernel is bootable. Real vendor drivers, when
 * their code/directories are restored, override these weak symbols.
 *
 * NOTE: this is a pragmatic stopgap for 'compile + basically functional'.
 * The proper fix is to wire/restore the real vendor providers.
 */
#include <linux/types.h>
#include <linux/export.h>

/* ---- function stubs (no-op, safe default return) ---- */

/* ---- data stubs (64-byte placeholder objects) ---- */

