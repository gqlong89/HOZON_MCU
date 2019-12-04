/**
 * @Title: shell_list.c
 * @author yuzhimin
 * @date Jul 27, 2017
 * @version V1.0
 */

#include <acc.h>
#include <adc.h>
#include <app/battery/battery.h>
#include <app/can_mgr/can_mgr.h>
#include <app/can_mgr/can_hardware.h>
#include <app/power/pm_mode.h>
#include <app/power/power.h>
#include <app/scom/scom_para_msg.h>
#include <app/scom/scom_tl_ex.h>
#include <brdInfo.h>
#include <call.h>
#include <config.h>
#include <dbg_uart.h>
#include <flash.h>
#include <mcu_mgr.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <mid/kernel/timing.h>
#include <osif.h>
#include <pm.h>
#include <rtc.h>
#include <shell.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_stdint.h>
#include <thres_diag_futian.h>
#include <wdg.h>


/* show version */
static int showVersion(unsigned int argc, unsigned char **argv)
{
    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - show software version.\r\n", argv[1]);
        return 0;
    }

    shellPrintf(" ");
    shellPrintf(kernelGetVersion());
    shellPrintf("\r\n");

    brd_info_init();

    shellPrintf(" BTL: %s \r\n", brd_get_btlVerStr());
    shellPrintf(" projectCode: %u \r\n", brd_get_projectCode());
    shellPrintf(" brdRev     : %u \r\n", brd_get_boardRevision());
    shellPrintf(" bomRev     : %u \r\n", brd_get_bomRevision());
    return 1;
}

/* show time */
static int showTime(unsigned int argc, unsigned char **argv)
{
    RTCTime localtime;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - show system time.\r\n", argv[1]);
        return 0;
    }

    driverRTCGetTime(&localtime);
    shellPrintf("   On-chip  RTC: %04d/%02d/%02d %02d:%02d:%02d\r\n",
                localtime.RTC_Year + 2000, localtime.RTC_Mon, localtime.RTC_Mday,
                localtime.RTC_Hour, localtime.RTC_Min, localtime.RTC_Sec);

    return 1;
}

/* set time */
static int setTime(unsigned int argc, unsigned char **argv)
{
    RTCTime time;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set system time.\r\n", argv[1]);
        return 0;
    }

    if (argc != 6)
    {
        shellPrintf(" usage: settime year month day hour min sec \r\n");
        return 0;
    }

    time.RTC_Year = atoi((char const *) argv[0]);
    time.RTC_Mon = atoi((char const *) argv[1]);
    time.RTC_Mday = atoi((char const *) argv[2]);
    time.RTC_Hour = atoi((char const *) argv[3]);
    time.RTC_Min = atoi((char const *) argv[4]);
    time.RTC_Sec = atoi((char const *) argv[5]);
    time.RTC_Msec = 0;

    driverRTCSetTime(time);
    //driverExRTCI2CWrite( time );
    initUTCTime();

    showTime(0, NULL);

    return 1;
}

/* set SN */
static int setSN(unsigned int argc, unsigned char **argv)
{
    unsigned int addr;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set terminal SN.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: setsn xxxxxxxx\r\n");
        return 0;
    }

    sscanf((char const *) argv[0], "%u", &addr);

    /* set SN */
    cfg_save_sn(addr);

    scom_tl_ex_para_report(SCOM_PARA_ID_SN);

    if (cfg_restore_sn(&addr) == 0)
    {
        shellPrintf(" Terminal SN: %u\r\n", addr);
        shellPrintf(" it will take effect after reset!\r\n");
    }
    else
    {
        shellPrintf(" It's failed to read SN!\r\n");
    }

    return 1;
}


/* set system running mode */
static int setMode(unsigned int argc, unsigned char **argv)
{
    unsigned int mode = 0xFF;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set mode.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: setmode 0(RUNNING_ONLY)/1(LISTEN_ONLY)/2(SLEEP_ONLY)/3(AUTO)\r\n");
        return 0;
    }

    sscanf((char const *)argv[0], "%u", &mode);

    switch (mode)
    {
        case 0:
            cfg_save_sysmode(0);
            break;

        case 1:
            cfg_save_sysmode(1);
            break;

        case 2:
            cfg_save_sysmode(2);
            break;

        case 3:
            cfg_save_sysmode(3);
            break;

        default:
            shellPrintf(" usage: setmode 0(RUNNING_ONLY)/1(LISTEN_ONLY)/2(SLEEP_ONLY)/3(AUTO)\r\n");
            return 0;
    }

    scom_tl_ex_para_report(SCOM_PARA_ID_SYSMODE);

    shellPrintf(" ok\r\n");

    return 1;
}

static int setvtd(unsigned int argc, unsigned char **argv)
{
    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set Anti Thief.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: setvtd on/off\r\n");
        return 0;
    }

    if ((strcmp((char const *)argv[0], "ON") == 0) || (strcmp((char const *)argv[0], "on") == 0))
    {
        cfg_save_anti_thief(1);
    }
    else if ((strcmp((char const *)argv[0], "OFF") == 0) || (strcmp((char const *)argv[0], "off") == 0))
    {
        cfg_save_anti_thief(0);
    }
    else
    {
        shellPrintf(" usage: setvtd on/off\r\n");
    }

    scom_tl_ex_para_report(SCOM_PARA_ID_VTD);

    return 1;
}

static int setEcall(unsigned int argc, unsigned char **argv)
{
    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set ecall.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: setecall on/off\r\n");
        return 0;
    }

    if ((strcmp((char const *)argv[0], "ON") == 0) || (strcmp((char const *)argv[0], "on") == 0))
    {
        cfg_save_ecall(1);
    }
    else if ((strcmp((char const *)argv[0], "OFF") == 0) || (strcmp((char const *)argv[0], "off") == 0))
    {
        cfg_save_ecall(0);
    }
    else
    {
        shellPrintf(" usage: setecall on/off\r\n");
    }

    scom_tl_ex_para_report(SCOM_PARA_ID_ECALL);

    return 1;
}

