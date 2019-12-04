



#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <stdint.h>
#include <timer.h>
#include <pm.h>
#include <call.h>
#include <app/power/power.h>
#include <app/can_mgr/can_mgr.h>
#include <adc.h>
#include <rtc.h>
#include <app/can_mgr/can_broadCast.h>
#include <app/power/power.h>
#include <config.h>
#include <app/nm_osek/nm_osekTimer.h>
#include <app/nm_osek/nm_osekCore.h>


#if USED_NM_OSEK


static int8_t timer_nmOsek_TTyp[2] = {TIMER_INVALID, TIMER_INVALID};
static int8_t timer_nmOsek_TMax[2] = {TIMER_INVALID, TIMER_INVALID};
static int8_t timer_nmOsek_TError[2] = {TIMER_INVALID, TIMER_INVALID};
static int8_t timer_nmOsek_TTx[2] = {TIMER_INVALID, TIMER_INVALID};
static int8_t timer_nmOsek_TWBS[2] = {TIMER_INVALID, TIMER_INVALID};
static int8_t timer_nmOsek_TSleepReqMin_LimpHome[2] = {TIMER_INVALID, TIMER_INVALID};
static int8_t timer_nmOsek_TSleepReqMin_Normal[2] = {TIMER_INVALID, TIMER_INVALID};
static int8_t timer_appActive[2] = {TIMER_INVALID, TIMER_INVALID};//  应用报文 首帧定时器

extern unsigned char canBasic_taskid;
extern NMControlBlock_ST  nm_OsekCB[2];

void canPort0_nmOsek_TTpyISR(int8_t handle)
{
    if (handle != timer_nmOsek_TTyp[0])
    {
        return;
    }

    nm_OsekCB[0].nmTimerOverFlow |= TTyp_MASK;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
}

void canPort1_nmOsek_TTpyISR(int8_t handle)
{
    if (handle != timer_nmOsek_TTyp[1])
    {
        return;
    }

    nm_OsekCB[1].nmTimerOverFlow |= TTyp_MASK;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
}


void canPort0_nmOsek_TMaxISR(int8_t handle)
{
    if (handle != timer_nmOsek_TMax[0])
    {
        return;
    }

    nm_OsekCB[0].nmTimerOverFlow |= TMax_MASK;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
}

void canPort1_nmOsek_TMaxISR(int8_t handle)
{
    if (handle != timer_nmOsek_TMax[1])
    {
        return;
    }

    nm_OsekCB[1].nmTimerOverFlow |= TMax_MASK;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
}

void canPort0_nmOsek_TErrorISR(int8_t handle)
{
    if (handle != timer_nmOsek_TError[0])
    {
        return;
    }

    nm_OsekCB[0].nmTimerOverFlow |= TError_MASK;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
}

void canPort1_nmOsek_TErrorISR(int8_t handle)
{
    if (handle != timer_nmOsek_TError[1])
    {
        return;
    }

    nm_OsekCB[1].nmTimerOverFlow |= TError_MASK;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
}

void canPort0_nmOsek_TTxISR(int8_t handle)
{
    if (handle != timer_nmOsek_TTx[0])
    {
        return;
    }

    nm_OsekCB[0].nmTimerOverFlow |= TTx_MASK;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
}


void canPort1_nmOsek_TTxISR(int8_t handle)
{
    if (handle != timer_nmOsek_TTx[1])
    {
        return;
    }

    nm_OsekCB[1].nmTimerOverFlow |= TTx_MASK;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
}

void canPort0_nmOsek_TWBSISR(int8_t handle)
{
    if (handle != timer_nmOsek_TWBS[0])
    {
        return;
    }

    nm_OsekCB[0].nmTimerOverFlow |= TWBS_MASK;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
}

void canPort1_nmOsek_TWBSISR(int8_t handle)
{
    if (handle != timer_nmOsek_TWBS[1])
    {
        return;
    }

    nm_OsekCB[1].nmTimerOverFlow |= TWBS_MASK;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
}

