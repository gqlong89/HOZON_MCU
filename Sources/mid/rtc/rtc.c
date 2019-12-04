#include <string.h>
#include "i2c.h"
#include "rtc.h"
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include "lpi2c1.h"
#include "softrtc.h"

#define PCF85163_SLAVE_ADDR 0x51

/*******************************************************************************
 * Code
 ******************************************************************************/
static unsigned int bcd2bin(uint8_t val)
{
    return ((val) & 0x0f) + ((val) >> 4) * 10;
}

static uint8_t bin2bcd(unsigned int val)
{
    return (((val / 10) << 4) | (val % 10));
}

unsigned int driverExRTCI2CWriteReg(unsigned char addr, unsigned char val)
{
    driverI2CWrite(PCF85163_SLAVE_ADDR, addr, &val, 1);
    return 0;
}

unsigned int driverExRTCI2CReadReg(unsigned char addr, unsigned char *valPtr)
{
    if (driverI2CRead(PCF85163_SLAVE_ADDR, addr, valPtr, 1))
    {
        return 1; // error
    }
    else
    {
        return 0; // ok
    }
}

/* I2C write */
unsigned int driverExRTCI2CWrite(RTCTime time)
{
    unsigned char century;

    time.RTC_Year = time.RTC_Year + 2000;
    driverExRTCI2CWriteReg(0x08, bin2bcd(time.RTC_Year % 100));

    century = (time.RTC_Year >= 2000) ? 0 : 0x80;
    driverExRTCI2CWriteReg(0x07, bin2bcd(time.RTC_Mon) | century);

    driverExRTCI2CWriteReg(0x05, bin2bcd(time.RTC_Mday));
    driverExRTCI2CWriteReg(0x04, bin2bcd(time.RTC_Hour));
    driverExRTCI2CWriteReg(0x03, bin2bcd(time.RTC_Min));
    driverExRTCI2CWriteReg(0x02, bin2bcd(time.RTC_Sec));

    return 0;
}

unsigned int driverExRTCI2CRead(RTCTime *timePtr)
{
    unsigned char sec = 0, min = 0, hour = 0, mday = 0, mon_cent = 0, year = 0;
    unsigned int ret = 0;

    ret |= driverExRTCI2CReadReg(0x08, &year);
    ret |= driverExRTCI2CReadReg(0x07, &mon_cent);
    ret |= driverExRTCI2CReadReg(0x05, &mday);
    ret |= driverExRTCI2CReadReg(0x04, &hour);
    ret |= driverExRTCI2CReadReg(0x03, &min);
    ret |= driverExRTCI2CReadReg(0x02, &sec);

    timePtr->RTC_Msec = 0;
    timePtr->RTC_Sec = bcd2bin(sec & 0x7F);
    timePtr->RTC_Min = bcd2bin(min & 0x7F);
    timePtr->RTC_Hour = bcd2bin(hour & 0x3F);
    timePtr->RTC_Mday = bcd2bin(mday & 0x3F);
    timePtr->RTC_Mon = bcd2bin(mon_cent & 0x1F);
    //timePtr->RTC_Year = bcd2bin (year) + ((mon_cent & 0x80) ? 1900 : 2000);
    timePtr->RTC_Year = bcd2bin(year);

    if (ret == 1)
    {
        ret = 0x02;
    }

    ret |= (sec & (1 << 7)) ? 1 : 0;

    return ret;

}

unsigned int driverExRTCI2CAlarmRead(RTCTime *timePtr)
{
    unsigned char min = 0, hour = 0, mday = 0;

    driverExRTCI2CReadReg(0x0B, &mday);
    driverExRTCI2CReadReg(0x0A, &hour);
    driverExRTCI2CReadReg(0x09, &min);

    timePtr->RTC_Min = bcd2bin(min & 0x7F);
    timePtr->RTC_Hour = bcd2bin(hour & 0x3F);
    timePtr->RTC_Mday = bcd2bin(mday & 0x3F);

    return 0;

}

