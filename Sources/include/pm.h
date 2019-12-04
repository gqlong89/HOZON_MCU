#ifndef __PM_H__
#define __PM_H__

#include <stdbool.h>

void pm_init(void);
void pm_gpio_init(void);
void pm_sys_on(unsigned char is_power_on_4G);
void pm_sys_off(void);
void pm_5v_ctrl(bool enable);
void pm_12v_ctrl(bool enable);
void pm_3v3c_ctrl(bool enable);
void pm_4g_power_ctrl(bool enable);
bool pm_get_4g_status(void);
void pm_4g_safe_on(void);
void pm_4g_safe_off(void);
void pm_4g_reset(void);
void pm_4g_wakeup(void);
void pm_can_stb_ctrl(bool enable);
void pm_bat_k_ctrl(bool enable);
void pm_cpu_charge_ctrl(bool enable);
void pm_cpu_charge_trickle_ctrl(bool enable);   //added by Cindy
void pm_cpu_charge_trickle_varta_ctrl(bool enable);
void pm_vsc_ctrl(bool enable);
void pm_offset_ctrl(bool enable);
void pm_ap_en_ctrl(bool enable);
void pm_DC1_ctrl(bool enable);
void pm_DC2_ctrl(bool enable);
void pm_I_Indicator_ctrl(bool enable);
void pm_B_Indicator_ctrl(bool enable);
void pm_E_Indicator_ctrl(bool enable);
bool pm_get_s_charge_wakeup(void);
bool pm_get_f_charge_wakeup(void);
bool pm_get_acc_wakeup(void);
bool pm_get_on_wakeup(void);
bool pm_get_ring_wakeup(void);
bool pm_get_rtc_wakeup(void);
bool pm_get_gsensor_wakeup(void);
bool pm_get_ICALL(void);
bool pm_get_BCALL(void);
bool pm_get_ECALL(void);
bool pm_get_usb_status(void);
void pm_sleep_ctrl(bool enable);
void pm_off_mode(void);
void pm_acc_reset(void);
void pm_ethernet_ctrl(bool enable);
void pm_vbus_ctrl(bool enable);
void pm_wakeup_ec20_ctrl(bool enable);
bool pm_get_usb_wakeup(void);
void pm_4g_op_clear(void);
bool pm_get_virtual_on_status(void);
void pm_set_virtual_on(int on);


#endif

