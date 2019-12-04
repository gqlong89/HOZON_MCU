#ifndef _PM_MODE_H_
#define _PM_MODE_H_

#define PM_MIN_RUN_TIME           (240)     /* 240 s*/
#define PM_MAX_EMERGENCY_RUN_TIME (300)     /* 300 s*/
#define PM_RUN_LAST_TICK_NUM      (5)       /* 240 s*/
#define PM_MIN_INPUT_WAKEUP       (120)     /*Cindy <= 120, 运行120+240 后进入休眠; > 120, 先休眠再唤醒然后再休眠*/


enum
{
    PM_RUNNING_MODE = 0,
    PM_EMERGENCY_MODE,
    PM_LISTEN_MODE,
    PM_SLEEP_MODE,
    PM_DEEP_SLEEP_MODE,
    PM_OFF_MODE
};

enum
{
    PM_RUNNING_ONLY = 0,
    PM_LISTEN_ONLY,
    PM_SLEEP_ONLY,
    PM_AUTO
};

void RunningModeCheck(void);
void pm_set_wakeup_val(unsigned int interval);
unsigned int pm_get_run_mode(void);
unsigned int pm_get_sys_mode(void);
void pm_set_sys_mode(unsigned int sys_mode);
unsigned int get_anti_thief_trigger(void);
unsigned int get_can_status(void);
unsigned int pm_sleep_available(void);

#endif