static int setBcall(unsigned int argc, unsigned char **argv)
{
    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set bcall.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: setbcall on/off\r\n");
        return 0;
    }

    if ((strcmp((char const *)argv[0], "ON") == 0) || (strcmp((char const *)argv[0], "on") == 0))
    {
        cfg_save_bcall(1);
    }
    else if ((strcmp((char const *)argv[0], "OFF") == 0) || (strcmp((char const *)argv[0], "off") == 0))
    {
        cfg_save_bcall(0);
    }
    else
    {
        shellPrintf(" usage: setbcall on/off\r\n");
    }

    scom_tl_ex_para_report(SCOM_PARA_ID_BCALL);

    return 1;
}

static int setIcall(unsigned int argc, unsigned char **argv)
{
    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set icall.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: seticall on/off\r\n");
        return 0;
    }

    if ((strcmp((char const *)argv[0], "ON") == 0) || (strcmp((char const *)argv[0], "on") == 0))
    {
        cfg_save_icall(1);
    }
    else if ((strcmp((char const *)argv[0], "OFF") == 0) || (strcmp((char const *)argv[0], "off") == 0))
    {
        cfg_save_icall(0);
    }
    else
    {
        shellPrintf(" usage: seticall on/off\r\n");
    }

    scom_tl_ex_para_report(SCOM_PARA_ID_ICALL);

    return 1;
}

static int setstime(unsigned int argc, unsigned char **argv)
{
    unsigned int sleep_time;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set standby sleep time.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: setstime <time(min)>\r\n");
        return 0;
    }

    sscanf((char const *) argv[0], "%u", &sleep_time);

    cfg_save_standbysleep(sleep_time);

    shellPrintf(" standby sleep time: %u\r\n", sleep_time);

    scom_tl_ex_para_report(SCOM_PARA_ID_SLP_TIME);

    return 1;
}

static int setdstime(unsigned int argc, unsigned char **argv)
{
    unsigned int deep_sleep_time;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set standby deep sleep time.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: setdstime <time(min)>\r\n");
        return 0;
    }

    sscanf((char const *) argv[0], "%u", &deep_sleep_time);

    cfg_save_deep_standbysleep(deep_sleep_time);

    shellPrintf(" deep sleep time:%u\r\n", deep_sleep_time);

    scom_tl_ex_para_report(SCOM_PARA_ID_DEEP_SLP_TIME);

    return 1;
}

static int setaccvecm(unsigned int argc, unsigned char **argv)
{
    unsigned int acc_vecm_ths;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set ACC VECM THS.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: setaccvecm xxxxxxxx\r\n");
        return 0;
    }

    sscanf((char const *) argv[0], "%u", &acc_vecm_ths);

    cfg_save_acc_vecm_ths((unsigned short)acc_vecm_ths);

    shellPrintf(" ACC VECM THS: %u\r\n", acc_vecm_ths);

    acc_device_init();

    scom_tl_ex_para_report(SCOM_PARA_ID_ACCVECMTHS);

    return 1;
}


/* wdtreset */
static int reset(unsigned int argc, unsigned char **argv)
{
    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - hardware reset.\r\n", argv[1]);
        return 0;
    }

    *((volatile unsigned int *)RESET_FLAG_RAM_ADDR) = RESET_FLAG_RAM_INIT;

    driverReset();
    return 1;
}

/* set Battery type */
static int setBatterytype(unsigned int argc, unsigned char **argv)
{
    unsigned int type = 0;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set Battery type.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: setbat 0(unknown) 1(varte 450) 2(varte 500) 3(panasonic 500)\r\n");
        return 0;
    }

    sscanf((char const *) argv[0], "%u", &type);

    if (type >= BATTERY_TYPE_MAX)
    {
        shellPrintf(" wrong Battery type=%u!\r\n", type);
        shellPrintf(" usage: setbat 0(unknown) 1(varte 450) 2(varte 500) 3(panasonic 500)\r\n");
        return 0;
    }

    cfg_save_batterytype(type);

    shellPrintf(" Battery type=%d!\r\n", cfg_get_batterytype());
    scom_tl_ex_para_report(SCOM_PARA_ID_BAT_TYPE);

    return 1;
}

