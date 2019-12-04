#include <acc.h>
#include <adc.h>
#include <app/battery/battery.h>
#include <app/can_mgr/can_mgr.h>
#include <app/power/pm_mode.h>
#include <app/power/power.h>
#include <app/scom/scom_tl.h>
#include <app/scom/scom_tl_ex.h>
#include <brdInfo.h>
#include <config.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <osif.h>
#include <pm.h>
#include <rtc.h>
#include <scom_msg_def.h>
#include <shell.h>
#include <softrtc.h>
#include <srs.h>
#include <string.h>
#include <sys/_stdint.h>


MPU_FAULT_SYN_DATA  mpu_fault_syn_data;
MPU_STATUS_SYN_DATA mpu_status_syn_data;

static unsigned int wake_time   = 0;
static uint64_t upg_change_time = 0;


void scom_tl_ex_mcu_cfg_syn(void)
{
    ConfigSyncStr sync;

    memcpy(&sync.mcu_conf, cfg_get_mcu_conf(), sizeof(sync.mcu_conf));
    sync.sn = cfg_get_sn();
    scom_tl_send_msg(SCOM_TL_CMD_MCU_CFG_SYN, (unsigned char *)&sync, sizeof(ConfigSyncStr));
}

void scom_tl_ex_mcu_fault_syn(void)
{
    unsigned char data[16];
    unsigned int size;
    unsigned int powervVoltage;

    size = 0;

    /*GPS ANT STATUS*/
    switch (kernelGetGpsAntStatus())
    {
        case GPSANT_SHORT:
            data[size++] = 0x02;
            break;

        case GPSANT_OPEN:
            data[size++] = 0x03;
            break;

        case GPSANT_OK:
            data[size++] = 0x01;
            break;

        default: /*unknow*/
            data[size++] = 0x00;
            break;
    }

    /*CAN NODE LOSE*/
    for (unsigned char i = 0; i < CAN_MAX_PORT; i++)
    {
        data[size++] = can_state_lost(i);
    }

    /*CAN BUS ERROR*/
    for (unsigned char i = 0; i < CAN_MAX_PORT; i++)
    {
        data[size++] = can_state_busoff(i);
    }

    /*Power Voltage*/
    powervVoltage = (unsigned int)(100
                                   * ((3.3 * (49900 + 4700) * adc_Abat() / 4096 / 4700) + VOLTAGE_COMPENSATION));

    if (powervVoltage < 950)     //low voltage
    {
        data[size++] = 0x01;
    }
    else if (powervVoltage > 1550)     //high voltage
    {
        data[size++] = 0x02;
    }
    else
    {
        data[size++] = 0x00;
    }

    /*Battery*/
    if (BatteryInsertDetect())
    {
        data[size++] = 0x03;
    }
    else
    {
        if (BatteryGetResistance() == 0)
        {
            data[size++] = 0x00;
        }
        else
        {
            if (BatteryGetStatus())
            {
                data[size++] = 0x02;
            }
            else
            {
                data[size++] = 0x01;
            }
        }
    }

    /*MIC Voltage*/
    if (kernelGetMICVStatus() == MICV_OK)
    {
        data[size++] = 0x00;
    }
    else
    {
        data[size++] = 0x01;
    }

    /*福田故障检测 与MPU同步*/
    //    #if FUTIAN_DIAG_ENABLE

    /*MIC status*/
    data[size++] = (unsigned char)kernelGetMICStatus();

    /*SOS BTN status 按下：0 弹起：1*/
//    if (kernelGetSOSStatus() == SOS_DOWN)
//    {
//        data[size++] = 0x00;
//    }
//    else
//    {
        data[size++] = kernelGetSOSStatus();
//    }

    /*
       SPK+  SPK- status
       SPK_UNKNOW = 0,
       SPK_SHORT,
       SPK_OPEN,
       SPK_OK
    */
    if (kernelGetSPKStatus() == SPK_UNKNOW)
    {
        data[size++] = 0x00;
    }
    else if (kernelGetSPKStatus() == SPK_SHORT)
    {
        data[size++] = 0x01;
    }
    else if (kernelGetSPKStatus() == SPK_OPEN)
    {
        data[size++] = 0x02;
    }
    else if (kernelGetSPKStatus() == SPK_OK)
    {
        data[size++] = 0x03;
    }
    else
    {
        data[size++] = kernelGetSPKStatus();
    }

    //    #endif

    data[size++] = accstatus;                       //Gsense Cindy

    powervVoltage = (3.3 * adc_Bbat() * 2 / 4096) * 100;

    if (powervVoltage == 0)
    {
        data[size++] =
            BAT_POWER_NONE;              //bat == 0, no battery; [Shenlei provide: battery < 3 the board cannot run]
    }
    else if (powervVoltage < 270)
    {
        data[size++] = BAT_POWER_LOW;
    }
    else if (powervVoltage > 500)
    {
        data[size++] = BAT_POWER_HIGH;
    }
    else
    {
        data[size++] = BAT_POWER_OK;
    }

    scom_tl_send_frame(SCOM_TL_CMD_MCU_FAULT_SYN, SCOM_TL_SINGLE_FRAME, 0, data, size);
}

