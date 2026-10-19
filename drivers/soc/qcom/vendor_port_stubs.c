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
void arch_set_max_thermal_scale(void) __attribute__((weak));
void arch_set_max_thermal_scale(void) {}
EXPORT_SYMBOL(arch_set_max_thermal_scale);
void compat_fastrpc_device_ioctl(void) __attribute__((weak));
void compat_fastrpc_device_ioctl(void) {}
EXPORT_SYMBOL(compat_fastrpc_device_ioctl);
void ion_populate_vm_list(void) __attribute__((weak));
void ion_populate_vm_list(void) {}
EXPORT_SYMBOL(ion_populate_vm_list);
void slim_add_device(void) __attribute__((weak));
void slim_add_device(void) {}
EXPORT_SYMBOL(slim_add_device);
void slim_change_val_element(void) __attribute__((weak));
void slim_change_val_element(void) {}
EXPORT_SYMBOL(slim_change_val_element);
void slim_connect_sink(void) __attribute__((weak));
void slim_connect_sink(void) {}
EXPORT_SYMBOL(slim_connect_sink);
void slim_connect_src(void) __attribute__((weak));
void slim_connect_src(void) {}
EXPORT_SYMBOL(slim_connect_src);
void slim_control_ch(void) __attribute__((weak));
void slim_control_ch(void) {}
EXPORT_SYMBOL(slim_control_ch);
void slim_define_ch(void) __attribute__((weak));
void slim_define_ch(void) {}
EXPORT_SYMBOL(slim_define_ch);
void slim_disconnect_ports(void) __attribute__((weak));
void slim_disconnect_ports(void) {}
EXPORT_SYMBOL(slim_disconnect_ports);
void slim_driver_register(void) __attribute__((weak));
void slim_driver_register(void) {}
EXPORT_SYMBOL(slim_driver_register);
void slim_get_slaveport(void) __attribute__((weak));
void slim_get_slaveport(void) {}
EXPORT_SYMBOL(slim_get_slaveport);
void slim_query_ch(void) __attribute__((weak));
void slim_query_ch(void) {}
EXPORT_SYMBOL(slim_query_ch);
void slim_remove_device(void) __attribute__((weak));
void slim_remove_device(void) {}
EXPORT_SYMBOL(slim_remove_device);
void slim_request_val_element(void) __attribute__((weak));
void slim_request_val_element(void) {}
EXPORT_SYMBOL(slim_request_val_element);
void snd_compr_stop_error(void) __attribute__((weak));
void snd_compr_stop_error(void) {}
EXPORT_SYMBOL(snd_compr_stop_error);
void usb_gsi_ep_op(void) __attribute__((weak));
void usb_gsi_ep_op(void) {}
EXPORT_SYMBOL(usb_gsi_ep_op);

/* ---- data stubs (64-byte placeholder objects) ---- */