void canPort0_nmOsek_TSleepReqMin_LimpHomeISR(int8_t handle)
{
    if (handle != timer_nmOsek_TSleepReqMin_LimpHome[0])
    {
        return;
    }

    //  nm_OsekCB[0].nmTimerOverFlow | TSleepReqMin_LimpHome_MASK;

    nm_OsekCB[0].NetworkStatus.bit.presleepenable = 1;//

    nm_OsekCancleAlarm(0, TSleepReqMin_LimpHome_MASK);

}

void canPort1_nmOsek_TSleepReqMin_LimpHomeISR(int8_t handle)
{
    if (handle != timer_nmOsek_TSleepReqMin_LimpHome[1])
    {
        return;
    }

    //  nm_OsekCB[1].nmTimerOverFlow | TSleepReqMin_LimpHome_MASK;
    //触发任务

    nm_OsekCB[1].NetworkStatus.bit.presleepenable = 1;//

    nm_OsekCancleAlarm(1, TSleepReqMin_LimpHome_MASK);
}

void canPort0_nmOsek_TSleepReqMin_NormalISR(int8_t handle)
{
    if (handle != timer_nmOsek_TSleepReqMin_Normal[0])
    {
        return;
    }

    //nm_OsekCB[0].nmTimerOverFlow | TSleepReqMin_Normal_MASK;
    //触发任务

    nm_OsekCB[0].NetworkStatus.bit.presleepenable = 1;//

    nm_OsekCancleAlarm(0, TSleepReqMin_Normal_MASK);
}

void canPort1_nmOsek_TSleepReqMin_NormalISR(int8_t handle)
{
    if (handle != timer_nmOsek_TSleepReqMin_Normal[1])
    {
        return;
    }

    //nm_OsekCB[1].nmTimerOverFlow | TSleepReqMin_Normal_MASK;
    //触发任务

    nm_OsekCB[1].NetworkStatus.bit.presleepenable = 1;//

    nm_OsekCancleAlarm(1, TSleepReqMin_Normal_MASK);
}


void canPort0_appActiveISR(int8_t handle)
{
    if (handle != timer_appActive[0])
    {
        return;
    }

    nm_OsekCB[0].appActiveFlag = 1;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST0);//触发任务

    ltimer_stop(timer_appActive[0]);
}

void canPort1_appActiveISR(int8_t handle)
{
    if (handle != timer_appActive[1])
    {
        return;
    }

    nm_OsekCB[1].appActiveFlag = 1;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST0);//触发任务
    ltimer_stop(timer_appActive[1]);
}







