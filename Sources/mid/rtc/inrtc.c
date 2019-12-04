#include <rtc.h>
#include <rtc_driver.h>
#include <rtc_hal.h>


#define RTC_INSTANCE        (0UL)

rtc_timedate_t tempTime;

/* start RTC */
static void INRTCStart(void)
{
    /*--- Start RTC counters ---*/
    RTC_DRV_StartCounter(RTC_INSTANCE);
    return;
}

/* stop RTC */
static void INRTCStop(void)
{
    /*--- Stop RTC counters ---*/
    RTC_DRV_StopCounter(RTC_INSTANCE);
    return;
}

/* set time */
void driverINRTCSetTime(RTCTime time)
{
    INRTCStop();

    tempTime.seconds = time.RTC_Sec;
    tempTime.minutes = time.RTC_Min;
    tempTime.hour = time.RTC_Hour;
    tempTime.day = time.RTC_Mday;
    tempTime.month = time.RTC_Mon;
    tempTime.year = time.RTC_Year + 2000;

    RTC_DRV_SetTimeDate(RTC_INSTANCE, &tempTime);

    INRTCStart();

    return;
}

/* get time */
void driverINRTCGetTime(RTCTime *localTime)
{
    RTC_DRV_GetCurrentTimeDate(RTC_INSTANCE, &tempTime);

    localTime->RTC_Msec = 0;
    localTime->RTC_Sec = tempTime.seconds;
    localTime->RTC_Min = tempTime.minutes;
    localTime->RTC_Hour = tempTime.hour;
    localTime->RTC_Mday = tempTime.day;
    localTime->RTC_Mon = tempTime.month;
    localTime->RTC_Year = tempTime.year - 2000;
}

int driverGetINRTCFault(void)
{
    return 0;
}

void driverINRTCSetInitTime(RTCTime time)
{
    driverINRTCSetTime(time);
    return;
}

/* init RTC */
void driverINRTCInit(void)
{

    /*! rtcTimer configuration structure */
    rtc_init_config_t rtcTimerCfg =
    {
        /*! Time compensation interval */
        .compensationInterval       =   0U,
        /*! Time compensation value */
        .compensation               =   0,
        /*! RTC Clock Source is 32 KHz crystal */
        .clockSelect                =   RTC_CLK_SRC_OSC_32KHZ,
        /*! RTC Clock Out is disabled */
        .clockOutConfig             =   RTC_CLKOUT_DISABLED,
        /*! Update of the TCE bit is not allowed */
        .updateEnable               =   false,
        /*! Non-supervisor mode write accesses are not supported and generate
         * a bus error.
         */
        .nonSupervisorAccessEnable  =   false
    };

    /* Initialize RTC instance
     *  - See RTC configuration component for options
     */
    RTC_DRV_Init(RTC_INSTANCE, &rtcTimerCfg);

    /* Start the RTC counter */
    RTC_DRV_StartCounter(RTC_INSTANCE);

    return;
}