void RTC_AlarmIntConfig(uint32_t AlarmTimeType, uint32_t ALValue, FunctionalState NewState)
{
    if (NewState == ENABLE)
    {
        switch (AlarmTimeType)
        {
            case RTC_TIMETYPE_MINUTE:
                driverExRTCI2CWriteReg(0x09, bin2bcd(ALValue) & 0x7F);
                break;

            case RTC_TIMETYPE_HOUR:
                driverExRTCI2CWriteReg(0x0A, bin2bcd(ALValue) & 0x7F);
                break;

            case RTC_TIMETYPE_DAY:
                driverExRTCI2CWriteReg(0x0B, bin2bcd(ALValue) & 0x7F);
                break;

            case RTC_TIMETYPE_WEEK:
                driverExRTCI2CWriteReg(0x0C, bin2bcd(ALValue) & 0x7F);
                break;

            default:
                break;
        }
    }
    else
    {
        switch (AlarmTimeType)
        {
            case RTC_TIMETYPE_MINUTE:
                driverExRTCI2CWriteReg(0x09, bin2bcd(ALValue) | 0x80);
                break;

            case RTC_TIMETYPE_HOUR:
                driverExRTCI2CWriteReg(0x0A, bin2bcd(ALValue) | 0x80);
                break;

            case RTC_TIMETYPE_DAY:
                driverExRTCI2CWriteReg(0x0B, bin2bcd(ALValue) | 0x80);
                break;

            case RTC_TIMETYPE_WEEK:
                driverExRTCI2CWriteReg(0x0C, bin2bcd(ALValue) | 0x80);
                break;

            default:
                break;
        }
    }
}

static unsigned char daysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static RTCTime RTC_GetNextTime(unsigned int min)
{
    RTCTime rtcTime;
    unsigned int integralPart;
    unsigned char mdays;

    driverExRTCI2CRead(&rtcTime);

    do
    {
        // min
        integralPart = (rtcTime.RTC_Min + min) / 60;

        if (integralPart == 0)
        {
            rtcTime.RTC_Min = rtcTime.RTC_Min + min;
            break;
        }
        else if (integralPart >= 1)
        {
            rtcTime.RTC_Min = (rtcTime.RTC_Min + min) % 60;
            rtcTime.RTC_Hour += integralPart;
        }

        // hour
        integralPart = (rtcTime.RTC_Hour) / 24;

        if (integralPart == 0)
        {
            break;
        }
        else if (integralPart >= 1)
        {
            rtcTime.RTC_Hour = rtcTime.RTC_Hour % 24;
            rtcTime.RTC_Mday += integralPart;
        }

        // day
        mdays = daysOfMonth[rtcTime.RTC_Mon - 1];

        if ((rtcTime.RTC_Year % 4 == 0) && (rtcTime.RTC_Mon == 2))
        {
            mdays++;
        }

        integralPart = (rtcTime.RTC_Mday - 1) / mdays;

        if (integralPart == 0)
        {
            break;
        }
        else if (integralPart >= 1)
        {
            rtcTime.RTC_Mday = (rtcTime.RTC_Mday - 1) % mdays + 1;
            rtcTime.RTC_Mon += integralPart;
        }

        // month
        integralPart = (rtcTime.RTC_Mon - 1) / 12;

        if (integralPart == 0)
        {
            break;
        }
        else if (integralPart >= 1)
        {
            rtcTime.RTC_Mon = (rtcTime.RTC_Mon - 1) % 12 + 1;
            rtcTime.RTC_Year += integralPart;
        }

        break;
    }
    while (0);

    //修正打印信息 Modified by Cindy
    printk(PRINTK_LEVEL_LOG, "alarm time: %02d/%02d/%02d %02d:%02d:%02d",
           rtcTime.RTC_Year + 2000, rtcTime.RTC_Mon, rtcTime.RTC_Mday,
           rtcTime.RTC_Hour, rtcTime.RTC_Min, rtcTime.RTC_Sec);

    return rtcTime;
}

