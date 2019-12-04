/**
 * @Title: mcu_task.c
 * @author yuzhimin
 * @date Jul 8, 2017
 * @version V1.0
 */

#include <acc.h>
#include <adc.h>
#include <app/battery/battery.h>
#include <app/power/pm_mode.h>
#include <app/power/power.h>
#include <app/scom/scom_tl.h>
#include <call.h>
#include <config.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <mid/kernel/timing.h>
#include <osif.h>
#include <pm.h>
#include <rtc.h>
#include <scom.h>
#include <softrtc.h>
#include <srs.h>
#include <stdbool.h>
#include <string.h>
#include <sys/_stdint.h>
#include <thres_diag_futian.h>
#include <time.h>
#include <app/custom/custom.h>


#define MCU_MANAGER_REQUEST             TASK_SYS_EVENT_REQST0
#define MPU_RESET_EVENT                 TASK_SYS_EVENT_REQST1
#define ADC_POLLING_REQUEST             TASK_SYS_EVENT_REQST2

#define SCOM_TIMEOUT                    (25*60*100)  // 25min
#define GPRS_RESET_SAFE_OFF_INTERVAL    (2000)      // 2S
#define MPU_MAX_FAULT_CNT               10
#define ADC_POLLING_VALUE               10          // 20ms
static unsigned char   AdcPollingTimerNo;

static unsigned char mcu_timer;
static unsigned char mpu_reset_timer;

volatile unsigned int gpsAntStatus = GPSANT_UNKNOW;
volatile unsigned int micVoltageStatus = MICV_ERROR;
volatile unsigned int micStatus = MIC_UNKNOW;
static unsigned int micCheckCount = 0;

volatile unsigned int sosbtnstatus = SOS_UNKNOW;        //added by Cindy 0409 sos按键状态
volatile unsigned int spkstatus = SPK_UNKNOW;       //added by Cindy 0411 spk开短路正常状态

uint8_t mcu_taskid = INVALID_TASK_NUMBER;

volatile unsigned int CurrentUTCTime = 0;
static volatile unsigned int  BootUTCTime = 0;
static volatile unsigned int  SleepUTCTime = 0;
static unsigned int SaveCurrenttimeCount = 0;
static unsigned char RTCfeil_initStep = 0;

/* RTC init status */
enum
{
    RTC_INIT_FAIL = 0,
    RTC_INIT_SUCCESS
};
static unsigned int RTCInitFlag = RTC_INIT_FAIL;

unsigned int localRTCisSet = 0;
/* init RTC */
int RTCInit(void)
{
    unsigned int ret = 0;
    RTCTime rtcTime;

    driverRTCInit();

    ret = driverExRTCI2CRead(&rtcTime);

    if (ret != 0)
    {
        /* 2000.1.1 0:0:0 */
        rtcTime.RTC_Year = 0;
        rtcTime.RTC_Mon = 1;
        rtcTime.RTC_Mday = 1;
        rtcTime.RTC_Hour = 0;
        rtcTime.RTC_Min = 0;
        rtcTime.RTC_Sec = 0;
        softrtc_init(driverGetUptime(), &rtcTime, 0);

        if (ret & 0x2)     // rtc init failed
        {
            return 1;
        }
        else    // rtc time is wrong
        {
            driverRTCSetInitTime(rtcTime);
        }
    }
    else
    {
        softrtc_init(driverGetUptime(), &rtcTime, 1);
        localRTCisSet = 1;
    }

    return 0;
}

#if FUTIAN_DIAG_ENABLE
static void mic_check(void)
{
    unsigned int micStatus_temp = 0;
    static unsigned int micStatus_last = 0;
    static unsigned int micStatus_count = 0;

    if (get_mic_test_flag())
    {
        if (micCheckCount < 50)
        {
            micCheckCount++;

            /*Modified by Cindy 0409 硬件上仅通过ADC_MIC+采集进行判断*/
            if ((adc_MIC_POSITIVE() >= MIC_P_IN_MIN) && (adc_MIC_POSITIVE() <= MIC_P_IN_MAX))
            {
                micStatus_temp = MIC_OK;
            }
            else if ((adc_MIC_POSITIVE() >= MIC_P_OPEN_MIN) && (adc_MIC_POSITIVE() <= MIC_P_OPEN_MAX))
            {
                micStatus_temp = MIC_OPEN;
            }
            else if ((adc_MIC_POSITIVE() >= MIC_P_SHORT_POWER_MIN)
                     && (adc_MIC_POSITIVE() <= MIC_P_SHORT_POWER_MAX))
            {
                micStatus_temp = MIC_SHORT_POWER;
            }
            else if ((adc_MIC_POSITIVE() <= MIC_P_SHORT_GND_MAX))
            {
                micStatus_temp = MIC_SHORT_GND;
            }
            else
            {
                micStatus_temp = MIC_UNKNOW;
            }

            if (micStatus_temp == micStatus_last)
            {
                micStatus_count ++;
                //                printf("mic+ = %d\r\n", adc_MIC_POSITIVE());

                if (micStatus_count > 10)
                {
                    micStatus = micStatus_temp;
                    set_mic_test_flag(false);   //稳定后将MIC测试标志置为false
                    micStatus_count = 0;
                }
            }
            else
            {
                micStatus_count = 0;
            }

            micStatus_last = micStatus_temp;
        }
        else
        {
            set_mic_test_flag(false);        //若50次到了，仍然没有稳定，置测试标志为false
        }
    }
    else
    {
        micCheckCount = 0;
        /*
                if (get_mic_test_flag())
                {
                    set_mic_test_flag(false);
                }
        */
    }
}

