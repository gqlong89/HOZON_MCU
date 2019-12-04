/****************************************************
 *          Power function file                     *
 *                             www.intest.com.cn    *
 ****************************************************/

#include <app/battery/battery.h>
#include <app/can_mgr/can_mgr.h>
#include <app/power/pm_mode.h>
#include <app/power/power.h>
#include <app/scom/scom_tl_ex.h>
#include <call.h>
#include <config.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <osif.h>
#include <pm.h>
#include <rtc.h>
#include <stdbool.h>
#include <sys/_stdint.h>
#include <mcu_fun_def.h>

static unsigned int pm_run_mode = PM_RUNNING_MODE;
static unsigned int pm_sys_mode = PM_RUNNING_ONLY;
static unsigned int pm_rtc_wakeup_val = MCU_RTC_WAKEUP_IN_LP_INTERVAL;

void pm_set_wakeup_val(unsigned int interval)
{
    pm_rtc_wakeup_val = interval;
}

unsigned int pm_get_wakeup_val(void)
{
    return pm_rtc_wakeup_val;
}

unsigned int pm_get_run_mode(void)
{
    return pm_run_mode;
}

unsigned int get_anti_thief_trigger(void)
{
    unsigned int ret = 0;

    /*  anti thief function is TBD */
    #if 0

    if (cfg_get_anti_thief())
    {
        /* if tbox is wakeup by g_sensor */
        if (driverGetWakeUpSrc() & POWERSTATUS_MASK_G_SENSOR)
        {
            ret = 1;
        }

        /* if tbox is wakeup by KL30 disconnection */
        if (driverGetWakeUpSrc() & POWERSTATUS_MASK_PM)
        {
            ret = 1;
        }

        /* if tbox is wakeup by RF disconnection */
        if (driverGetWakeUpSrc() & POWERSTATUS_MASK_RF)
        {
            ret = 1;
        }

        /* no kl15 signal and kl75 signal, and the kl30 is disconnected  */
        if ((!pm_get_on_wakeup()) && (!pm_get_acc_wakeup())
            && (get_powervoltage() < PM_KL30_MIN_POW_VOL))
        {
            ret = 1;
        }

        /* no kl15 signal and kl75 signal, and the gensor interrupt occurs   */
    }

    #endif

    return ret;
}

unsigned int get_standby_timeout(void)
{
    unsigned int ret = 0;

    if (driverGetWakeUpSrc() & POWERSTATUS_MASK_RTC)
    {
        ret = 1;
    }

    return ret;
}

unsigned int get_can_status(void)
{
    return (can_sta_get(0) | can_sta_get(1) | can_sta_get(2));
}

unsigned int pm_sleep_available(void)
{
    if (scom_get_upg_status())
    {
        taskPrintf(TASK_LEVEL_DEBUG, "upgrade status:%u", scom_get_upg_status());
        return 0;
    }

    /*  防盗跟踪功能打开并且防盗功能已报警 */
    if (get_anti_thief_trigger())
    {
        taskPrintf(TASK_LEVEL_DEBUG, "anti thief status:%u", get_anti_thief_trigger());
        return 0;
    }

    /*  USB有供电 */
    if (pm_get_usb_status())
    {
        taskPrintf(TASK_LEVEL_DEBUG, "usb status:%u", pm_get_usb_status());
        return 0;
    }

    /*  CAN1,CAN2,CAN3任意一路CAN有CAN报文 */
    if (get_can_status())
    {
        taskPrintf(TASK_LEVEL_DEBUG, "can status:%u", get_can_status());
        return 0;
    }

    /*  ECALL/BCALL按键被按下或者当前有 ECALL/BCALL通话 */
    if (call_get_status())
    {
        taskPrintf(TASK_LEVEL_DEBUG, "call status:%u", call_get_status());
        return 0;
    }

    /*  有硬线唤醒信号 */
    if (driverGetHardLineWakeSignals())
    {
        taskPrintf(TASK_LEVEL_DEBUG, "hard line status:%u", driverGetHardLineWakeSignals());
        return 0;
    }

    /*  有RING唤醒信号 */
    if (pm_get_ring_wakeup())
    {
        taskPrintf(TASK_LEVEL_DEBUG, "ring status:%u", pm_get_ring_wakeup());
        return 0;
    }

    /* KL30 is disconnected, TBOX will enter emergency mode */
    if ((!BatteryInsertDetect()) && (get_powervoltage() < PM_KL30_MIN_POW_VOL))
    {
        taskPrintf(TASK_LEVEL_DEBUG, "bat status:%u, vol:%u", BatteryInsertDetect(), get_powervoltage());
        return 0;
    }

    /* RTC 唤醒时间在5分钟内 */
    //Deleted by Cindy
    /*
    if( (scom_get_wake_time() >= 0) && (scom_get_wake_time() <= 5) )
    {
        taskPrintf(TASK_LEVEL_DEBUG, "wake time:%u", scom_get_wake_time());
        return 0;
    }
    */

    /* 升级过程中不能休眠，待修改 */

    /* 虚拟ON线不休眠 */
    if (pm_get_virtual_on_status())
    {
        return 0;
    }

    return 1;
}