unsigned int driverExRTCI2CSetWakeIntvl(unsigned int interval)
{
    RTCTime rtcTime;

    if (interval == 0)
    {
        // cancel RTC wake
        RTC_AlarmIntConfig(RTC_TIMETYPE_MINUTE, rtcTime.RTC_Min, DISABLE);
        RTC_AlarmIntConfig(RTC_TIMETYPE_HOUR, rtcTime.RTC_Hour, DISABLE);
        RTC_AlarmIntConfig(RTC_TIMETYPE_DAY, rtcTime.RTC_Mday, DISABLE);
        RTC_AlarmIntConfig(RTC_TIMETYPE_WEEK, rtcTime.RTC_Week, DISABLE);
        driverExRTCI2CWriteReg(0x01, 0x00);
        return 0;
    }

    rtcTime = RTC_GetNextTime(interval);

    RTC_AlarmIntConfig(RTC_TIMETYPE_MINUTE, rtcTime.RTC_Min, ENABLE);
    RTC_AlarmIntConfig(RTC_TIMETYPE_HOUR, rtcTime.RTC_Hour, ENABLE);
    RTC_AlarmIntConfig(RTC_TIMETYPE_DAY, rtcTime.RTC_Mday, ENABLE);
    RTC_AlarmIntConfig(RTC_TIMETYPE_WEEK, rtcTime.RTC_Week, DISABLE);
    driverExRTCI2CWriteReg(0x01, 0x02);

    return 0;
}

unsigned int driverRTCSetWakeIntvl(unsigned int interval)
{
    driverExRTCI2CSetWakeIntvl(interval);
    return 0;
}

void driverRTCGetTime(RTCTime *localtime)
{
    if (0 != softrtc_fromuptime(driverGetUptime(), localtime))
    {
        printk(PRINTK_LEVEL_ERROR, "softrtc not init: %d/%d/%d %d:%d:%d\r\n",
               localtime->RTC_Year + 2000, localtime->RTC_Mon, localtime->RTC_Mday,
               localtime->RTC_Hour, localtime->RTC_Min, localtime->RTC_Sec);
    }
}

void driverRTCSetTime(RTCTime time)
{
    printk(PRINTK_LEVEL_LOG, "Set Hard RTC: %d/%d/%d %d:%d:%d\r\n",
           time.RTC_Year, time.RTC_Mon, time.RTC_Mday,
           time.RTC_Hour, time.RTC_Min, time.RTC_Sec);
    driverExRTCI2CWrite(time);

    if (time.RTC_Year > 100)
    {
        time.RTC_Year -= 2000;
    }

    printk(PRINTK_LEVEL_LOG, "Set Soft RTC: %d/%d/%d %d:%d:%d\r\n",
           time.RTC_Year + 2000, time.RTC_Mon, time.RTC_Mday,
           time.RTC_Hour, time.RTC_Min, time.RTC_Sec);
    softrtc_init(driverGetUptime(), &time, 1);
}

/* start RTC */
static void RTCStart(void)
{
    /*--- Start RTC counters ---*/
    driverExRTCI2CWriteReg(0x00, 0x00);
    driverExRTCI2CWriteReg(0x01, 0x00);
    driverExRTCI2CWriteReg(0x0D, 0x00);
    return;
}

/* stop RTC */
static void RTCStop(void)
{
    /*--- Stop RTC counters ---*/
    driverExRTCI2CWriteReg(0x00, 0x20);
    return;
}

void driverRTCSetInitTime(RTCTime time)
{
    unsigned char century;

    RTCStop();

    time.RTC_Year = time.RTC_Year + 2000;
    driverExRTCI2CWriteReg(0x08, bin2bcd(time.RTC_Year % 100));

    century = (time.RTC_Year >= 2000) ? 0 : 0x80;
    driverExRTCI2CWriteReg(0x07, bin2bcd(time.RTC_Mon) | century);

    driverExRTCI2CWriteReg(0x05, bin2bcd(time.RTC_Mday));
    driverExRTCI2CWriteReg(0x04, bin2bcd(time.RTC_Hour));
    driverExRTCI2CWriteReg(0x03, bin2bcd(time.RTC_Min));

    RTCStart();

    return;
}

void driverRTCInit(void)
{
    driverI2CInit();
    RTCStart();

    return;
}