static void sos_btn_check(void)
{
	#if 0
    if (adc_get_call_type() >= BTN_DOWN_750_MIN && adc_get_call_type() <= BTN_DOWN_750_MAX)
    {
        sosbtnstatus =  SOS_DOWN;
        return;
    }
	#endif
    if (adc_get_call_type() <= BTN_DOWN_750_470_MIN)
    {
        sosbtnstatus = SOS_DOWN;
		//printk(PRINTK_LEVEL_LOG, "SOS_DOWN");
        return;
    }

    if (adc_get_call_type() >= BTN_UP_MIN)
    {
        sosbtnstatus = SOS_UP;
        return;
    }

    sosbtnstatus = SOS_UNKNOW;
    return;
}

static unsigned int spk_once_check(void)
{
    if (adc_spk_positive() >= SPK_P_OPEN_MIN && adc_spk_negative() <= SPK_N_OPEN_MAX)
    {
        return SPK_OPEN;
    }
    else if (adc_spk_positive() > adc_spk_negative() &&
             adc_spk_positive() - adc_spk_negative() < SPK_SHORT_DELTA_MAX)
    {
        return SPK_SHORT;
    }
    else if (adc_spk_positive() > adc_spk_negative() &&
             adc_spk_positive() - adc_spk_negative() > SPK_IN_DELTA_MIN)
    {
        return SPK_OK;
    }

    else
    {
        return SPK_UNKNOW;
    }

}


static void spk_check(void)
{
    unsigned int spkStatus_temp = 0;
    static unsigned int spkStatus_last = 0;
    static unsigned int spkStatus_count = 0;


    /*开短路判断，阀值的界定需要根据实际情况略有偏移*/

    //    printf("spk+ = %d, spk- =%d\r\n", adc_spk_positive(), adc_spk_negative());

    if (get_spk_test_flag())
    {
        if (spkStatus_count < 20)
        {
            spkStatus_temp = spk_once_check();

            if (spkStatus_temp == spkStatus_last)
            {
                spkStatus_count++;

                if (spkStatus_count > 10)
                {
                    spkstatus = spkStatus_temp;
                    spkStatus_count = 0;
                    spk_check_switch(false);
                }
            }
            else
            {
                spkStatus_count = 0;
            }

            spkStatus_last = spkStatus_temp;

        }
        else
        {
            spk_check_switch(false);
        }
    }
    else
    {
        spkStatus_count = 0;
    }

}
#endif