/* dump configuration */
static int dumpCfg(unsigned int argc, unsigned char **argv)
{
    unsigned int i;
    float adcValue;
    char adcValueStr[20] = { 0 };
    uds_set_def current_uds_set;

    #if FUTIAN_DIAG_ENABLE
    char mic_p[10] = { 0 };
    char spk_p[10] = { 0 };
    char spk_n[10] = { 0 };
    char btn_v[10] = { 0 };
    #endif

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - dump configuration.\r\n", argv[1]);
        return 0;
    }

    shellPrintf(" ----------------------\r\n");
    shellPrintf(" Terminal Addr: %u\r\n", cfg_get_sn());
    shellPrintf(" Reboot: \t%u\r\n", cfg_get_rbt());
    //   shellPrintf(" Real can nums: %u\r\n", can_get_flexcan_phy_num());
    shellPrintf(" ----------------------\r\n");
    shellPrintf(" CAN Default baudrate:\r\n");

    for (i = 0; i < CAN_MAX_PORT; i++)
    {
        shellPrintf(" Can%d: \t%dk\r\n", i + 1, can_default_baud.baud[i].can_normal_baud);
    }

    shellPrintf(" CAN Current baudrate:\r\n");

    for (i = 0; i < CAN_MAX_PORT; i++)
    {
        shellPrintf(" Can%d: \t%dk\r\n", i + 1, can_baud.baud[i].can_normal_baud);
    }

    #if 0
    shellPrintf(" SYSRunTime: \t%llus\r\n", tmpllong / 100);
    driverRestoreINRebootCnt(&tmpllong);
    shellPrintf(" Reboot: \t%llu\r\n", tmpllong);
    shellPrintf(" ----------------------\r\n");

    if (! RTData.ParaStatus)
    {
        shellPrintf(" The saved parameters are lost!\r\n");
    }

    #endif

    switch (cfg_get_sysmode())
    {
        case PM_RUNNING_ONLY:
            shellPrintf(" sys mode: PM_RUNNING_ONLY(0)\r\n");
            break;

        case PM_LISTEN_ONLY:
            shellPrintf(" sys mode: PM_LISTEN_ONLY(1)\r\n");
            break;

        case PM_SLEEP_ONLY:
            shellPrintf(" sys mode: PM_SLEEP_ONLY(2)\r\n");
            break;

        case PM_AUTO:
            shellPrintf(" sys mode: PM_AUTO(3)\r\n");
            break;

        default:
            shellPrintf(" sys mode: Unknown(%u)\r\n", cfg_get_sysmode());
            break;
    }

    switch (pm_get_run_mode())
    {
        case PM_RUNNING_MODE:
            shellPrintf(" run mode: PM_RUNNING_MODE\r\n");
            break;

        case PM_EMERGENCY_MODE:
            shellPrintf(" run mode: PM_EMERGENCY_MODE\r\n");
            break;

        case PM_LISTEN_MODE:
            shellPrintf(" run mode: PM_LISTEN_MODE\r\n");
            break;

        case PM_SLEEP_MODE:
            shellPrintf(" run mode: PM_SLEEP_MODE\r\n");
            break;

        case PM_OFF_MODE:
            shellPrintf(" run mode: PM_OFF_MODE\r\n");
            break;

        default:
            shellPrintf(" run mode: Unknown\r\n");
            break;
    }

    shellPrintf(" ----------------------\r\n");

    switch (cfg_get_anti_thief())
    {
        case 0:
            shellPrintf(" AntiThief: disable\r\n");
            break;

        case 1:
            shellPrintf(" AntiThief: enable\r\n");
            break;

        default:
            shellPrintf(" AntiThief: Unknown\r\n");
            break;
    }

    switch (cfg_get_ecall())
    {
        case 0:
            shellPrintf(" Ecall: disable\r\n");
            break;

        case 1:
            shellPrintf(" Ecall: enable\r\n");
            break;

        default:
            shellPrintf(" Ecall: Unknown\r\n");
            break;
    }

    switch (cfg_get_bcall())
    {
        case 0:
            shellPrintf(" Bcall: disable\r\n");
            break;

        case 1:
            shellPrintf(" Bcall: enable\r\n");
            break;

        default:
            shellPrintf(" Bcall: Unknown\r\n");
            break;
    }

    switch (cfg_get_icall())
    {
        case 0:
            shellPrintf(" Icall: disable\r\n");
            break;

        case 1:
            shellPrintf(" Icall: enable\r\n");
            break;

        default:
            shellPrintf(" Icall: Unknown\r\n");
            break;
    }

    shellPrintf(" standby sleep time: %u\r\n", cfg_get_standbysleep());

    shellPrintf(" standby deep sleep time: %u\r\n", cfg_get_deep_standbysleep());

    switch (cfg_get_standbyflag())
    {
        case STANDBY_NO_SLEEP:
            shellPrintf(" standby sleep type: STANDBY_NO_SLEEP\r\n");
            break;

        case STANDBY_SLEEP:
            shellPrintf(" standby sleep type: STANDBY_SLEEP\r\n");
            break;

        case STANDBY_DEEP_SLEEP:
            shellPrintf(" standby sleep type: STANDBY_DEEP_SLEEP\r\n");
            break;

        default:
            shellPrintf(" standby sleep type: UNKNOW\r\n");
            break;
    }

    shellPrintf(" ACC VECM THS: %u\r\n", cfg_get_acc_vecm_ths());

    /* WakeUpSource status */
    shellPrintf(" ----------------------\r\n");

    shellPrintf(" WakeUpSource: \tA15(fast charge)-%d\tE2(slow charge)-%d\tE12(ACC)-%d\tA1(ON)-%d\r\n \
        \tB9(RING)-%d\tE4(CAN1)-%d\tA12(CAN2)-%d\tC16(CAN3)-%d\tC7(RTC)-%d\r\n \
        \tE7(G_SENSOR)-%d\tB15(PM)-%d\tE13(USB)-%d\tD17(RF)-%d\r\n",
                (driverGetWakeUpSrc() & POWERSTATUS_MASK_FCHARGE) ? 1 : 0,
                (driverGetWakeUpSrc() & POWERSTATUS_MASK_NCHARGE) ? 1 : 0,
                (driverGetWakeUpSrc() & POWERSTATUS_MASK_ACC) ? 1 : 0,
                (driverGetWakeUpSrc() & POWERSTATUS_MASK_ON) ? 1 : 0,
                (driverGetWakeUpSrc() & POWERSTATUS_MASK_RING) ? 1 : 0,
                (driverGetWakeUpSrc() & POWERSTATUS_MASK_CAN1) ? 1 : 0,
                (driverGetWakeUpSrc() & POWERSTATUS_MASK_CAN2) ? 1 : 0,
                (driverGetWakeUpSrc() & POWERSTATUS_MASK_CAN3) ? 1 : 0,
                (driverGetWakeUpSrc() & POWERSTATUS_MASK_RTC) ? 1 : 0,
                (driverGetWakeUpSrc() & POWERSTATUS_MASK_G_SENSOR) ? 1 : 0,
                (driverGetWakeUpSrc() & POWERSTATUS_MASK_PM) ? 1 : 0,
                (driverGetWakeUpSrc() & POWERSTATUS_MASK_USB) ? 1 : 0,
                (driverGetWakeUpSrc() & POWERSTATUS_MASK_RF) ? 1 : 0);

    /* GPS status */
    shellPrintf(" ----------------------\r\n");

    /* GPS Antenna status */
    switch (kernelGetGpsAntStatus())
    {
        case GPSANT_OK:
            shellPrintf(" GPS Ant OK!\r\n");
            break;

        case GPSANT_SHORT:
            shellPrintf(" GPS Ant short Err!\r\n");
            break;

        case GPSANT_OPEN:
            shellPrintf(" GPS Ant OPEN Err!\r\n");
            break;

        default:
            shellPrintf(" GPS Ant unknow Err!\r\n");
            break;
    }

    switch (kernelGetMICVStatus())
    {
        case MICV_OK:
            shellPrintf(" MIC Voltage OK!\r\n");
            break;

        case MICV_ERROR:
            shellPrintf(" MIC Voltage Err!\r\n");
            break;

        default:
            shellPrintf(" MIC Voltage unknow Err!\r\n");
            break;
    }

    #if FUTIAN_DIAG_ENABLE
    float_to_string((3.3 * adc_MIC_POSITIVE() / 4096) , mic_p);

    switch (kernelGetMICStatus())
    {
        case MIC_OK:
            shellPrintf(" MIC Status OK!\r\n");
            break;

        case MIC_SHORT_POWER:
            shellPrintf(" MIC Status  short_Power Err! Vmic+ = %s\r\n", mic_p);
            break;

        case MIC_SHORT_GND:
            shellPrintf(" MIC Status  short_Gnd Err! Vmic+ = %s\r\n", mic_p);
            break;

        case MIC_OPEN:
            shellPrintf(" MIC Status  OPEN Err! Vmic+ = %s\r\n", mic_p);
            break;

        default:
            shellPrintf(" MIC Status  unknow Err! Vmic+ = %s\r\n", mic_p);
            break;
    }

    /*SPK+/-*/
    float_to_string((3.3 * adc_spk_positive() / 4096) , spk_p);
    float_to_string((3.3 * adc_spk_negative() / 4096), spk_n);

    switch (kernelGetSPKStatus())
    {
        case SPK_UNKNOW:
            shellPrintf(" SPK unknow! Vspk+ = %s, Vspk- = %s\r\n", spk_p, spk_n);
            break;

        case SPK_SHORT:

            shellPrintf(" SPK short: error! Vspk+ = %s, Vspk- = %s\r\n", spk_p, spk_n);
            break;

        case SPK_OPEN:
            shellPrintf(" SPK open: error! Vspk+ = %s, Vspk- = %s\r\n", spk_p, spk_n);
            break;

        case SPK_OK:
            shellPrintf(" SPK ok\r\n");
            break;

        default:
            shellPrintf(" SPK unknow! Vspk+ = %s, Vspk- = %s\r\n", spk_p, spk_n);
            break;
    }

    float_to_string(3.3 * adc_get_call_type() / 4096 , btn_v);

    switch (kernelGetSOSStatus())
    {
        case SOS_UP:

            shellPrintf(" SOS DISABE Vbtn = %s\r\n", btn_v);
            break;

        case SOS_DOWN:
            shellPrintf(" SOS ENABLE\r\n");
            break;

        default:
            shellPrintf(" SOS status: unknown. Vbtn = %s\r\n", btn_v);
            break;
    }

    switch (accstatus)
    {
        case ACC_R_ERROR:
            shellPrintf(" ACC receive ERROR\r\n");
            break;

        case ACC_R_OK:
            shellPrintf(" ACC receive OK\r\n");
            break;

        default:
            shellPrintf(" ACC receive UNKNOW\r\n");
            break;
    }

    #endif

    shellPrintf(" ----------------------\r\n");
    adcValue = (3.3 * (49900 + 4700) * adc_Abat() / 4096 / 4700) * VOLTAGE_SLOPE + VOLTAGE_COMPENSATION;
    float_to_string(adcValue, adcValueStr);
    shellPrintf(" VIN: %s \r\n", adcValueStr);

    shellPrintf(" board temprature: %d \r\n", (int)kernelGetTemp());


    switch (cfg_get_batterytype())
    {
        case BATTERY_VARTA_450:
            shellPrintf(" battery type: varta 450\r\n");
            break;

        case BATTERY_VARTA_500:
            shellPrintf(" battery type: varta 500\r\n");
            break;

        case BATTERY_PANASONIC_500:
            shellPrintf(" battery type: panasonic 500\r\n");
            break;

        default:
            shellPrintf(" battery type: unknown\r\n");
            break;
    }

    adcValue = BatteryVoltage();
    float_to_string(adcValue, adcValueStr);
    shellPrintf(" VSC: %s\r\n", adcValueStr);
    shellPrintf(" battery temprature: %d\r\n", BatteryGetTemprature());

    shellPrintf(" ----------------------\r\n");
    cfg_restore_uds((unsigned char *)&current_uds_set);
    shellPrintf(" UDS Config: \r\n");
    shellPrintf("\tcanport[from 0] = %u\t\tu16N_Bs = %u\t\tu16N_Cr = %u\r\n\
        u32CANID_Fun = 0x%x\tu32CANID_Phy = 0x%x\tu32CANID_Respon = 0x%x\r\n\
        u8FC_BS = %u\t\tu8FC_Stmin = %u\t\tu8FillValue = 0x%x\r\n",
                current_uds_set.canport, current_uds_set.u16N_Bs, current_uds_set.u16N_Cr,
                current_uds_set.u32CANID_Fun, current_uds_set.u32CANID_Phy, current_uds_set.u32CANID_Respond,
                current_uds_set.u8FC_BS, current_uds_set.u8FC_Stmin, current_uds_set.u8FillValue);
    shellPrintf(" ----------------------\r\n");



    return 0;
}