void scom_tl_ex_mcu_status_syn(void)
{
    static unsigned char data[60];
    unsigned int size;
    unsigned int powervVoltage;
    unsigned int WakeUpSource;
    unsigned int reboot_cnt;
    unsigned int bat_vol;
    unsigned int bat_rst;
    int bat_temp, i;

    size = 0;

    /*Power Voltage*/
    powervVoltage = (unsigned int)(1000 * ((3.3 * (49900 + 4700) * adc_Abat() / 4096 / 4700) +
                                           VOLTAGE_COMPENSATION));

    data[size++] = powervVoltage;
    data[size++] = powervVoltage >> 8;

    data[size++] = pm_get_s_charge_wakeup();

    data[size++] = pm_get_on_wakeup();

    data[size++] = pm_get_acc_wakeup();

    data[size++] = pm_get_f_charge_wakeup();

    WakeUpSource = driverGetWakeUpSrc();

    data[size++] = WakeUpSource;
    data[size++] = WakeUpSource >> 8;

    data[size++] = pm_get_ICALL();

    data[size++] = pm_get_BCALL();

    data[size++] = pm_get_ECALL();

    data[size++] = pm_get_run_mode();

    data[size++] = kernelGetSRSStatus();

    reboot_cnt   = cfg_get_rbt();
    data[size++] = reboot_cnt;
    data[size++] = reboot_cnt >> 8;
    data[size++] = reboot_cnt >> 16;
    data[size++] = reboot_cnt >> 24;

    /* battery voltage */
    bat_vol = BatteryVoltage() * 1000;
    data[size++] = bat_vol;
    data[size++] = bat_vol >> 8;

    /* battery temprature */
    bat_temp = BatteryGetTemprature();
    data[size++] = bat_temp;
    data[size++] = bat_temp >> 8;
    data[size++] = bat_temp >> 16;
    data[size++] = bat_temp >> 24;

    /* battery Resistance */
    bat_rst = BatteryGetResistance();
    data[size++] = bat_rst;
    data[size++] = bat_rst >> 8;
    data[size++] = bat_rst >> 16;
    data[size++] = bat_rst >> 24;

    /* battery health status */
    if (BatteryGetStatus())
    {
        data[size++] = 0x02;
    }
    else
    {
        data[size++] = 0x01;
    }

    /* can bus status */
    for (i = 0; i < 4; i++)
    {
        /* can bus error */
        if (MPU_SYN_DATA_STATUS_ERROR == can_state_buserr(i))
        {
            data[size++] = 0x03;
        }
        else
        {
            /* can bus busy */
            if (RET_HAVE_DATA == can_sta_get(i))
            {
                data[size++] = 0x0;
            }
            else /* can bus idle */
            {
                data[size++] = 0x02;
            }
        }
    }

    scom_tl_send_frame(SCOM_TL_CMD_MCU_STATUS_SYN, SCOM_TL_SINGLE_FRAME, 0, data, size);
}

void scom_tl_ex_report_time(void)
{
    RTCTime time;

    driverRTCGetTime(&time);

    scom_tl_send_frame(SCOM_TL_CMD_REPORT_TIME, SCOM_TL_SINGLE_FRAME, 0, (unsigned char *)&time,
                       sizeof(RTCTime));
}