static void adc_check(void)
{
    int v_tmp_gns_h, v_tmp_gns_l;

    /*
            FT GL GPS ANT THRED
            0.05V< ADCH-ADCL<0.5V     normal

            ADCH-ADCL<0.05V           open

            ADCH-ADCL>0.8V            short
    */
    #if 0

    if (((adc_Gps_H() - adc_Gps_L()) > 993) && (adc_Gps_H() > adc_Gps_L()))
    {
        gpsAntStatus = GPSANT_SHORT;
    }
    else if (((adc_Gps_H() - adc_Gps_L()) > 62)
             && ((adc_Gps_H() - adc_Gps_L()) < 621))
    {
        gpsAntStatus = GPSANT_OK;
    }
    else if ((adc_Gps_H() - adc_Gps_L()) < 62)
    {
        gpsAntStatus = GPSANT_OPEN;
    }
    else
    {
        gpsAntStatus = GPSANT_UNKNOW;
    }

    #endif

    v_tmp_gns_h = adc_Gps_H();
    v_tmp_gns_l = adc_Gps_L();

    //taskPrintf(TASK_LEVEL_ERROR, "H:%04u L:%04u sub:%04d",v_tmp_gns_h,v_tmp_gns_l,v_tmp_gns_h-v_tmp_gns_l);

    /* 电咖GPS天线检测start */
    if (v_tmp_gns_l < 1240)
    {
        gpsAntStatus = GPSANT_SHORT;
    }
    else if (((v_tmp_gns_h - v_tmp_gns_l) >= 38) && (v_tmp_gns_h > v_tmp_gns_l))
    {
        gpsAntStatus = GPSANT_OK;
    }
    else if ((v_tmp_gns_h - v_tmp_gns_l) < 38)
    {
        gpsAntStatus = GPSANT_OPEN;
    }
    else
    {
        gpsAntStatus = GPSANT_UNKNOW;
    }

    /* 电咖GPS天线检测end */

    #if FUTIAN_DIAG_ENABLE
    {
        mic_check();
        sos_btn_check();    //sos开关
        //spk_check();
        spkstatus = FAULT_NOT_SUPPORT;      /*not support*/
    }
    #else
    {
        micStatus = FAULT_NOT_SUPPORT;
        sosbtnstatus = FAULT_NOT_SUPPORT;
        spkstatus = FAULT_NOT_SUPPORT;
    }

    #endif

    //    printf("adc_spk_positive = %d, adc_spk_negative = %d\r\n", adc_spk_positive(), adc_spk_negative());
}

void mpu_reset_check(void)
{
    unsigned int reason = 0;

    reason |= (scom_get_timeout() > SCOM_TIMEOUT) ? 1 : 0;
    reason |= (scom_get_fault_cnt() > MPU_MAX_FAULT_CNT) ? 2 : 0;
    reason |= (scom_get_upg_time() > SCOM_TIMEOUT) ? 4 : 0;

    if (reason && (PM_RUNNING_MODE == pm_get_run_mode() || PM_EMERGENCY_MODE == pm_get_run_mode()))
    {
        taskPrintf(TASK_LEVEL_ERROR, "MPU_RESET_EVENT!!!");
        taskPrintf(TASK_LEVEL_ERROR, "reset reason: 0x%08x", reason);
        kernelSetTaskEvent(mcu_taskid, MPU_RESET_EVENT);
        scom_timeout_refresh();
        scom_clear_fault_cnt();
        scom_clear_upg_time();
    }
}

static bool mpu_reset_enable = true;

void mpu_reset_flag(bool enable)
{
    mpu_reset_enable = enable;
}

void mpu_reset_proc(void)
{
    static unsigned char step = 1;
    taskPrintf(TASK_LEVEL_LOG, "4G Reset step:%d", step);

    if (false == mpu_reset_enable)
    {
        taskPrintf(TASK_LEVEL_LOG, "4G Reset disable");
        return;
    }

    switch (step)
    {
        /* setp 1: 4G safe off */
        case 1:
            pm_4g_reset();
            taskPrintf(TASK_LEVEL_LOG, "[%d]:reset 4g module", step);
            kernelDisableTimer(mpu_reset_timer);
            kernelEnableTimer(mpu_reset_timer, GPRS_RESET_SAFE_OFF_INTERVAL, MPU_RESET_EVENT);
            step = 2;
            break;

        /* setp 2: 4G safe on */
        case 2:
            pm_4g_safe_on();
            taskPrintf(TASK_LEVEL_LOG, "[%d]:safe on 4g module", step);
            step = 1;
            break;
            #if 0
            taskPrintf(TASK_LEVEL_LOG, "[%d]:safe off failed, wait times: [%d]", step, safe_on_timeout);
            kernelDisableTimer(mpu_reset_timer);
            kernelEnableTimer(mpu_reset_timer, GPRS_RESET_SAFE_OFF_INTERVAL, MPU_RESET_EVENT);

            if (safe_on_timeout < 20)
            {
                step = 2;
                break;
            }
            else
            {
                step = 3;
                break;
            }

        case 3:
            pm_4g_power_ctrl(false);
            taskPrintf(TASK_LEVEL_LOG, "[%d]:power off 4g module", step);
            kernelDisableTimer(mpu_reset_timer);
            kernelEnableTimer(mpu_reset_timer, GPRS_RESET_SAFE_OFF_INTERVAL, MPU_RESET_EVENT);
            step = 4;
            break;

        case 4:
            pm_4g_power_ctrl(true);
            taskPrintf(TASK_LEVEL_LOG, "[%d]:power on 4g module", step);
            kernelDisableTimer(mpu_reset_timer);
            kernelEnableTimer(mpu_reset_timer, GPRS_RESET_SAFE_OFF_INTERVAL, MPU_RESET_EVENT);
            step = 2;
            break;
            #endif

        default:
            break;
    }
}

