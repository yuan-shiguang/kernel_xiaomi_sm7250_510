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
void complete_err_ready(void) __attribute__((weak));
void complete_err_ready(void) {}
EXPORT_SYMBOL(complete_err_ready);
void complete_shutdown_ack(void) __attribute__((weak));
void complete_shutdown_ack(void) {}
EXPORT_SYMBOL(complete_shutdown_ack);
void cpufreq_platform_cooling_register(void) __attribute__((weak));
void cpufreq_platform_cooling_register(void) {}
EXPORT_SYMBOL(cpufreq_platform_cooling_register);
void devm_thermal_of_virtual_sensor_register(void) __attribute__((weak));
void devm_thermal_of_virtual_sensor_register(void) {}
EXPORT_SYMBOL(devm_thermal_of_virtual_sensor_register);
void find_subsys_device(void) __attribute__((weak));
void find_subsys_device(void) {}
EXPORT_SYMBOL(find_subsys_device);
void get_hw_version_platform(void) __attribute__((weak));
void get_hw_version_platform(void) {}
EXPORT_SYMBOL(get_hw_version_platform);
void ion_populate_vm_list(void) __attribute__((weak));
void ion_populate_vm_list(void) {}
EXPORT_SYMBOL(ion_populate_vm_list);
void mi_drm_notifier_call_chain(void) __attribute__((weak));
void mi_drm_notifier_call_chain(void) {}
EXPORT_SYMBOL(mi_drm_notifier_call_chain);
void mipi_dsi_create_packet(void) __attribute__((weak));
void mipi_dsi_create_packet(void) {}
EXPORT_SYMBOL(mipi_dsi_create_packet);
void mipi_dsi_dcs_set_display_brightness(void) __attribute__((weak));
void mipi_dsi_dcs_set_display_brightness(void) {}
EXPORT_SYMBOL(mipi_dsi_dcs_set_display_brightness);
void mipi_dsi_dcs_write(void) __attribute__((weak));
void mipi_dsi_dcs_write(void) {}
EXPORT_SYMBOL(mipi_dsi_dcs_write);
void mipi_dsi_host_register(void) __attribute__((weak));
void mipi_dsi_host_register(void) {}
EXPORT_SYMBOL(mipi_dsi_host_register);
void mipi_dsi_host_unregister(void) __attribute__((weak));
void mipi_dsi_host_unregister(void) {}
EXPORT_SYMBOL(mipi_dsi_host_unregister);
void msm_bus_cl_get_pdata(void) __attribute__((weak));
void msm_bus_cl_get_pdata(void) {}
EXPORT_SYMBOL(msm_bus_cl_get_pdata);
void msm_bus_pdata_from_node(void) __attribute__((weak));
void msm_bus_pdata_from_node(void) {}
EXPORT_SYMBOL(msm_bus_pdata_from_node);
void msm_bus_scale_client_update_context(void) __attribute__((weak));
void msm_bus_scale_client_update_context(void) {}
EXPORT_SYMBOL(msm_bus_scale_client_update_context);
void msm_bus_scale_client_update_request(void) __attribute__((weak));
void msm_bus_scale_client_update_request(void) {}
EXPORT_SYMBOL(msm_bus_scale_client_update_request);
void msm_bus_scale_register(void) __attribute__((weak));
void msm_bus_scale_register(void) {}
EXPORT_SYMBOL(msm_bus_scale_register);
void msm_bus_scale_register_client(void) __attribute__((weak));
void msm_bus_scale_register_client(void) {}
EXPORT_SYMBOL(msm_bus_scale_register_client);
void msm_bus_scale_unregister(void) __attribute__((weak));
void msm_bus_scale_unregister(void) {}
EXPORT_SYMBOL(msm_bus_scale_unregister);
void msm_bus_scale_unregister_client(void) __attribute__((weak));
void msm_bus_scale_unregister_client(void) {}
EXPORT_SYMBOL(msm_bus_scale_unregister_client);
void msm_bus_scale_update_bw(void) __attribute__((weak));
void msm_bus_scale_update_bw(void) {}
EXPORT_SYMBOL(msm_bus_scale_update_bw);
void msm_cpuidle_get_deep_idle_latency(void) __attribute__((weak));
void msm_cpuidle_get_deep_idle_latency(void) {}
EXPORT_SYMBOL(msm_cpuidle_get_deep_idle_latency);
void msm_gpio_mpm_wake_set(void) __attribute__((weak));
void msm_gpio_mpm_wake_set(void) {}
EXPORT_SYMBOL(msm_gpio_mpm_wake_set);
void msm_hdcp_cache_repeater_topology(void) __attribute__((weak));
void msm_hdcp_cache_repeater_topology(void) {}
EXPORT_SYMBOL(msm_hdcp_cache_repeater_topology);
void msm_hdcp_notify_topology(void) __attribute__((weak));
void msm_hdcp_notify_topology(void) {}
EXPORT_SYMBOL(msm_hdcp_notify_topology);
void msm_hdcp_register_cb(void) __attribute__((weak));
void msm_hdcp_register_cb(void) {}
EXPORT_SYMBOL(msm_hdcp_register_cb);
void notify_before_auth_and_reset(void) __attribute__((weak));
void notify_before_auth_and_reset(void) {}
EXPORT_SYMBOL(notify_before_auth_and_reset);
void notify_proxy_unvote(void) __attribute__((weak));
void notify_proxy_unvote(void) {}
EXPORT_SYMBOL(notify_proxy_unvote);
void notify_proxy_vote(void) __attribute__((weak));
void notify_proxy_vote(void) {}
EXPORT_SYMBOL(notify_proxy_vote);
void of_thermal_handle_trip(void) __attribute__((weak));
void of_thermal_handle_trip(void) {}
EXPORT_SYMBOL(of_thermal_handle_trip);
void of_thermal_handle_trip_temp(void) __attribute__((weak));
void of_thermal_handle_trip_temp(void) {}
EXPORT_SYMBOL(of_thermal_handle_trip_temp);
void register_system_pm_ops(void) __attribute__((weak));
void register_system_pm_ops(void) {}
EXPORT_SYMBOL(register_system_pm_ops);
void rmnet_all_flows_enabled(void) __attribute__((weak));
void rmnet_all_flows_enabled(void) {}
EXPORT_SYMBOL(rmnet_all_flows_enabled);
void rmnet_clear_powersave_format(void) __attribute__((weak));
void rmnet_clear_powersave_format(void) {}
EXPORT_SYMBOL(rmnet_clear_powersave_format);
void rmnet_enable_all_flows(void) __attribute__((weak));
void rmnet_enable_all_flows(void) {}
EXPORT_SYMBOL(rmnet_enable_all_flows);
void rmnet_get_dlmarker_info(void) __attribute__((weak));
void rmnet_get_dlmarker_info(void) {}
EXPORT_SYMBOL(rmnet_get_dlmarker_info);
void rmnet_get_packets(void) __attribute__((weak));
void rmnet_get_packets(void) {}
EXPORT_SYMBOL(rmnet_get_packets);
void rmnet_get_powersave_notif(void) __attribute__((weak));
void rmnet_get_powersave_notif(void) {}
EXPORT_SYMBOL(rmnet_get_powersave_notif);
void rmnet_prepare_ps_bearers(void) __attribute__((weak));
void rmnet_prepare_ps_bearers(void) {}
EXPORT_SYMBOL(rmnet_prepare_ps_bearers);
void rmnet_set_powersave_format(void) __attribute__((weak));
void rmnet_set_powersave_format(void) {}
EXPORT_SYMBOL(rmnet_set_powersave_format);
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
void subsys_get_crash_status(void) __attribute__((weak));
void subsys_get_crash_status(void) {}
EXPORT_SYMBOL(subsys_get_crash_status);
void subsys_register(void) __attribute__((weak));
void subsys_register(void) {}
EXPORT_SYMBOL(subsys_register);
void subsys_set_crash_status(void) __attribute__((weak));
void subsys_set_crash_status(void) {}
EXPORT_SYMBOL(subsys_set_crash_status);
void subsys_unregister(void) __attribute__((weak));
void subsys_unregister(void) {}
EXPORT_SYMBOL(subsys_unregister);
void subsystem_crashed(void) __attribute__((weak));
void subsystem_crashed(void) {}
EXPORT_SYMBOL(subsystem_crashed);
void subsystem_get(void) __attribute__((weak));
void subsystem_get(void) {}
EXPORT_SYMBOL(subsystem_get);
void subsystem_get_with_fwname(void) __attribute__((weak));
void subsystem_get_with_fwname(void) {}
EXPORT_SYMBOL(subsystem_get_with_fwname);
void subsystem_put(void) __attribute__((weak));
void subsystem_put(void) {}
EXPORT_SYMBOL(subsystem_put);
void subsystem_restart_dev(void) __attribute__((weak));
void subsystem_restart_dev(void) {}
EXPORT_SYMBOL(subsystem_restart_dev);
void usb_gsi_ep_op(void) __attribute__((weak));
void usb_gsi_ep_op(void) {}
EXPORT_SYMBOL(usb_gsi_ep_op);
void wait_for_shutdown_ack(void) __attribute__((weak));
void wait_for_shutdown_ack(void) {}
EXPORT_SYMBOL(wait_for_shutdown_ack);

