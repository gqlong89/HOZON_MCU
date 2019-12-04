/****************************************************************************
 file:         scom_tl_ex.h
 description:  the header file of spi transport layer extend definition
 date:         2017/7/9
 copyright     Wuhan Intest Electronic Technology Co.,Ltd
 author        liuzhongwen
 *****************************************************************************/
#ifndef __SCOM_TL_EX_H__
#define __SCOM_TL_EX_H__

enum MPU_SYN_DATA_STATUS
{
    MPU_SYN_DATA_STATUS_UNKONW = 0,
    MPU_SYN_DATA_STATUS_OK,
    MPU_SYN_DATA_STATUS_ERROR,
};

typedef struct MPU_FAULT_SYN_DATA
{
    unsigned char gps_status;
    unsigned char emmc_status;
    unsigned char gprs_status;
    unsigned char sim_status;
    unsigned char wifi_status;
    unsigned char usb_status;
} MPU_FAULT_SYN_DATA;

typedef struct MPU_STATUS_SYN_DATA
{
    unsigned char ecall_status;
    unsigned char bcall_status;
    unsigned char icall_status;
    unsigned char app_can_sleep;
    unsigned char upg_status;
} MPU_STATUS_SYN_DATA;

void scom_tl_ex_mcu_cfg_syn(void);

void scom_tl_ex_mcu_fault_syn(void);

void scom_tl_ex_mcu_status_syn(void);

void scom_tl_ex_report_ver(void);

void scom_tl_ex_report_btl_ver(void);

void scom_tl_ex_mpu_fault_syn(unsigned char *msg, unsigned int len);

void scom_tl_ex_mpu_status_syn(unsigned char *msg, unsigned int len);

void scom_tl_ex_syn_time(void);

void scom_tl_ex_set_time(unsigned char *msg, unsigned int len);

void scom_tl_ex_report_time(void);

void scom_tl_ex_mcu_shell(unsigned char *msg, unsigned int len);

void scom_tl_ex_mpu_shell_req(unsigned char *msg, unsigned int len);

void scom_tl_ex_mpu_shell_res(unsigned char *msg, unsigned int len);

void scom_tl_ex_wake_time(unsigned char *msg, unsigned int len);

unsigned char scom_get_gps_status(void);

unsigned char scom_get_emmc_status(void);

unsigned char scom_get_gprs_status(void);

unsigned char scom_get_sim_status(void);

unsigned char scom_get_wifi_status(void);

unsigned char scom_get_usb_status(void);

unsigned char scom_get_ecall_status(void);

unsigned char scom_get_bcall_status(void);

unsigned char scom_get_icall_status(void);

unsigned char scom_is_app_can_sleep(void);

unsigned char scom_get_upg_status(void);

unsigned int scom_get_upg_time(void);

unsigned int scom_clear_upg_time(void);


int scom_get_wake_time(void);

void set_sleep_time_in_running(void);


#endif