/* set DC */
static int setDC1(unsigned int argc, unsigned char **argv)
{
    unsigned int onoff = 0;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set DC 1.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: setdc1 1(on)/0(off)\r\n");
        return 0;
    }

    sscanf((char const *) argv[0], "%u", &onoff);

    if (onoff == 0)
    {
        driverDcOut1(1);
    }
    else
    {
        driverDcOut1(0);
    }

    return 1;
}

/* set DC */
static int setDC2(unsigned int argc, unsigned char **argv)
{
    unsigned int onoff = 0;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set DC 2.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: setdc2 1(on)/0(off)\r\n");
        return 0;
    }

    sscanf((char const *) argv[0], "%u", &onoff);

    if (onoff == 0)
    {
        driverDcOut2(1);
    }
    else
    {
        driverDcOut2(0);
    }

    return 1;
}

/* wake by timer */
static int timerWake(unsigned int argc, unsigned char **argv)
{
    unsigned char intvl;
    RTCTime localtime;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - timerWake.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: timerwake min\r\n");
        return 0;
    }

    intvl = atoi((char const *) argv[0]);
    driverRTCGetTime(&localtime);
    shellPrintf("   Current time: %02d/%02d/%02d %02d:%02d:%02d\r\n",
                localtime.RTC_Year, localtime.RTC_Mon, localtime.RTC_Mday,
                localtime.RTC_Hour, localtime.RTC_Min, localtime.RTC_Sec);
    shellPrintf("   System will wakeup after %d minutes.\r\n", intvl);

    driverRTCSetWakeIntvl(intvl);

    PowerListen();

    return 0;
}

