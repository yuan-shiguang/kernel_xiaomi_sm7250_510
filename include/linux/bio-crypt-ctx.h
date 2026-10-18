/* SPDX-License-Identifier: GPL-2.0 */
/*
 * [VENDOR-PORT Phase7] Compatibility shim -- do not add code here.
 *
 * The 5.10 GKI tree defines the inline-encryption types in
 * <linux/blk-crypto.h>: struct bio_crypt_ctx, struct blk_crypto_key,
 * enum blk_crypto_mode_num, BLK_CRYPTO_DUN_ARRAY_SIZE, ...
 *
 * The 4.19 / 5.10.100-vendor tree carried a second, incompatible copy of
 * those definitions in this header (plus a keyslot in bio_crypt_ctx and a
 * flat blk_crypto_key layout).  Keeping both produced a storm of
 * "redefinition of ..." errors, so this header now only forwards to GKI.
 *
 * Vendor consumers must use the GKI field names:
 *   key->crypto_mode    -> key->crypto_cfg.crypto_mode
 *   key->data_unit_size -> key->crypto_cfg.data_unit_size
 *   key->is_hw_wrapped  -> key->crypto_cfg.is_hw_wrapped
 * and the GKI keyslot-manager API (struct blk_keyslot_manager /
 * struct blk_ksm_ll_ops) instead of struct keyslot_manager /
 * struct keyslot_mgmt_ll_ops.
 */
#ifndef __LINUX_BIO_CRYPT_CTX_H
#define __LINUX_BIO_CRYPT_CTX_H

#include <linux/blk-crypto.h>

#endif /* __LINUX_BIO_CRYPT_CTX_H */
