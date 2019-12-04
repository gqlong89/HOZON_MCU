#include <osif.h>
#include <rtc.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>


uint32_t basetime = 0;
uint32_t base_uptime = 0;
unsigned int rtc_isset = 0;     // 0: hard rtc is not set, 1:hard rtc is set

void softrtc_init(uint64_t uptime, RTCTime *time, unsigned int rtc_flag)
{
    if (time)
    {
        struct tm localtm;
        memset(&localtm, 0, sizeof(localtm));

        /* localtm.tm_year is begin from 1900, 0 indicates 1900
           localtm.tm_mon is begin from 0, 0 indicates January
           time->RTC_Year  is begin from 2000, 0 indicates 2000
           time->RTC_Mon  is begin from 1, 1 indicates January
        */

        localtm.tm_year  = time->RTC_Year + 100;
        localtm.tm_mon   = time->RTC_Mon - 1;
        localtm.tm_mday  = time->RTC_Mday;
        localtm.tm_hour  = time->RTC_Hour;
        localtm.tm_min   = time->RTC_Min;
        localtm.tm_sec   = time->RTC_Sec;
        localtm.tm_wday  = 0;
        localtm.tm_yday  = 0;
        localtm.tm_isdst = 0;

        basetime = mktime(&localtm) - (uint32_t)(uptime / 100);
        base_uptime = (uint32_t)uptime;
        rtc_isset = rtc_flag;
    }
}

int softrtc_fromuptime(uint64_t uptime, RTCTime *time)
{
    struct tm *localtm;
    time_t timedata;

    if (!basetime)
    {
        /* 2000.1.1 0:0:0 */
        time->RTC_Year = 0;
        time->RTC_Mon  = 1;
        time->RTC_Mday = 1;
        time->RTC_Hour = 0;
        time->RTC_Min  = 0;
        time->RTC_Sec  = uptime / 100;
        return -1;
    }

    timedata = basetime + (uint32_t)(uptime / 100);
    localtm  = localtime(&timedata);

    time->RTC_Year = localtm->tm_year - 100;
    time->RTC_Mon  = localtm->tm_mon + 1;
    time->RTC_Mday = localtm->tm_mday;
    time->RTC_Hour = localtm->tm_hour;
    time->RTC_Min  = localtm->tm_min;
    time->RTC_Sec  = localtm->tm_sec;
    time->RTC_Msec = (uptime % 100) * 10;

    return 0;
}

/* the unit of interval is second */
int softrtc_frombasetime(unsigned int interval, unsigned int *sec)
{
    if (!basetime)
    {
        return -1;
    }

    *sec = (basetime + (uint32_t)(driverGetUptime() / 100) + interval);

    return 0 ;
}