static void scanUTCTime(void)
{
    unsigned int tmpUTCTime;

    if (BootUTCTime)
    {
        tmpUTCTime = BootUTCTime + (driverGetUptime() / 100);
        CurrentUTCTime = tmpUTCTime;
    }
    else
    {
        SaveCurrenttimeCount = 0;
    }

    {
        SaveCurrenttimeCount = (SaveCurrenttimeCount + 1) % 1800;

        if (SaveCurrenttimeCount == 30)
        {
            cfg_save_batchktime(CurrentUTCTime);
        }
    }
}

void initUTCTime(void)
{
    unsigned int ret = 0;
    RTCTime rtcTime;
    struct tm tmTime;
    unsigned int tmpUTCTime;

    memset(&tmTime, 0, sizeof(tmTime));
    ret = driverExRTCI2CRead(&rtcTime);

    if (ret == 0)
    {
        tmTime.tm_year = rtcTime.RTC_Year + 100;
        tmTime.tm_mon  = rtcTime.RTC_Mon - 1;
        tmTime.tm_mday = rtcTime.RTC_Mday;
        tmTime.tm_hour = rtcTime.RTC_Hour;
        tmTime.tm_min  = rtcTime.RTC_Min;
        tmTime.tm_sec  = rtcTime.RTC_Sec;
        tmpUTCTime = mktime(&tmTime);
    }
    else
    {
        tmpUTCTime = (driverGetUptime() / 100) + basetime;
    }

    CurrentUTCTime = tmpUTCTime;
    BootUTCTime    = CurrentUTCTime - (driverGetUptime() / 100);

}

unsigned int kernelGetCurrentUTCTime(void)
{
    return CurrentUTCTime;
}

unsigned int kernelGetSleepUTCTime(void)
{
    return SleepUTCTime;
}

void kernelSetSleepUTCTime(unsigned int sleeptime)
{
    SleepUTCTime = sleeptime;
}

/* get GPS Antenna status */
unsigned int kernelGetGpsAntStatus(void)
{
    return gpsAntStatus;
}

/* get MIC Voltage status */
unsigned int kernelGetMICVStatus(void)
{
    return micVoltageStatus;
}

/* get MIC status */
unsigned int kernelGetMICStatus(void)
{
    return micStatus;
}

/*get SOS status: Cindy*/
unsigned int kernelGetSOSStatus(void)
{
    return  sosbtnstatus;
}

/*get SPK status: Cindy*/
unsigned int kernelGetSPKStatus(void)
{
    return spkstatus;
}

float kernelGetTemp(void)
{
    float voltage;
    float degree;

    voltage = adc_Temperature() * 3.3 * 1000.0 / 4096;
    degree = - (float)(((float)voltage - 1034) * 100 / 558.0);

    return degree;
}

void kernelMcumgrReinit(void)
{
    RTCInitFlag = RTC_INIT_FAIL;
    BootUTCTime = 0;
    SaveCurrenttimeCount = 0;
    BatterychargeInit();
    micCheckCount = 0;
    RTCfeil_initStep = 0;
}

static void mcumgr_init(void)
{
    unsigned int tmpInt;

    mcu_taskid = kernelGetTaskID();

    /* reboot counter */
    tmpInt = cfg_get_rbt();

    tmpInt++;
    cfg_save_rbt(tmpInt);

    kernelRequestTimer(&mcu_timer);
    kernelEnableTimer(mcu_timer, 1000, MCU_MANAGER_REQUEST);
    kernelRequestTimer(&mpu_reset_timer);
    driverInitADC();
    BatterychargeInit();

    if (RTCInit())
    {
        RTCInitFlag = RTC_INIT_FAIL;
        taskPrintf(TASK_LEVEL_ERROR, " RTC INIT FAIL");
        initUTCTime();
        kernelSetSleepUTCTime(CurrentUTCTime);
    }
    else
    {
        RTCInitFlag = RTC_INIT_SUCCESS;
        taskPrintf(TASK_LEVEL_LOG, " RTC INIT SUCCESS");
        initUTCTime();

        #if 0

        if (driverGetWakeUpSrc() & (~(POWERSTATUS_MASK_NCHARGE | POWERSTATUS_MASK_ON |
                                      POWERSTATUS_MASK_ACC | POWERSTATUS_MASK_FCHARGE)))
        {
            kernelSetSleepUTCTime(CurrentUTCTime + 240);
        }
        else
        {
            kernelSetSleepUTCTime(CurrentUTCTime);
        }

        #else
        kernelSetSleepUTCTime(CurrentUTCTime);
        #endif
    }

    /* request timer */
    kernelRequestTimer(&AdcPollingTimerNo);
    kernelEnableTimer(AdcPollingTimerNo, ADC_POLLING_VALUE, ADC_POLLING_REQUEST);
}