void scom_tl_ex_report_ver(void)
{
    scom_tl_send_frame(SCOM_TL_CMD_UPG_MCU_VER, SCOM_TL_SINGLE_FRAME, 0,
                       (unsigned char *)kernelGetVersion(),
                       strlen(kernelGetVersion()));
}

void scom_tl_ex_report_btl_ver(void)
{
    scom_tl_send_frame(SCOM_TL_CMD_MCU_BTL_VER, SCOM_TL_SINGLE_FRAME, 0,
                       (unsigned char *)brd_get_btlVerStr(),
                       strlen(brd_get_btlVerStr()));
}

void scom_tl_ex_mpu_fault_syn(unsigned char *msg, unsigned int len)
{
    UNUSED(len);

    if (len - sizeof(SCOM_TL_MSG_HDR) <= sizeof(MPU_FAULT_SYN_DATA))
    {
        memcpy(&mpu_fault_syn_data, &msg[sizeof(SCOM_TL_MSG_HDR)], len - sizeof(SCOM_TL_MSG_HDR));
    }
    else
    {
        memcpy(&mpu_fault_syn_data, &msg[sizeof(SCOM_TL_MSG_HDR)], sizeof(MPU_FAULT_SYN_DATA));
    }
}

void scom_tl_ex_mpu_status_syn(unsigned char *msg, unsigned int len)
{
    UNUSED(len);

    MPU_STATUS_SYN_DATA *new = (MPU_STATUS_SYN_DATA *)(msg + sizeof(SCOM_TL_MSG_HDR));

    if (mpu_status_syn_data.upg_status != new->upg_status)
    {
        upg_change_time = driverGetUptime();
    }

    if (len - sizeof(SCOM_TL_MSG_HDR) <= sizeof(MPU_STATUS_SYN_DATA))
    {
        memcpy(&mpu_status_syn_data, &msg[sizeof(SCOM_TL_MSG_HDR)], len - sizeof(SCOM_TL_MSG_HDR));
    }
    else
    {
        memcpy(&mpu_status_syn_data, &msg[sizeof(SCOM_TL_MSG_HDR)], sizeof(MPU_STATUS_SYN_DATA));
    }
}

void scom_tl_ex_syn_time(void)
{
    RTCTime time;

    driverRTCGetTime(&time);

    taskPrintf(TASK_LEVEL_LOG, "scom get time: %02d/%02d/%02d %02d:%02d:%02d\r\n",
               time.RTC_Year, time.RTC_Mon, time.RTC_Mday,
               time.RTC_Hour, time.RTC_Min, time.RTC_Sec);

    scom_tl_send_frame(SCOM_TL_CMD_SET_TIME, SCOM_TL_SINGLE_FRAME, 0, (unsigned char *)&time,
                       sizeof(time));
}

void scom_tl_ex_set_time(unsigned char *msg, unsigned int len)
{
    RTCTime *time;

    if (len < sizeof(SCOM_TL_MSG_HDR) + sizeof(RTCTime))
    {
        taskPrintf(TASK_LEVEL_LOG, "msg len=%u is not enough, need be %d", len, sizeof(RTCTime));
        return;
    }

    time = (RTCTime *) &msg[sizeof(SCOM_TL_MSG_HDR)];

    driverRTCSetTime(*time);

    initUTCTime();

    taskPrintf(TASK_LEVEL_LOG, "scom set time: %02d/%02d/%02d %02d:%02d:%02d\r\n",
               time->RTC_Year, time->RTC_Mon, time->RTC_Mday,
               time->RTC_Hour, time->RTC_Min, time->RTC_Sec);
}

void scom_tl_ex_mcu_shell(unsigned char *msg, unsigned int len)
{
    static unsigned char tmpShellBuf[384];
    unsigned int respLen = 0;

    DUMPHEX(TASK_LEVEL_DEBUG, msg, len);

    memset(tmpShellBuf, 0, sizeof(tmpShellBuf));

    setshellRedirectBuf((char *)tmpShellBuf);
    setshellRedirectBufSize(sizeof(tmpShellBuf));

    shell_cmd_proc(&msg[sizeof(SCOM_TL_MSG_HDR)], (len - sizeof(SCOM_TL_MSG_HDR)));

    respLen = sizeof(tmpShellBuf) - getshellRedirectBufSize();

    scom_tl_send_frame(SCOM_TL_CMD_MCU_SHELL, SCOM_TL_SINGLE_FRAME, 0, tmpShellBuf, respLen);

    stopshellRedirect();

}