/* shutdown */
static int shutdown(unsigned int argc, unsigned char **argv)
{
    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - shutdown.\r\n", argv[1]);
        return 0;
    }

    PowerListen();

    //PARA_SetResetTag( RESET_FLAG_INIT );
    //RTData.reboot = 1;

    return 1;
}

extern int showirq(unsigned int argc, unsigned char **argv);

/*static int writeflash(unsigned int argc, unsigned char **argv)
{
    int i,size;
    unsigned char data_buf[256];

    memset(data_buf,0,256);

    sscanf( (char const*) argv[0], "%u", &size );
    for(i=0;i<size;i++)
    {
        taskPrintf( TASK_LEVEL_LOG, "write time = %d",i);
        Flash_WriteBuffer( data_buf, OTA_IMG_START_ADDR + 256*i, 256);
    }
    taskPrintf( TASK_LEVEL_ERROR, "write flash over" );
}*/

#define XMODEM_SOH (0x01)
#define XMODEM_STX (0x02)
#define XMODEM_EOT (0x04)
#define XMODEM_ACK (0x06)
#define XMODEM_NAK (0x15)
#define XMODEM_CAN (0x18)
#define XMODEM_C   (0x43)

unsigned short calCRC16(const unsigned char *data, signed int len)
{
    unsigned int i;
    unsigned short crc = 0;

    while (--len >= 0)
    {
        i = 8;
        crc = crc ^ (((unsigned short) * data++) << 8);

        do
        {
            if (crc & 0x8000)
            {
                crc = (crc << 1) ^ 0x1021;
            }
            else
            {
                crc = crc << 1;
            }
        }
        while (--i);
    }

    return crc;
}