/* ---- data stubs (64-byte placeholder objects) ---- */
char kp_active_mode[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(kp_active_mode);
char rq_info[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(rq_info);
char rq_lock[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(rq_lock);
char rq_wq[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(rq_wq);
char wlfw_athdiag_read_req_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_athdiag_read_req_msg_v01_ei);
char wlfw_athdiag_read_resp_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_athdiag_read_resp_msg_v01_ei);
char wlfw_athdiag_write_req_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_athdiag_write_req_msg_v01_ei);
char wlfw_athdiag_write_resp_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_athdiag_write_resp_msg_v01_ei);
char wlfw_cap_req_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_cap_req_msg_v01_ei);
char wlfw_cap_resp_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_cap_resp_msg_v01_ei);
char wlfw_dynamic_feature_mask_req_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_dynamic_feature_mask_req_msg_v01_ei);
char wlfw_dynamic_feature_mask_resp_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_dynamic_feature_mask_resp_msg_v01_ei);
char wlfw_fw_ready_ind_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_fw_ready_ind_msg_v01_ei);
char wlfw_ind_register_req_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_ind_register_req_msg_v01_ei);
char wlfw_ind_register_resp_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_ind_register_resp_msg_v01_ei);
char wlfw_ini_req_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_ini_req_msg_v01_ei);
char wlfw_ini_resp_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_ini_resp_msg_v01_ei);
char wlfw_msa_info_req_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_msa_info_req_msg_v01_ei);
char wlfw_msa_info_resp_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_msa_info_resp_msg_v01_ei);
char wlfw_msa_ready_ind_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_msa_ready_ind_msg_v01_ei);
char wlfw_msa_ready_req_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_msa_ready_req_msg_v01_ei);
char wlfw_msa_ready_resp_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_msa_ready_resp_msg_v01_ei);
char wlfw_pin_connect_result_ind_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_pin_connect_result_ind_msg_v01_ei);
char wlfw_rejuvenate_ack_req_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_rejuvenate_ack_req_msg_v01_ei);
char wlfw_rejuvenate_ack_resp_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_rejuvenate_ack_resp_msg_v01_ei);
char wlfw_rejuvenate_ind_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_rejuvenate_ind_msg_v01_ei);
char wlfw_shutdown_req_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_shutdown_req_msg_v01_ei);
char wlfw_shutdown_resp_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_shutdown_resp_msg_v01_ei);
char wlfw_vbatt_req_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_vbatt_req_msg_v01_ei);
char wlfw_vbatt_resp_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_vbatt_resp_msg_v01_ei);
char wlfw_wlan_cfg_req_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_wlan_cfg_req_msg_v01_ei);
char wlfw_wlan_cfg_resp_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_wlan_cfg_resp_msg_v01_ei);
char wlfw_wlan_mode_req_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_wlan_mode_req_msg_v01_ei);
char wlfw_wlan_mode_resp_msg_v01_ei[64] __attribute__((weak, aligned(8)));
EXPORT_SYMBOL(wlfw_wlan_mode_resp_msg_v01_ei);