/*注册NM相关定时器*/
void nm_TimerInit(void)
{
    unsigned char i;

    for (i = 0; i < 2; i++)
    {
        if (nm_OsekCB[i].nm_osekEN == 0)
        {
            continue;
        }
        else
        {
            if (timer_nmOsek_TTyp[i] == TIMER_INVALID) // 此定时器还没有创建
            {
                if (i == 0)
                {
                    timer_nmOsek_TTyp[i] = ltimer_create(canPort0_nmOsek_TTpyISR);//只创建，没有开启
                }
                else if (i == 1)
                {
                    timer_nmOsek_TTyp[i] = ltimer_create(canPort1_nmOsek_TTpyISR);//只创建，没有开启
                }
                else
                {}
            }

            if (timer_nmOsek_TMax[i] == TIMER_INVALID) // 此定时器还没有创建
            {
                if (i == 0)
                {
                    timer_nmOsek_TMax[i] = ltimer_create(canPort0_nmOsek_TMaxISR);//只创建，没有开启
                }
                else if (i == 1)
                {
                    timer_nmOsek_TMax[i] = ltimer_create(canPort1_nmOsek_TMaxISR);//只创建，没有开启
                }
                else
                {}
            }

            if (timer_nmOsek_TError[i] == TIMER_INVALID) // 此定时器还没有创建
            {
                if (i == 0)
                {
                    timer_nmOsek_TError[i] = ltimer_create(canPort0_nmOsek_TErrorISR);//只创建，没有开启
                }
                else if (i == 1)
                {
                    timer_nmOsek_TError[i] = ltimer_create(canPort1_nmOsek_TErrorISR);//只创建，没有开启
                }
                else
                {}
            }

            if (timer_nmOsek_TTx[i] == TIMER_INVALID) // 此定时器还没有创建
            {
                if (i == 0)
                {
                    timer_nmOsek_TTx[i] = ltimer_create(canPort0_nmOsek_TTxISR);//只创建，没有开启
                }
                else if (i == 1)
                {
                    timer_nmOsek_TTx[i] = ltimer_create(canPort1_nmOsek_TTxISR);//只创建，没有开启
                }
                else
                {}
            }

            if (timer_nmOsek_TWBS[i] == TIMER_INVALID) // 此定时器还没有创建
            {
                if (i == 0)
                {
                    timer_nmOsek_TWBS[i] = ltimer_create(canPort0_nmOsek_TWBSISR);//只创建，没有开启
                }
                else if (i == 1)
                {
                    timer_nmOsek_TWBS[i] = ltimer_create(canPort1_nmOsek_TWBSISR);//只创建，没有开启
                }
                else
                {}
            }

            if (timer_nmOsek_TSleepReqMin_LimpHome[i] == TIMER_INVALID) // 此定时器还没有创建
            {
                if (i == 0)
                {
                    timer_nmOsek_TSleepReqMin_LimpHome[i] = ltimer_create(
                            canPort0_nmOsek_TSleepReqMin_LimpHomeISR);//只创建，没有开启
                }
                else if (i == 1)
                {
                    timer_nmOsek_TSleepReqMin_LimpHome[i] = ltimer_create(
                            canPort1_nmOsek_TSleepReqMin_LimpHomeISR);//只创建，没有开启
                }
                else
                {}
            }

            if (timer_nmOsek_TSleepReqMin_Normal[i] == TIMER_INVALID) // 此定时器还没有创建
            {
                if (i == 0)
                {
                    timer_nmOsek_TSleepReqMin_Normal[i] = ltimer_create(
                            canPort0_nmOsek_TSleepReqMin_NormalISR);//只创建，没有开启
                }
                else if (i == 1)
                {
                    timer_nmOsek_TSleepReqMin_Normal[i] = ltimer_create(
                            canPort1_nmOsek_TSleepReqMin_NormalISR);//只创建，没有开启
                }
                else
                {}
            }

            if (timer_appActive[i] == TIMER_INVALID) // 此定时器还没有创建
            {
                if (i == 0)
                {
                    timer_appActive[i] = ltimer_create(canPort0_appActiveISR);//只创建，没有开启
                }
                else if (i == 1)
                {
                    timer_appActive[i] = ltimer_create(canPort1_appActiveISR);//只创建，没有开启
                }
                else
                {}
            }

        }
    }
}


/*停止NM相关定时器*/
void nm_StopTimer(unsigned char canport, unsigned int timerMask) //停止定时器
{

    if (canport >= 2)
    {
        printk(PRINTK_LEVEL_ERROR, "CAN port %d >=2", canport);
        return;
    }

    if ((timerMask & TTyp_MASK) != 0)
    {
        if (timer_nmOsek_TTyp[canport] != TIMER_INVALID) // 开启VIN定时器
        {
            ltimer_stop(timer_nmOsek_TTyp[canport]);
        }
    }

    if ((timerMask & TMax_MASK) != 0)
    {
        if (timer_nmOsek_TMax[canport] != TIMER_INVALID) // 开启VIN定时器
        {
            ltimer_stop(timer_nmOsek_TMax[canport]);
        }
    }


    if ((timerMask & TError_MASK) != 0)
    {
        if (timer_nmOsek_TError[canport] != TIMER_INVALID) // 开启VIN定时器
        {
            ltimer_stop(timer_nmOsek_TError[canport]);
        }
    }

    if ((timerMask & TTx_MASK) != 0)
    {
        if (timer_nmOsek_TTx[canport] != TIMER_INVALID) // 开启VIN定时器
        {
            ltimer_stop(timer_nmOsek_TTx[canport]);
        }
    }

    if ((timerMask & TWBS_MASK) != 0)
    {
        if (timer_nmOsek_TWBS[canport] != TIMER_INVALID) // 开启VIN定时器
        {
            ltimer_stop(timer_nmOsek_TWBS[canport]);
        }
    }

    if ((timerMask & TSleepReqMin_LimpHome_MASK) != 0)
    {
        if (timer_nmOsek_TSleepReqMin_LimpHome[canport] != TIMER_INVALID) // 开启VIN定时器
        {
            ltimer_stop(timer_nmOsek_TSleepReqMin_LimpHome[canport]);
        }
    }

    if ((timerMask & TSleepReqMin_Normal_MASK) != 0)
    {
        if (timer_nmOsek_TSleepReqMin_Normal[canport] != TIMER_INVALID) // 开启VIN定时器
        {
            ltimer_stop(timer_nmOsek_TSleepReqMin_Normal[canport]);
        }
    }

    if ((timerMask & TappActive_MASK) != 0)
    {
        if (timer_appActive[canport] != TIMER_INVALID) // 开启VIN定时器
        {
            ltimer_stop(timer_appActive[canport]);
        }
    }


}