/* update from bin : xmodem  */
static int xBin(unsigned int argc, unsigned char **argv)
{
    unsigned char buf[1200];
    unsigned char expectedSeq = 1;
    unsigned int handshake = 1;
    unsigned int fileLen = 0;
    uint64_t recvTOup = 0;
    unsigned int num;
    unsigned int endSession = 0;
    unsigned int offset = 1;
    unsigned int lastLen = 0;
    unsigned int flash_app_addr = BTL_IMG_START_ADDR;
    uint32_t sectorNumber = 0;
    unsigned char recerrcnt = 0;
    unsigned int busoffcnt = 0;
    unsigned short crc16data = 0;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - xbin.\r\n", argv[1]);
        return 0;
    }

    flash_app_addr = BTL_IMG_START_ADDR;
    shellPrintf(" xbin flash add BTL_IMG_START_ADDR: 0x%x!\r\n", BTL_IMG_START_ADDR);

    g_xModemCommEnable = 1;

    while (dbg_uart_recv(buf, sizeof(buf)));

    buf[0] = XMODEM_C;
    dbg_uart_send(buf, 1);
    handshake = 1;
    recvTOup = driverGetUptime();

    while (1)
    {

        driverWDGFeed();

        if (dbg_uart_recv(&buf[0], 1))
        {
            driverWDGFeed();

            switch (buf[0])
            {
                //busoffcnt=0;
                case XMODEM_SOH:
                    {
                        if (handshake)
                        {
                            handshake = 0;
                        }

                        offset = 1;

                        recvTOup = driverGetUptime();

                        while (1)
                        {
                            driverWDGFeed();

                            // continue recv data
                            num = dbg_uart_recv(buf + offset, 1);
                            driverWDGFeed();

                            if (num)
                            {
                                offset++;
                                // reset timeout
                                recvTOup = driverGetUptime();
                            }

                            if (offset == 133)
                            {
                                crc16data = calCRC16(&buf[3], 128);

                                if ((expectedSeq == (buf[1] + 1) % 0x100) &&
                                    (crc16data == (buf[131] * 256 + buf[132])))
                                {
                                    busoffcnt = 0;
                                    buf[0] = XMODEM_ACK;
                                    dbg_uart_send(buf, 1);
                                    recvTOup = driverGetUptime();
                                    break;
                                }
                                else if ((expectedSeq == buf[1]) && (0xFF - buf[1] == buf[2]) &&
                                         (crc16data == (buf[131] * 256 + buf[132])))
                                {
                                    busoffcnt = 0;
                                    expectedSeq++;
                                    lastLen = 128;

                                    if (lastLen < FLASH_SECTORE_SIZE - flash_app_addr % FLASH_SECTORE_SIZE)
                                    {
                                        if ((flash_app_addr % FLASH_SECTORE_SIZE) == 0)
                                        {
                                            sectorNumber = flash_app_addr / FLASH_SECTORE_SIZE;
                                            driverWDGFeed();
                                            Flash_SE(sectorNumber * FLASH_SECTORE_SIZE);
                                        }

                                    }
                                    else
                                    {
                                        sectorNumber = flash_app_addr / FLASH_SECTORE_SIZE + 1;
                                        driverWDGFeed();
                                        Flash_SE(sectorNumber * FLASH_SECTORE_SIZE);
                                    }

                                    driverWDGFeed();

                                    if (Flash_WriteBufferDirect(&buf[3], flash_app_addr, lastLen) == STATUS_SUCCESS)
                                    {
                                        flash_app_addr += lastLen;
                                        buf[0] = XMODEM_ACK;
                                        dbg_uart_send(buf, 1);
                                        recvTOup = driverGetUptime();
                                        break;
                                    }
                                    else
                                    {
                                        driverWDGFeed();

                                        if (Flash_WriteBuffer(&buf[3], flash_app_addr, lastLen) == STATUS_SUCCESS)
                                        {
                                            flash_app_addr += lastLen;
                                            buf[0] = XMODEM_ACK;
                                            dbg_uart_send(buf, 1);
                                            recvTOup = driverGetUptime();
                                            break;
                                        }
                                        else
                                        {
                                            endSession = 3;
                                            break;
                                            //return FLASH_FAIL;
                                        }
                                    }
                                }
                                else  //校验不通过
                                {
                                    endSession = 1;
                                    break;
                                }

                            }

                            if (driverGetUptime() > recvTOup + 100)  //超时
                            {
                                endSession = 1;
                                break;
                            }
                        }
                    }
                    break;

                case XMODEM_STX:
                    {
                        if (handshake)
                        {
                            handshake = 0;
                        }

                        offset = 1;

                        recvTOup = driverGetUptime();

                        while (1)
                        {
                            driverWDGFeed();

                            // continue recv data
                            num = dbg_uart_recv(buf + offset, 1);
                            driverWDGFeed();

                            if (num)
                            {
                                offset++;
                                // reset timeout
                                recvTOup = driverGetUptime();
                            }

                            if (offset == 1029)
                            {
                                crc16data = calCRC16(&buf[3], 1024);

                                if ((expectedSeq == (buf[1] + 1) % 0x100) &&
                                    (crc16data == (buf[1027] * 256 + buf[1028])))
                                {
                                    busoffcnt = 0;
                                    buf[0] = XMODEM_ACK;
                                    dbg_uart_send(buf, 1);
                                    recvTOup = driverGetUptime();
                                    break;
                                }
                                else if ((expectedSeq == buf[1]) && (0xFF - buf[1] == buf[2]) &&
                                         (crc16data == (buf[1027] * 256 + buf[1028])))
                                {
                                    busoffcnt = 0;
                                    expectedSeq++;
                                    lastLen = 1024;

                                    if (lastLen < FLASH_SECTORE_SIZE - flash_app_addr % FLASH_SECTORE_SIZE)
                                    {
                                        if ((flash_app_addr % FLASH_SECTORE_SIZE) == 0)
                                        {
                                            sectorNumber = flash_app_addr / FLASH_SECTORE_SIZE;
                                            driverWDGFeed();
                                            Flash_SE(sectorNumber * FLASH_SECTORE_SIZE);
                                        }
                                    }
                                    else
                                    {
                                        sectorNumber = flash_app_addr / FLASH_SECTORE_SIZE + 1;
                                        driverWDGFeed();
                                        Flash_SE(sectorNumber * FLASH_SECTORE_SIZE);
                                    }

                                    driverWDGFeed();

                                    if (Flash_WriteBufferDirect(&buf[3], flash_app_addr, lastLen) == STATUS_SUCCESS)
                                    {
                                        flash_app_addr += lastLen;
                                        buf[0] = XMODEM_ACK;
                                        dbg_uart_send(buf, 1);
                                        recvTOup = driverGetUptime();
                                        break;
                                    }
                                    else
                                    {
                                        if (Flash_WriteBuffer(&buf[3], flash_app_addr, lastLen) == STATUS_SUCCESS)
                                        {
                                            flash_app_addr += lastLen;
                                            buf[0] = XMODEM_ACK;
                                            dbg_uart_send(buf, 1);
                                            recvTOup = driverGetUptime();
                                            break;
                                        }
                                        else
                                        {
                                            endSession = 3;
                                            break;
                                            //return FLASH_FAIL;
                                        }
                                    }
                                }
                                else
                                {
                                    endSession = 1;
                                    break;
                                }
                            }

                            if (driverGetUptime() > recvTOup + 100)
                            {
                                endSession = 1;
                                break;
                            }
                        }
                    }
                    break;

                case XMODEM_EOT:
                    {
                        // send ACK
                        buf[0] = XMODEM_ACK;
                        dbg_uart_send(buf, 1);
                        endSession = 2;
                    }
                    break;

                default:
                    {
                        endSession = 1;
                        // do nothing, just record data flow
                        /*if(buf[0] == '\n')
                        {
                            endSession = 3;
                        }*/
                    }
                    break;
            }

            if ((endSession == 3) || (endSession == 2))
            {
                break;
            }
        }
        else
        {
            driverWDGFeed();

            if (handshake)
            {
                if (driverGetUptime() > recvTOup + 100)
                {
                    handshake++ ;

                    if (handshake > 60)
                    {
                        endSession = 3;
                        break;
                    }
                    else
                    {
                        buf[0] = XMODEM_C;
                        dbg_uart_send(buf, 1);
                        recvTOup = driverGetUptime();
                    }
                }
            }
            else
            {
                if (driverGetUptime() > recvTOup + 100)
                {
                    endSession = 1;
                }
            }

        }

        if (endSession == 1) //重传
        {
            driverWDGFeed();

            while (driverGetUptime() < recvTOup + 100);

            driverWDGFeed();

            endSession = 0;

            busoffcnt++;

            if (busoffcnt > 30)
            {
                endSession = 3;
                break;
            }
            else
            {
                driverWDGFeed();

                while (dbg_uart_recv(buf, sizeof(buf)));

                driverWDGFeed();

                buf[0] = XMODEM_NAK;
                dbg_uart_send(buf, 1);
                recvTOup = driverGetUptime();
            }
        }
    }

    if (endSession == 3)
    {
        buf[0] = XMODEM_CAN;
        dbg_uart_send(buf, 1);
    }

    /* clear */
    driverWDGFeed();

    while (dbg_uart_recv(buf, sizeof(buf)));

    g_xModemCommEnable = 0;

    if (endSession == 2)
    {
        Delay(10000000);
        shellPrintf(" recv file OK!\r\n");
    }
    else
    {
        shellPrintf(" recv file ERROR!\r\n");
    }

    g_xModemCommEnable = 0;

    return 1;

}

