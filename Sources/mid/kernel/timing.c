/****************************************************
 *          timing function file                    *
 *                             www.intest.com.cn    *
 ****************************************************/

#include <interrupt_manager.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <mid/kernel/timing.h>
#include <osif.h>
#include <string.h>

static volatile timerStruct timerList[MAX_TIMER_NUMBER];

/* init timer */
void kernelInitTimer(void)
{
    printk(PRINTK_LEVEL_LOG, "Initializing timer structure: "
           "it's able to handle up to %d timers.", MAX_TIMER_NUMBER);

    /* init list */
    memset((void *) timerList, 0, sizeof(timerStruct) * MAX_TIMER_NUMBER);
}

/* request timer */
unsigned char kernelRequestTimer(unsigned char *timerNoPtr)
{
    unsigned char i;

    /* find unused timer */
    for (i = 0; i < MAX_TIMER_NUMBER; i++)
    {
        if (timerList[i].timerMask & TIMER_MASK_USED)
        {
            continue;
        }

        break;
    }

    if (i == MAX_TIMER_NUMBER)
    {
        printk(PRINTK_LEVEL_ERROR, "Failed to request timer for task %d!", kernelGetTaskID());
        return TASK_TIMER_FULL;
    }

    timerList[i].timerMask = TIMER_MASK_USED;
    timerList[i].timerTaskID = kernelGetTaskID();

    *timerNoPtr = i;

    printk(PRINTK_LEVEL_LOG, "Request timer %d for task %d.", i, kernelGetTaskID());

    return TASK_TIMER_OK;
}

/* enable timer */
unsigned char kernelEnableTimer(unsigned char timerNo, unsigned int interval, unsigned int event)
{
    if (timerNo >= MAX_TIMER_NUMBER)
    {
        printk(PRINTK_LEVEL_ERROR, "Invalid timer number %d!", timerNo);
        return TASK_TIMER_INVALID;
    }

    if (!(timerList[timerNo].timerMask & TIMER_MASK_USED))
    {
        printk(PRINTK_LEVEL_ERROR, "Timer %d isn't inited!", timerNo);
        return TASK_TIMER_NOT_USED;
    }

    if (timerList[timerNo].timerTaskID != kernelGetTaskID())
    {
        /* printk( PRINTK_LEVEL_LOG, "Timer %d doesn't belong to task %d!", timerNo, kernelGetTaskID()); */
        /* return 3 ; */
    }

    timerList[timerNo].timerEvent = event;
    timerList[timerNo].timerUptime = SysTick_GetTickCount() + (interval / 10) - 1;

    /* start timing */
    timerList[timerNo].timerMask |= TIMER_MASK_TIMING;

    return TASK_TIMER_OK;
}

/* disable timer */
unsigned char kernelDisableTimer(unsigned char timerNo)
{
    unsigned char tmpMask;
    unsigned char taskID;

    if (timerNo >= MAX_TIMER_NUMBER)
    {
        printk(PRINTK_LEVEL_ERROR, "Invalid timer number %d!", timerNo);
        return TASK_TIMER_INVALID;
    }

    if (!(timerList[timerNo].timerMask & TIMER_MASK_USED))
    {
        printk(PRINTK_LEVEL_ERROR, "Timer %d isn't inited!", timerNo);
        return TASK_TIMER_NOT_USED;
    }

    tmpMask = timerList[timerNo].timerMask;

    /* stop timing */
    timerList[timerNo].timerMask &= (~ TIMER_MASK_TIMING);

    if (tmpMask & TIMER_MASK_TIMING)
    {
        /* disable interrupt */
        INT_SYS_DisableIRQGlobal();

        taskID = timerList[timerNo].timerTaskID;
        kernelUnsetTaskEvent(taskID, timerList[timerNo].timerEvent);

        /* enable interrupt */
        INT_SYS_EnableIRQGlobal();
    }

    return TASK_TIMER_OK;
}

/* request timer */
unsigned char kernelReleaseTimer(unsigned char timerNo)
{
    unsigned char ret;

    ret = kernelDisableTimer(timerNo);

    if (ret != TASK_TIMER_OK)
    {
        return ret;
    }

    /* clear mask */
    timerList[timerNo].timerMask = 0;

    return TASK_TIMER_OK;
}

/* scan timer(used in ISR) */
void kernelScanTimer(uint64_t uptime)
{
    unsigned char i;
    unsigned char taskID;

    /* find unused timer */
    for (i = 0; i < MAX_TIMER_NUMBER; i++)
    {
        if ((timerList[i].timerMask & TIMER_MASK_USED)
            && (timerList[i].timerMask & TIMER_MASK_TIMING))
        {
            if (uptime > timerList[i].timerUptime)
            {
                taskID = timerList[i].timerTaskID;
                kernelSetTaskEvent(taskID, timerList[i].timerEvent);
                timerList[i].timerMask &= (~ TIMER_MASK_TIMING);
            }
        }

    }

    return;
}

unsigned char kernelTimerIsEnable(unsigned char timerNo)
{
    if (timerNo >= MAX_TIMER_NUMBER)
    {
        printk(PRINTK_LEVEL_ERROR, "Invalid timer number %d!", timerNo);
        return TASK_TIMER_INVALID;
    }

    if (!(timerList[timerNo].timerMask & TIMER_MASK_USED))
    {
        printk(PRINTK_LEVEL_ERROR, "Timer %d isn't inited!", timerNo);
        return TASK_TIMER_NOT_USED;
    }

    if (timerList[timerNo].timerMask & TIMER_MASK_TIMING)
    {
        return TASK_TIMER_TIMING;
    }
    else
    {
        return TASK_TIMER_STOPED;
    }
}

/* show timer info */
int kernelShowTimer(unsigned int argc, unsigned char **argv)
{
    unsigned char i = 0;
    unsigned char taskID;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - show system timer info.\r\n", argv[1]);
        return 0;
    }

    for (i = 0; i < MAX_TIMER_NUMBER; i++)
    {
        if (timerList[i].timerMask & TIMER_MASK_USED)
        {
            taskID = timerList[i].timerTaskID;
            shellPrintf(" Timer %02d: task-%02d timing-%s \r\n", i, taskID,
                        (timerList[i].timerMask & TIMER_MASK_TIMING) ? "Yes" : "No");
        }
    }

    return 0;
}

void Delay(unsigned int ticks)
{
    while (ticks--)
    {
        __asm("NOP");
    }
}