/*重启NM相关定时器*/
void nm_RestartTimer(unsigned char canport, unsigned int timerMask)
{
    if (canport >= 2)
    {
        printk(PRINTK_LEVEL_ERROR, "CAN port %d >=2", canport);
        return;
    }


    if ((timerMask & TTyp_MASK) != 0)
    {

        nm_StopTimer(canport, timerMask); //先停止定时器

        if (timer_nmOsek_TTyp[canport] != TIMER_INVALID) // 再开启定时器
        {
            ltimer_start(timer_nmOsek_TTyp[canport], nm_OsekCB[canport].TTyp);
        }
    }

    if ((timerMask & TMax_MASK) != 0)
    {

        nm_StopTimer(canport, timerMask); //先停止定时器

        if (timer_nmOsek_TTyp[canport] != TIMER_INVALID) // 再开启定时器
        {
            ltimer_start(timer_nmOsek_TMax[canport], nm_OsekCB[canport].TMax);
        }
    }

    if ((timerMask & TError_MASK) != 0)
    {

        nm_StopTimer(canport, timerMask); //先停止定时器

        if (timer_nmOsek_TTyp[canport] != TIMER_INVALID) // 再开启定时器
        {
            ltimer_start(timer_nmOsek_TError[canport], nm_OsekCB[canport].TError);
        }
    }

    if ((timerMask & TTx_MASK) != 0)
    {

        nm_StopTimer(canport, timerMask); //先停止定时器

        if (timer_nmOsek_TTx[canport] != TIMER_INVALID) // 再开启定时器
        {
            ltimer_start(timer_nmOsek_TTx[canport], nm_OsekCB[canport].TTx);
        }
    }

    if ((timerMask & TWBS_MASK) != 0)
    {

        nm_StopTimer(canport, timerMask); //先停止定时器

        if (timer_nmOsek_TWBS[canport] != TIMER_INVALID) // 再开启定时器
        {
            ltimer_start(timer_nmOsek_TWBS[canport], nm_OsekCB[canport].TWbs);
        }
    }

    if ((timerMask & TSleepReqMin_LimpHome_MASK) != 0)
    {

        nm_StopTimer(canport, timerMask); //先停止定时器

        if (timer_nmOsek_TSleepReqMin_LimpHome[canport] != TIMER_INVALID) // 再开启定时器
        {
            ltimer_start(timer_nmOsek_TSleepReqMin_LimpHome[canport], nm_OsekCB[canport].TSleepReqMin_LimpHome);
        }
    }

    if ((timerMask & TSleepReqMin_Normal_MASK) != 0)
    {

        nm_StopTimer(canport, timerMask); //先停止定时器

        if (timer_nmOsek_TSleepReqMin_Normal[canport] != TIMER_INVALID) // 再开启定时器
        {
            ltimer_start(timer_nmOsek_TSleepReqMin_Normal[canport], nm_OsekCB[canport].TSleepReqMin_Normal);
        }
    }

    if ((timerMask & TappActive_MASK) != 0)
    {

        nm_StopTimer(canport, timerMask); //先停止定时器

        if (timer_appActive[canport] != TIMER_INVALID) // 再开启定时器
        {
            ltimer_start(timer_appActive[canport], nm_OsekCB[canport].TAppActive);
        }
    }

}





void nm_OsekSetAlarm(unsigned char canport, unsigned char timerMask) // 开启相应定时器。
{
    nm_RestartTimer(canport, timerMask);
}

void nm_OsekCancleAlarm(unsigned char canport, unsigned char timerMask) // 关闭定时器
{
    nm_StopTimer(canport, timerMask);
}

#endif