static int flash_erase(unsigned int argc, unsigned char **argv)
{
    unsigned int section = 0;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - flash erase.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: erase <id>\r\n");
        shellPrintf(" id=0: General config area 0x%08x to 0x%08x\r\n", FLASH_SEC1_START_ADDR,
                    FLASH_SEC1_END_ADDR);
        shellPrintf(" id=1: MCU Flag area       0x%08x to 0x%08x\r\n", FLASH_SEC2_START_ADDR,
                    FLASH_SEC2_END_ADDR);
        shellPrintf(" id=2: Custom config area  0x%08x to 0x%08x\r\n", FLASH_SEC3_START_ADDR,
                    FLASH_SEC3_END_ADDR);
        return 0;
    }

    sscanf((char const *) argv[0], "%u", &section);

    switch (section)
    {
        case 0:
            shellPrintf(" erase 0x%08x to 0x%08x\r\n", FLASH_SEC1_START_ADDR, FLASH_SEC1_END_ADDR);
            cfg_erase(FLASH_SEC1_START_ADDR, FLASH_SEC1_END_ADDR);
            break;

        case 1:
            shellPrintf(" erase 0x%08x to 0x%08x\r\n", FLASH_SEC2_START_ADDR, FLASH_SEC2_END_ADDR);
            cfg_erase(FLASH_SEC2_START_ADDR, FLASH_SEC2_END_ADDR);
            break;

        case 2:
            shellPrintf(" erase 0x%08x to 0x%08x\r\n", FLASH_SEC3_START_ADDR, FLASH_SEC3_END_ADDR);
            cfg_erase(FLASH_SEC3_START_ADDR, FLASH_SEC3_END_ADDR);
            break;

        default:
            shellPrintf(" id=%u error\r\n", section);
            shellPrintf(" id=0: General config area 0x%08x to 0x%08x\r\n", FLASH_SEC1_START_ADDR,
                        FLASH_SEC1_END_ADDR);
            shellPrintf(" id=1: MCU Flag area       0x%08x to 0x%08x\r\n", FLASH_SEC2_START_ADDR,
                        FLASH_SEC2_END_ADDR);
            shellPrintf(" id=2: Custom config area  0x%08x to 0x%08x\r\n", FLASH_SEC3_START_ADDR,
                        FLASH_SEC3_END_ADDR);
            return 0;
    }

    shellPrintf(" it takes effect after reboot mcu!\r\n");
    return 1;
}

static int flash_dump(unsigned int argc, unsigned char **argv)
{
    unsigned int addr;
    unsigned int size;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - display hex content of flash.\r\n", argv[1]);
        return 0;
    }

    if (argc != 2)
    {
        shellPrintf(" usage: df <addr> <size>\r\n");
        shellPrintf(" addr from 0x%08x to 0x%08x\r\n", FLASH_SEC1_START_ADDR, FLASH_SEC3_END_ADDR - 1);
        return 0;
    }

    sscanf((char const *) argv[0], "%x", &addr);
    sscanf((char const *) argv[1], "%u", &size);

    if (addr >= FLASH_SEC3_END_ADDR || addr < FLASH_SEC1_START_ADDR)
    {
        shellPrintf(" Addr out of range. addr:0x%08x\r\n", addr);
        return 0;
    }

    if (size == 0 || size + addr > FLASH_SEC3_END_ADDR)
    {
        shellPrintf(" Size error or Addr out of range.addr:0x%08x,size:%u\r\n", addr, size);
        return 0;
    }

    cfg_dump(addr, size);

    return 1;
}

/* MPU reset enable */
static int mpu_reset(unsigned int argc, unsigned char **argv)
{
    unsigned int enable;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set MPU reset enable or disable.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: lte <enable>\r\n");
        shellPrintf(" 0: disable, 1: enable\r\n");
        return 0;
    }

    sscanf((char const *) argv[0], "%u", &enable);

    if (1 == enable)
    {
        shellPrintf(" set MPU reset enable\r\n");
        mpu_reset_flag(true);
    }
    else if (0 == enable)
    {
        shellPrintf(" set MPU reset disable\r\n");
        mpu_reset_flag(false);
    }
    else
    {
        shellPrintf(" usage: lte <enable>\r\n");
        shellPrintf(" 0: disable, 1: enable\r\n");
    }

    return 1;
}

static int dumpwakesrc(unsigned int argc, unsigned char **argv)
{
    unsigned char sys_mode, run_mode;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - Displays the current wake source state.\r\n", argv[1]);
        return 0;
    }

    sys_mode = cfg_get_sysmode();

    switch (sys_mode)
    {
        case PM_RUNNING_ONLY:
            shellPrintf(" sys mode:running only\r\n");
            break;

        case PM_LISTEN_ONLY:
            shellPrintf(" sys mode:listen only\r\n");
            break;

        case PM_SLEEP_ONLY:
            shellPrintf(" sys mode:sleep only\r\n");
            break;

        case PM_AUTO:
            shellPrintf(" sys mode:auto\r\n");
            break;

        default:
            shellPrintf(" sys mode:unknown\r\n");
            break;
    }

    run_mode = pm_get_run_mode();

    switch (run_mode)
    {
        case PM_RUNNING_MODE:
            shellPrintf(" current mode:running\r\n");
            break;

        case PM_EMERGENCY_MODE:
            shellPrintf(" current mode:emergency\r\n");
            break;

        case PM_LISTEN_MODE:
            shellPrintf(" current mode:listen\r\n");
            break;

        case PM_SLEEP_MODE:
            shellPrintf(" current mode:sleep\r\n");
            break;

        case PM_DEEP_SLEEP_MODE:
            shellPrintf(" current mode:deep sleep\r\n");
            break;

        case PM_OFF_MODE:
            shellPrintf(" current mode:off\r\n");
            break;

        default:
            shellPrintf(" current mode:unknown\r\n");
            break;
    }

    /*  防盗跟踪功能打开并且防盗功能已报警 */
    if (get_anti_thief_trigger())
    {
        shellPrintf(" anti thief is active\r\n");
    }

    /*  CAN1,CAN2,CAN3任意一路CAN有CAN报文 */
    if (get_can_status())
    {
        shellPrintf(" can is active\r\n");
    }

    /*  ECALL/BCALL按键被按下或者当前有 ECALL/BCALL通话 */
    if (call_get_status())
    {
        shellPrintf(" call is active\r\n");
    }

    if (pm_get_s_charge_wakeup())
    {
        shellPrintf(" slow charge is active\r\n");
    }

    if (pm_get_acc_wakeup())
    {
        shellPrintf(" acc is active\r\n");
    }

    if (pm_get_on_wakeup())
    {
        shellPrintf(" on is active\r\n");
    }

    if (pm_get_virtual_on_status())
    {
        shellPrintf("virtual on is active\r\n");
    }

    if (pm_get_usb_wakeup())
    {
        shellPrintf(" usb is active\r\n");
    }

    if (pm_get_f_charge_wakeup())
    {
        shellPrintf(" fast charge is active\r\n");
    }

    if (pm_get_ring_wakeup())
    {
        shellPrintf(" ring is active\r\n");
    }

    if (scom_get_upg_status())
    {
        shellPrintf(" upgrade status is active\r\n");
    }

    /* KL30 is disconnected, TBOX will enter emergency mode */
    if ((!BatteryInsertDetect()) && (get_powervoltage() < PM_KL30_MIN_POW_VOL))
    {
        shellPrintf(" emergency is active\r\n");
    }

    /* RTC 唤醒时间在5分钟内 */
    if ((scom_get_wake_time() >= 0) && (scom_get_wake_time() <= 5))
    {
        shellPrintf(" rtc wakeup time <= 5 min\r\n");
    }

    if (pm_sleep_available())
    {
        if ((cfg_get_sysmode() != PM_RUNNING_ONLY) && (kernelGetSleepUTCTime() > kernelGetCurrentUTCTime()))
        {
            shellPrintf(" wait %u seconds to enter low power\r\n",
                        kernelGetSleepUTCTime() - kernelGetCurrentUTCTime());
        }
    }

    return 0;
}


