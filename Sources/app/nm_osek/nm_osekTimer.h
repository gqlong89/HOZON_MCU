


#ifndef NM_OSEKTIMER_H
#define NM_OSEKTIMER_H

#include <app/can_mgr/can_basic_cfg.h>

#if USED_NM_OSEK


//用于注册，开启，关闭定时器

#define TTyp_MASK                     (1<<0)
#define TMax_MASK                     (1<<1)
#define TError_MASK                   (1<<2)
#define TWBS_MASK                     (1<<3)
#define TTx_MASK                      (1<<4)
#define TSleepReqMin_LimpHome_MASK    (1<<5)
#define TSleepReqMin_Normal_MASK      (1<<6)

#define TappActive_MASK               (1<<7)//应用报文首帧定时器

extern void nm_TimerInit(void);
extern void nm_OsekSetAlarm(unsigned char canport, unsigned char timerMask);
extern void nm_OsekCancleAlarm(unsigned char canport, unsigned char timerMask);


#endif

#endif