unsigned int pm_off_available(void)
{
    /* KL30 未断开 */
    if (get_powervoltage() > PM_KL30_MIN_POW_VOL)
    {
        taskPrintf(TASK_LEVEL_DEBUG, "power:%u", get_powervoltage());
        return 0;
    }

    /* 当前满足以下任意一个条件
       1、有活动的通话；
       2、ECall功能不支持或者等待ECall触发定时器超时；
       3、BCall功能不支持或者等待BCall触发定时器超时；
    */

    if (call_get_trigger_timeout())
    {
        taskPrintf(TASK_LEVEL_DEBUG, "call:%u", call_get_trigger_timeout());
        return 0;
    }
	#if 0
    /* 防盗功能打开 */
    if (cfg_get_anti_thief())
    {
        taskPrintf(TASK_LEVEL_DEBUG, "VTD:%u", cfg_get_anti_thief());
        return 0;
    }
	#endif
    return 1;
}

void RunningModeCheck(void)
{
    static unsigned int ticks           = 0;
    static uint64_t pm_4g_on_time       = 0;
    static unsigned int pm_sys_mode_bak = 0;
    static unsigned int run_last_ticks  = 0;
    int wake_time;

    pm_sys_mode = cfg_get_sysmode();

    switch (pm_run_mode)
    {
        case PM_RUNNING_MODE:
            pm_sys_mode_bak = pm_sys_mode;
            ticks = 0;

            if (pm_get_4g_status())
            {
                taskPrintf(TASK_LEVEL_DEBUG, "wakeup 4G");
                /* wakeup mcu */
                pm_wakeup_ec20_ctrl(true);
            }
            else
            {
                if (driverGetUptime() > (pm_4g_on_time + 30 * 100))
                {
                    /* power on 4G */
                    taskPrintf(TASK_LEVEL_DEBUG, "power on 4G");
                    pm_4g_safe_on();
                    pm_4g_on_time = driverGetUptime();
                }
            }

            if ((!BatteryInsertDetect()) && (get_powervoltage() < PM_KL30_MIN_POW_VOL))
            {
                pm_run_mode = PM_EMERGENCY_MODE;
                ticks = 0;
                call_set_tigger_timeout();
                taskPrintf(TASK_LEVEL_LOG, "switch to emergency mode! powervoltage: %d", (int)get_powervoltage());
                break;
            }

            if (!pm_sleep_available())
            {
                cfg_save_standbyflag(STANDBY_NO_SLEEP);
                cfg_save_wake_cnt(0);
            }

            if ((pm_sys_mode != PM_RUNNING_ONLY) && (kernelGetSleepUTCTime() > kernelGetCurrentUTCTime()))
            {
                //Modified by Cindy
                if (pm_sleep_available())
                {
                    if (kernelGetSleepUTCTime() - kernelGetCurrentUTCTime() <= PM_MIN_RUN_TIME)
                    {
                        taskPrintf(TASK_LEVEL_LOG, "after %d sec into other mode",
                                   kernelGetSleepUTCTime() - kernelGetCurrentUTCTime());
                    }
                }
                else
                {
                    run_last_ticks = 0;
                }

                break;
            }

            if (pm_sys_mode == PM_RUNNING_ONLY)
            {
                break;
            }
            else if (pm_sys_mode == PM_LISTEN_ONLY)
            {
                if (scom_is_need_upgrade())
                {
                    taskPrintf(TASK_LEVEL_LOG, "wait mcu to check version");
                    run_last_ticks = 0;
                }
                else if (pm_sleep_available())
                {
                    run_last_ticks++;

                    if (run_last_ticks > PM_RUN_LAST_TICK_NUM)
                    {
                        pm_wakeup_ec20_ctrl(false);
                        pm_run_mode = PM_LISTEN_MODE;
                        ticks = 0;
                        run_last_ticks = 0;
                        taskPrintf(TASK_LEVEL_LOG, "Switch to listen mode!");
                    }
                }
                else
                {
                    set_sleep_time_in_running(); //added by Cindy 20180329
                    run_last_ticks = 0;
                }
            }
            else if (pm_sys_mode == PM_SLEEP_ONLY)
            {
                if (scom_is_need_upgrade())
                {
                    taskPrintf(TASK_LEVEL_LOG, "wait mcu to check version");
                    run_last_ticks = 0;
                }
                else if (pm_sleep_available())
                {
                    run_last_ticks++;

                    if (run_last_ticks > PM_RUN_LAST_TICK_NUM)
                    {
                        pm_wakeup_ec20_ctrl(false);
                        pm_run_mode = PM_SLEEP_MODE;
                        ticks = 0;
                        run_last_ticks = 0;
                        taskPrintf(TASK_LEVEL_LOG, "Switch to sleep mode!");
                    }
                }
                else
                {
                    run_last_ticks = 0;
                }
            }
            else if (pm_sys_mode == PM_AUTO)
            {
                if (scom_is_need_upgrade())
                {
                    taskPrintf(TASK_LEVEL_LOG, "wait mcu to check version");
                    run_last_ticks = 0;
                }
                else if (pm_sleep_available())
                {
                    run_last_ticks++;

                    if (run_last_ticks > PM_RUN_LAST_TICK_NUM)
                    {
                        if (get_standby_timeout() && (cfg_get_standbyflag() == STANDBY_SLEEP))
                        {
                            pm_wakeup_ec20_ctrl(false);
                            pm_run_mode = PM_SLEEP_MODE;
                            ticks = 0;
                            taskPrintf(TASK_LEVEL_LOG, "Switch to sleep mode!");
                        }
                        else if (get_standby_timeout() && (cfg_get_standbyflag() == STANDBY_DEEP_SLEEP))
                        {
                            pm_wakeup_ec20_ctrl(false);
                            pm_run_mode = PM_DEEP_SLEEP_MODE;
                            ticks = 0;
                            taskPrintf(TASK_LEVEL_LOG, "Switch to deep sleep mode!");
                        }
                        else
                        {
                            pm_wakeup_ec20_ctrl(false);
                            pm_run_mode = PM_LISTEN_MODE;
                            ticks = 0;
                            taskPrintf(TASK_LEVEL_LOG, "Switch to listen mode!");
                        }

                        run_last_ticks = 0;
                    }
                }
                else
                {
                    run_last_ticks = 0;
                }
            }

            break;

        case PM_EMERGENCY_MODE:

            ticks++;

            taskPrintf(TASK_LEVEL_LOG, "emergency mode ticks = %d", ticks);

            if (pm_get_4g_status())
            {
                /* wakeup mcu */
                pm_wakeup_ec20_ctrl(true);
            }
            else
            {
                /* power on 4G */
                pm_4g_safe_on();
            }

            /* if KL30 is connected */
            if (get_powervoltage() > PM_KL30_MIN_POW_VOL)
            {
                pm_run_mode = PM_RUNNING_MODE;
                taskPrintf(TASK_LEVEL_LOG, "Switch to running mode!");
            }
            else
            {
                if ((scom_is_app_can_sleep()) || ((!scom_is_app_can_sleep())
                                                  && (ticks > PM_MAX_EMERGENCY_RUN_TIME)))
                {
                    if (pm_off_available() && (ticks > 64) && !scom_is_need_upgrade())
                    {
                        pm_wakeup_ec20_ctrl(false);
                        pm_run_mode = PM_OFF_MODE;
                        ticks = 0;
                        taskPrintf(TASK_LEVEL_LOG, "Switch to off mode!");
                    }
                }
            }

            break;

        case PM_LISTEN_MODE:

            if ((!BatteryInsertDetect()) && (get_powervoltage() < PM_KL30_MIN_POW_VOL))
            {
                pm_run_mode = PM_EMERGENCY_MODE;
                ticks = 0;
                call_set_tigger_timeout();
                taskPrintf(TASK_LEVEL_LOG, "Switch to emergency mode! powervoltage: %d", (int)get_powervoltage());
                break;
            }

            if (pm_get_4g_status())
            {
                /* wakeup mcu */
                pm_wakeup_ec20_ctrl(false);
            }
            else
            {
                /* power on 4G */
                pm_4g_safe_on();
            }

            ticks += 1;

            taskPrintf(TASK_LEVEL_LOG, "listen mode ticks = %d", ticks);

            if ((!pm_sleep_available()) || (pm_sys_mode_bak != pm_sys_mode))
            {
                if (driverGetHardLineWakeSignals())
                {
                    ticks = 0;
                }
                else
                {
                    kernelSetSleepUTCTime(kernelGetCurrentUTCTime() + PM_MIN_RUN_TIME);
                }

                cfg_save_standbyflag(STANDBY_NO_SLEEP);
                cfg_save_wake_cnt(0);

                pm_run_mode = PM_RUNNING_MODE;
                taskPrintf(TASK_LEVEL_LOG,
                           "Switch to running mode! power: %d, canstatus: %d, app_status: %d, callstatus: %d, usbstatus: %d, anti_thief: %d, sysmode: %d ",
                           driverGetHardLineWakeSignals(), get_can_status(), scom_is_app_can_sleep(), call_get_status(),
                           pm_get_usb_status(), get_anti_thief_trigger(), pm_sys_mode_bak);
                driverRTCSetWakeIntvl(0);
            }
            else if (ticks > 64)
            {
                wake_time = scom_get_wake_time();

                if (wake_time > 0)
                {
                    driverRTCSetWakeIntvl(wake_time);
                    cfg_save_standbyflag(STANDBY_NO_SLEEP);
                    cfg_save_wake_cnt(0);
                    taskPrintf(TASK_LEVEL_LOG, "set rtc wake time = (%d)", wake_time);
                }
                else
                {
                    if (pm_sys_mode == PM_AUTO)
                    {
                        unsigned int interval;

                        #ifndef MCU_RTC_WAKEUP_IN_LP_ENABLE
                        interval = cfg_get_standbysleep();
                        driverRTCSetWakeIntvl(interval);
                        cfg_save_standbyflag(STANDBY_SLEEP);
                        #else
                        interval = pm_rtc_wakeup_val;
                        driverRTCSetWakeIntvl(interval);
                        cfg_save_wake_cnt(cfg_get_wake_cnt() + 1);

                        if ((cfg_get_wake_cnt()*pm_rtc_wakeup_val) >= cfg_get_standbysleep())
                        {
                            cfg_save_standbyflag(STANDBY_SLEEP);
                        }

                        #endif
                        taskPrintf(TASK_LEVEL_LOG, "set rtc sleep wake time = (%d), wake cnt = (%u)", interval,
                                   cfg_get_wake_cnt());
                    }
                    else
                    {
                        driverRTCSetWakeIntvl(0);
                        cfg_save_standbyflag(STANDBY_NO_SLEEP);
                        cfg_save_wake_cnt(0);
                        taskPrintf(TASK_LEVEL_LOG, "disable rtc wake");
                    }
                }

                pm_run_mode = PM_RUNNING_MODE;
                PowerListen();
            }

            break;

        case PM_SLEEP_MODE:

            if ((!BatteryInsertDetect()) && (get_powervoltage() < PM_KL30_MIN_POW_VOL))
            {
                pm_run_mode = PM_EMERGENCY_MODE;
                ticks = 0;
                call_set_tigger_timeout();
                taskPrintf(TASK_LEVEL_LOG, "Switch to emergency mode! powervoltage: %d", (int)get_powervoltage());
                break;
            }

            pm_wakeup_ec20_ctrl(false);
            ticks += 1;

            taskPrintf(TASK_LEVEL_LOG, "sleep mode ticks = %d", ticks);

            if ((!pm_sleep_available()) || (pm_sys_mode_bak != pm_sys_mode))
            {
                if (driverGetHardLineWakeSignals())
                {
                    ticks = 0;
                }
                else
                {
                    kernelSetSleepUTCTime(kernelGetCurrentUTCTime() + PM_MIN_RUN_TIME);
                }

                cfg_save_standbyflag(STANDBY_NO_SLEEP);
                cfg_save_wake_cnt(0);

                pm_run_mode = PM_RUNNING_MODE;
                taskPrintf(TASK_LEVEL_LOG,
                           "Switch to running mode! power: %d, canstatus: %d, app_status: %d, callstatus: %d, usbstatus: %d, anti_thief: %d, sysmode: %d ",
                           driverGetHardLineWakeSignals(), get_can_status(), scom_is_app_can_sleep(), call_get_status(),
                           pm_get_usb_status(), get_anti_thief_trigger(), pm_sys_mode_bak);
                driverRTCSetWakeIntvl(0);
            }
            else if (ticks > 64)
            {
                wake_time = scom_get_wake_time();

                if (wake_time > 0)
                {
                    driverRTCSetWakeIntvl(wake_time);
                    cfg_save_standbyflag(STANDBY_NO_SLEEP);
                    cfg_save_wake_cnt(0);
                    taskPrintf(TASK_LEVEL_LOG, "set rtc wake time = (%d)", wake_time);
                }
                else
                {
                    if (pm_sys_mode == PM_AUTO)
                    {
                        unsigned int interval;

                        #ifndef MCU_RTC_WAKEUP_IN_LP_ENABLE
                        interval = cfg_get_deep_standbysleep();
                        driverRTCSetWakeIntvl(interval);
                        cfg_save_standbyflag(STANDBY_DEEP_SLEEP);
                        #else
                        interval = pm_rtc_wakeup_val;
                        driverRTCSetWakeIntvl(interval);
                        cfg_save_wake_cnt(cfg_get_wake_cnt() + 1);

                        if ((cfg_get_wake_cnt()*pm_rtc_wakeup_val) >= cfg_get_standbysleep() + cfg_get_deep_standbysleep())
                        {
                            cfg_save_standbyflag(STANDBY_DEEP_SLEEP);
                        }

                        #endif

                        taskPrintf(TASK_LEVEL_LOG, "set rtc deep wake time = (%u), wake cnt = (%u)",
                                   interval, cfg_get_wake_cnt());
                    }
                    else
                    {
                        driverRTCSetWakeIntvl(0);
                        cfg_save_standbyflag(STANDBY_NO_SLEEP);
                        cfg_save_wake_cnt(0);
                        taskPrintf(TASK_LEVEL_LOG, "disable rtc wake");
                    }
                }

                pm_run_mode = PM_RUNNING_MODE;
                PowerSleep();
            }

            break;

        case PM_DEEP_SLEEP_MODE:

            if ((!BatteryInsertDetect()) && (get_powervoltage() < PM_KL30_MIN_POW_VOL))
            {
                pm_run_mode = PM_EMERGENCY_MODE;
                ticks = 0;
                call_set_tigger_timeout();
                taskPrintf(TASK_LEVEL_LOG, "Switch to emergency mode! powervoltage: %d", (int)get_powervoltage());
                break;
            }

            pm_wakeup_ec20_ctrl(false);
            ticks += 1;

            taskPrintf(TASK_LEVEL_LOG, "deep sleep mode ticks = %d", ticks);

            if ((!pm_sleep_available()) || (pm_sys_mode_bak != pm_sys_mode))
            {
                if (driverGetHardLineWakeSignals())
                {
                    ticks = 0;
                }
                else
                {
                    kernelSetSleepUTCTime(kernelGetCurrentUTCTime() + PM_MIN_RUN_TIME);
                }

                cfg_save_standbyflag(STANDBY_NO_SLEEP);
                cfg_save_wake_cnt(0);

                pm_run_mode = PM_RUNNING_MODE;
                taskPrintf(TASK_LEVEL_LOG,
                           "Switch to running mode! power: %d, canstatus: %d, app_status: %d, callstatus: %d, usbstatus: %d, anti_thief: %d, sysmode: %d ",
                           driverGetHardLineWakeSignals(), get_can_status(), scom_is_app_can_sleep(), call_get_status(),
                           pm_get_usb_status(), get_anti_thief_trigger(), pm_sys_mode_bak);
                driverRTCSetWakeIntvl(0);
            }
            else if (ticks > 64)
            {
                pm_run_mode = PM_RUNNING_MODE;
                pm_off_mode();
            }

            break;

        case PM_OFF_MODE:

            pm_wakeup_ec20_ctrl(false);
            ticks += 1;      /* wait 4G power off */

            taskPrintf(TASK_LEVEL_LOG, "off mode ticks = %d", ticks);

            if ((get_powervoltage() > PM_KL30_MIN_POW_VOL))
            {
                if (ticks >= 30)
                {
                    pm_4g_safe_on();
                    pm_run_mode = PM_RUNNING_MODE;
                    taskPrintf(TASK_LEVEL_LOG, "Switch to running mode!");
                }
            }
            else if (ticks > 64)
            {
                pm_run_mode = PM_RUNNING_MODE;
                pm_off_mode();
            }

            break;

        default:
            break;
    }
}