unsigned int mcumgr_task(unsigned int mask)
{
    static unsigned int ticks;
    static unsigned int g_sense_cnt;

    /* init of task */
    if (mask & TASK_SYS_EVENT_INIT)
    {
        taskPrintf(TASK_LEVEL_DEBUG, "TASK_SYS_EVENT_INIT!");
        mcumgr_init();
        /* kernelSetTaskLevel( TASK_LEVEL_DEBUG ); */
        return 0;
    }
    else if (mask & TASK_SYS_EVENT_READ)
    {
        taskPrintf(TASK_LEVEL_DEBUG, "TASK_SYS_EVENT_READ!");
        mask &= (~TASK_SYS_EVENT_READ);
        custom_evt_proc(TASK_SYS_EVENT_READ);
    }
    else if (mask & MCU_MANAGER_REQUEST)
    {
        //        taskPrintf( TASK_LEVEL_LOG, "MCU_MANAGER_REQUEST!" );
        //        taskPrintf( TASK_LEVEL_LOG, "Ticks=%d!", OSIF_GetMilliseconds() );

        mask &= (~MCU_MANAGER_REQUEST);

        adc_check();

        call_button_poll();

        ticks += 200;

        if (ticks >= 1000)
        {
            ticks = 0;

            SRS_CRTL();

            BatterychargedischargeStrategy();

            if (RTCInitFlag == RTC_INIT_FAIL)
            {
                if (RTCInit())
                {
                    RTCInitFlag = RTC_INIT_FAIL;
                    taskPrintf(TASK_LEVEL_ERROR, " RTC INIT FAIL");
                    //initUTCTime();

                    if (RTCfeil_initStep == 0)
                    {
                        RTCfeil_initStep = 1;
                        initUTCTime();

                        if (driverGetWakeUpSrc() & (~(POWERSTATUS_MASK_NCHARGE | POWERSTATUS_MASK_ON |
                                                      POWERSTATUS_MASK_ACC | POWERSTATUS_MASK_FCHARGE)))
                        {
                            kernelSetSleepUTCTime(CurrentUTCTime + PM_MIN_RUN_TIME);
                        }
                        else
                        {
                            kernelSetSleepUTCTime(CurrentUTCTime);
                        }
                    }

                }
                else
                {
                    RTCInitFlag = RTC_INIT_SUCCESS;
                    taskPrintf(TASK_LEVEL_LOG, " RTC INIT SUCCESS");
                    initUTCTime();
                    RTCfeil_initStep = 0;

                    if (driverGetWakeUpSrc() & (~(POWERSTATUS_MASK_NCHARGE | POWERSTATUS_MASK_ON |
                                                  POWERSTATUS_MASK_ACC | POWERSTATUS_MASK_FCHARGE)))
                    {
                        kernelSetSleepUTCTime(CurrentUTCTime + PM_MIN_RUN_TIME);
                    }
                    else
                    {
                        kernelSetSleepUTCTime(CurrentUTCTime);
                    }

                    /* if not wakeup by rtc, clear it */
                    if (driverGetWakeUpSrc() & (~POWERSTATUS_MASK_RTC))
                    {
                        cfg_save_standbyflag(STANDBY_NO_SLEEP);
                        cfg_save_wake_cnt(0);
                    }
                }
            }
            else
            {
                BatteryHealthStrategy();
            }

            scanUTCTime();

            mpu_reset_check();

            /* system running mode check */
            RunningModeCheck();

            //acc_check();
        }

        kernelEnableTimer(mcu_timer, 200, MCU_MANAGER_REQUEST);
    }
    else if (mask & MPU_RESET_EVENT)
    {
        mask &= (~MPU_RESET_EVENT);
        mpu_reset_proc();
    }
    else if (mask & ADC_POLLING_REQUEST)
    {
        mask &= (~ADC_POLLING_REQUEST);
        kernelEnableTimer(AdcPollingTimerNo, ADC_POLLING_VALUE, ADC_POLLING_REQUEST);

        acc_check();

        g_sense_cnt++;

        if (g_sense_cnt % 2 == 0)
        {
            ADCPolling();
        }
    }
    /* unknown event */
    else if (mask & TASK_SYS_EVENT_MASK)
    {
        taskPrintf(TASK_LEVEL_ERROR, "Got unknown event %08x!", mask);
        mask = 0;
    }

    return mask;
}

