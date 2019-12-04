#ifndef __SOFTRTC_H__
#define __SOFTRTC_H__

#include <rtc.h>

extern unsigned int basetime;
extern unsigned int base_uptime;
extern unsigned int rtc_isset;
void softrtc_init(uint64_t uptime, RTCTime *time, unsigned int rtc_flag);
int softrtc_fromuptime(uint64_t uptime, RTCTime *time);
int softrtc_frombasetime(unsigned int interval, unsigned int *sec);

static inline unsigned int softrtc_Basetime(void)
{
    return rtc_isset ? basetime : 0xFFFFFFFF;
}

static inline unsigned int softrtc_BaseUptime(void)
{
    return rtc_isset ? base_uptime : 0xFFFFFFFF;
}

#endif

