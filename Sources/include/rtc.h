#ifndef _RTC_H_
#define _RTC_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* ------------ RTC ------------- */
/* RTC structure */
typedef struct
{
    unsigned short RTC_Msec; /* Micro second value - [0,999] */
    unsigned char RTC_Sec; /* Second value - [0,59] */
    unsigned char RTC_Min; /* Minute value - [0,59] */
    unsigned char RTC_Hour; /* Hour value - [0,23] */
    unsigned char RTC_Mday; /* Day of the month value - [1,31] */
    unsigned char RTC_Mon; /* Month value - [1,12] */
    unsigned char RTC_Week; /* Month value - [0,6] */
    unsigned short RTC_Year; /* Year value - [0,4095] */
} RTCTime;

/** @brief RTC time type option */
typedef enum
{
    /** Second */
    RTC_TIMETYPE_SECOND = 0,
    /** Month */
    RTC_TIMETYPE_MINUTE = 1,
    /** Hour */
    RTC_TIMETYPE_HOUR = 2,
    /** Day */
    RTC_TIMETYPE_DAY = 3,
    /** Week */
    RTC_TIMETYPE_WEEK = 4,
    /** Month */
    RTC_TIMETYPE_MONTH = 5,
    /** Year */
    RTC_TIMETYPE_YEAR = 6,
} RTC_TIMETYPE_Num;

typedef enum
{
    DISABLE = 0,
    ENABLE = !DISABLE
} FunctionalState;

unsigned int driverExRTCI2CSetWakeIntvl(unsigned int interval);

unsigned int driverExRTCI2CWrite(RTCTime time);

unsigned int driverExRTCI2CRead(RTCTime *timePtr);

unsigned int driverRTCSetWakeIntvl(unsigned int intvl);

void driverRTCGetTime(RTCTime *localtime);

void driverRTCSetTime(RTCTime time);

void driverRTCInit(void);

void driverRTCSetInitTime(RTCTime time);

#endif