void scom_tl_ex_mpu_shell_res(unsigned char *msg, unsigned int len)
{
    DUMPHEX(TASK_LEVEL_DEBUG, msg, len);
}

void scom_tl_ex_wake_time(unsigned char *msg, unsigned int len)
{
    //Modified by Cindy 20180329
    UNUSED(len);
    int ret;
    unsigned int *temp_time;
    unsigned int com_time;

    temp_time = (unsigned int *) &msg[sizeof(SCOM_TL_MSG_HDR)];

    wake_time = 0;
    ret = softrtc_frombasetime((*temp_time) * 60, &wake_time);
    softrtc_frombasetime(PM_MIN_INPUT_WAKEUP, &com_time);

    //  printf("wake_time = %d\r\n", wake_time);

    if (wake_time <= com_time)
    {
        kernelSetSleepUTCTime(wake_time + PM_MIN_RUN_TIME);
    }

    //  printf("kernelGetSleepUTCTime = %d\r\n", kernelGetSleepUTCTime());

    if (ret != 0)
    {
        taskPrintf(TASK_LEVEL_LOG, "softrtc_fromuptime failed");
    }
}

unsigned char scom_get_gps_status(void)
{
    return mpu_fault_syn_data.gps_status;
}

unsigned char scom_get_emmc_status(void)
{
    return mpu_fault_syn_data.emmc_status;
}

unsigned char scom_get_gprs_status(void)
{
    return mpu_fault_syn_data.gprs_status;
}

unsigned char scom_get_sim_status(void)
{
    return mpu_fault_syn_data.sim_status;
}

unsigned char scom_get_wifi_status(void)
{
    return mpu_fault_syn_data.wifi_status;
}

unsigned char scom_get_usb_status(void)
{
    return mpu_fault_syn_data.usb_status;
}

unsigned char scom_get_ecall_status(void)
{
    return mpu_status_syn_data.ecall_status;
}

unsigned char scom_get_bcall_status(void)
{
    return mpu_status_syn_data.bcall_status;
}

unsigned char scom_get_icall_status(void)
{
    return mpu_status_syn_data.icall_status;
}

unsigned char scom_is_app_can_sleep(void)
{
    return mpu_status_syn_data.app_can_sleep;
}

unsigned char scom_get_upg_status(void)
{
    return mpu_status_syn_data.upg_status;
}

unsigned int scom_get_upg_time(void)
{
    if (scom_get_upg_status())
    {
        return (driverGetUptime() - upg_change_time);
    }
    else
    {
        return 0;
    }
}

unsigned int scom_clear_upg_time(void)
{
    upg_change_time = driverGetUptime();
    return 0;
}


int scom_get_wake_time(void)
{
    int ret, interval;
    unsigned int current_time = 0;

    ret = softrtc_frombasetime(0, &current_time);

    if (ret != 0)
    {
        taskPrintf(TASK_LEVEL_ERROR, "softrtc_frombasetime failed");
        return -1;
    }

    if (wake_time < current_time)
    {
        interval = -1;
    }
    else
    {
        interval = (wake_time - current_time) / 60;
    }

    /* the unit of wake_time and current_time is second
       convert the second into minute */
    return  interval;
}


/*硬件唤醒存在，设置RTC唤醒条件下，到了RTC定时器时间与当前时间差小于PM_MIN_INPUT_WAKEUP，无论是否硬件唤醒运行时间，继续运行PM_MIN_INPUT_WAKEUP + */
/*Cindy*/
void set_sleep_time_in_running(void)
{
    if (wake_time >= kernelGetCurrentUTCTime())
    {
        if ((wake_time - kernelGetCurrentUTCTime()) <= PM_MIN_INPUT_WAKEUP)
        {
            kernelSetSleepUTCTime(wake_time - kernelGetCurrentUTCTime() + wake_time + PM_MIN_RUN_TIME);
            taskPrintf(TASK_LEVEL_LOG, "wake_time arriving in running = %d",
                       wake_time - kernelGetCurrentUTCTime() + PM_MIN_RUN_TIME);
        }
    }
}


