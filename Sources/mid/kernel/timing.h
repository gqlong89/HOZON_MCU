/****************************************************
 *          timing header file                       *
 *                             www.intest.com.cn    *
 ****************************************************/
#ifndef KERLEN_TIMING_H
#define KERLEN_TIMING_H

#define TIMER_MASK_USED                 (1<<0)
#define TIMER_MASK_TIMING               (1<<1)

#define MAX_TIMER_NUMBER                (15)

/* Timer Status */
enum
{
    TASK_TIMER_OK = 0,
    TASK_TIMER_NOT_USED,
    TASK_TIMER_INVALID,
    TASK_TIMER_FULL,
    TASK_TIMER_TIMING,
    TASK_TIMER_STOPED
};

/* timer structure */
typedef struct
{
    unsigned int timerEvent; /* event when time is up */
    uint64_t timerUptime; /* when it's out */
    unsigned char timerMask; /* mask */
    unsigned char timerTaskID; /* belong to which task */
} timerStruct;

void kernelInitTimer(void);
unsigned char kernelRequestTimer(unsigned char *timerNoPtr);
unsigned char kernelEnableTimer(unsigned char timerNo, unsigned int interval, unsigned int event);
unsigned char kernelDisableTimer(unsigned char timerNo);
unsigned char kernelReleaseTimer(unsigned char timerNo);
void kernelScanTimer(uint64_t uptime);
unsigned char kernelTimerIsEnable(unsigned char timerNo);

int kernelShowTimer(unsigned int argc, unsigned char **argv);

#endif