static int charge_set(unsigned int argc, unsigned char **argv)
{
    unsigned int enable;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - charge_set.0: stop, 1: stardard, 2: trickle\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: lte <enable>\r\n");
        shellPrintf(" 0: stop, 1: stardard, 2: trickle\r\n");
        return 0;
    }

    sscanf((char const *) argv[0], "%u", &enable);

    VartaChargePinStatus();

    if (0 == enable)
    {

        if (cfg_get_batterytype() == BATTERY_VARTA_450 || cfg_get_batterytype() == BATTERY_VARTA_500)
        {
            pm_cpu_charge_ctrl(false);
            pm_cpu_charge_trickle_ctrl(false);
            pm_cpu_charge_trickle_varta_ctrl(false);
        }
        else
        {
            pm_cpu_charge_trickle_ctrl(false);  //Cindy
            pm_cpu_charge_ctrl(false);
            pm_cpu_charge_trickle_varta_ctrl(false);
        }

        shellPrintf(" stop charge\r\n");

    }
    else if (1 == enable)
    {
        if (cfg_get_batterytype() == BATTERY_VARTA_450 || cfg_get_batterytype() == BATTERY_VARTA_500)
        {
            pm_cpu_charge_ctrl(true);
            pm_cpu_charge_trickle_ctrl(true);
            pm_cpu_charge_trickle_varta_ctrl(false);
        }

        else
        {
            pm_cpu_charge_trickle_ctrl(true);  //Cindy
            pm_cpu_charge_ctrl(true);
            pm_cpu_charge_trickle_varta_ctrl(false);
        }

        shellPrintf(" standard charge\r\n");


    }
    else if (2 == enable)
    {
        if (cfg_get_batterytype() == BATTERY_VARTA_450 || cfg_get_batterytype() == BATTERY_VARTA_500)
        {
            pm_cpu_charge_ctrl(false);
            pm_cpu_charge_trickle_ctrl(false);
            pm_cpu_charge_trickle_varta_ctrl(true);
        }
        else
        {
            pm_cpu_charge_trickle_ctrl(true);
            pm_cpu_charge_ctrl(false);
            pm_cpu_charge_trickle_varta_ctrl(false);
        }

        shellPrintf(" trickle charge\r\n");
    }
    else
    {
        shellPrintf(" usage: lte <enable>\r\n");
        shellPrintf(" 0: stop, 1: stardard, 2: trickle\r\n");
    }

    VartaChargePinStatus();

    return 1;
}

static int setWakeInterval(unsigned int argc, unsigned char **argv)
{
    unsigned int wake_interval;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set wakeup interval.\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" usage: setwakeval <time(min)>\r\n");
        return 0;
    }

    sscanf((char const *) argv[0], "%u", &wake_interval);

    if (0 == wake_interval)
    {
        shellPrintf(" 0 is invalid parameter\r\n");
        return 0;
    }

    pm_set_wakeup_val(wake_interval);

    shellPrintf(" set wake interval: %u\r\n", wake_interval);

    return 1;
}

static int setbatprint(unsigned int argc, unsigned char **argv)
{
    unsigned int enable;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - print bat temperature or not\r\n", argv[1]);
        return 0;
    }

    if (argc != 1)
    {
        shellPrintf(" 0: not printf  bat temperature; 1: printf  bat temperature\r\n");
        return 0;
    }

    sscanf((char const *) argv[0], "%u", &enable);

    BatterySetTempPrintFlag(enable);

    return 1;
}
void shell_list_init(void)
{
    shell_cmd_register("version", showVersion);
    shell_cmd_register("task", kernelShowTask);
    shell_cmd_register("timer", kernelShowTimer);
    shell_cmd_register("dbg", kernelShowDbg);
    shell_cmd_register("setdbg", kernelSetdbg);
    shell_cmd_register("time", showTime);
    shell_cmd_register("settime", setTime);
    shell_cmd_register("setsn", setSN);
    shell_cmd_register("setmode", setMode);
    shell_cmd_register("setvtd", setvtd);
    shell_cmd_register("setecall", setEcall);
    shell_cmd_register("setbcall", setBcall);
    shell_cmd_register("seticall", setIcall);
    shell_cmd_register("setstime", setstime);
    shell_cmd_register("setdstime", setdstime);
    shell_cmd_register("setaccvecm", setaccvecm);
    shell_cmd_register("reset", reset);
    shell_cmd_register("setbat", setBatterytype);
    shell_cmd_register("dumpcfg", dumpCfg);
    shell_cmd_register("setdc1", setDC1);
    shell_cmd_register("setdc2", setDC2);
    shell_cmd_register("timerwake", timerWake);
    shell_cmd_register("shutdown", shutdown);
    shell_cmd_register("showirq", showirq);
    shell_cmd_register("xbin", xBin);
    shell_cmd_register("erase", flash_erase);
    shell_cmd_register("df", flash_dump);
    shell_cmd_register("mpu", mpu_reset);
    shell_cmd_register("wakesrc", dumpwakesrc);
    shell_cmd_register("charge_set", charge_set);
    shell_cmd_register("wakeval", setWakeInterval);
    shell_cmd_register("batprint", setbatprint);
    //shell_cmd_register( "wf", writeflash );
}

