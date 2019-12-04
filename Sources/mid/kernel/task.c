/****************************************************
 *          task function file                      *
 *                             www.intest.com.cn    *
 ****************************************************/

#include <interrupt_manager.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <osif.h>
#include <string.h>
#include <driver.h>

static taskStruct taskList[MAX_TASK_NUMBER];
static taskStruct *currentTaskPtr = NULL;
static unsigned char taskHighPriNo = INVALID_TASK_NUMBER;
static unsigned char lastHighPriTaskNo = INVALID_TASK_NUMBER;

/* init task */
void kernelInitTask(void)
{
    printk(PRINTK_LEVEL_LOG, "Initializing task structure: "
           "it's able to handle up to %d tasks.", MAX_TASK_NUMBER);
    /* init list */
    memset(taskList, 0, sizeof(taskStruct) * MAX_TASK_NUMBER);
}

/* create task */
unsigned int kernelCreateTask(task_hander taskHandler, char const *taskName)
{
    unsigned char i;

    /* find unused task */
    for (i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (taskList[i].taskMask & TASK_MASK_USED)
        {
            continue;
        }

        break;
    }

    if (i == MAX_TASK_NUMBER)
    {
        printk(PRINTK_LEVEL_ERROR, "Failed to create task: %s!", taskName);
        return 1;
    }

    taskList[i].taskMask = TASK_MASK_USED;
    taskList[i].taskHandler = taskHandler;
    taskList[i].taskID = i;
    taskList[i].debugLevel = TASK_LEVEL_LOG;
    memcpy(taskList[i].taskName, taskName, sizeof(taskList[i].taskName) - 1);

    printk(PRINTK_LEVEL_LOG, "Create task %d \"%s\".", i, taskName);

    if (strcmp(taskName, TASKNAME_SPI_COM) == 0)
    {
        taskHighPriNo = i;
    }

    return 0;
}

/* run task */
static unsigned int kernelRunTask(unsigned char i)
{
    unsigned int ret;
    unsigned int backupMask;

    currentTaskPtr = &taskList[i];

    /* disable interrupt */
    INT_SYS_DisableIRQGlobal();
    /* backup mask */
    backupMask = taskList[i].taskMask;
    taskList[i].taskMask = backupMask & (~ TASK_SYS_EVENT_MASK);
    dbgTraceTaskId(currentTaskPtr->taskID, backupMask, DBG_TRACE_ENTER);
    /* enable interrupt */
    INT_SYS_EnableIRQGlobal();

    ret = taskList[i].taskHandler(backupMask & TASK_SYS_EVENT_MASK);

    /* disable interrupt */
    INT_SYS_DisableIRQGlobal();
    /* restore mask */
    taskList[i].taskMask |= ret;
    dbgTraceTaskId(currentTaskPtr->taskID, backupMask, DBG_TRACE_EXIT);
    /* enable interrupt */
    INT_SYS_EnableIRQGlobal();

    currentTaskPtr = NULL;

    return 0;
}

static void kernelSuspendTask(char *taskName)
{
    unsigned char i = 0;

    for (i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (strcmp(taskName, taskList[i].taskName) == 0)
        {
            taskList[i].taskMask &= (~TASK_MASK_USED);
            break;
        }
    }

    if (i == MAX_TASK_NUMBER)
    {
        printk(PRINTK_LEVEL_ERROR, " failed to suspend task : %s!", taskName);
    }
    else
    {
        printk(PRINTK_LEVEL_LOG, " successful suspend task : %s!", taskName);
    }

    return;
}

static void kernelResumeTask(char *taskName)
{
    unsigned int i = 0;

    for (i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (strcmp(taskName, taskList[i].taskName) == 0)
        {
            taskList[i].taskMask |= TASK_MASK_USED;
            break;
        }
    }

    if (i == MAX_TASK_NUMBER)
    {
        printk(PRINTK_LEVEL_ERROR, " failed to resume task : %s!", taskName);
    }
    else
    {
        printk(PRINTK_LEVEL_LOG, " successful resume task : %s!", taskName);
    }

    return;
}

void kernelHighPriTaskReassign(char *taskName)
{
    unsigned char i = 0;

    if (lastHighPriTaskNo == INVALID_TASK_NUMBER)
    {
        for (i = 0; i < MAX_TASK_NUMBER; i++)
        {
            if ((taskList[i].taskMask & TASK_MASK_USED)
                && (strcmp(taskName, taskList[i].taskName) == 0))
            {
                break;
            }
        }

        if (i == MAX_TASK_NUMBER)
        {
            printk(PRINTK_LEVEL_ERROR, " failed to reassign high priority task to : %s!",
                   taskName);
            return;
        }

        lastHighPriTaskNo = taskHighPriNo;
        taskHighPriNo = taskList[i].taskID;
        printk(PRINTK_LEVEL_LOG, " reassign high priority task to : %d (%s)", taskHighPriNo,
               taskName);

        kernelSuspendTask(TASKNAME_SPI_COM);
        kernelSuspendTask(TASKNAME_MCU_MGR);
        return;
    }
}

void kernelRestoreHighPriTask(void)
{
    if (lastHighPriTaskNo != INVALID_TASK_NUMBER)
    {
        taskHighPriNo = lastHighPriTaskNo;
        lastHighPriTaskNo = INVALID_TASK_NUMBER;
        printk(PRINTK_LEVEL_LOG, " restore high priority task to : %d", taskHighPriNo);

        kernelResumeTask(TASKNAME_SPI_COM);
        kernelResumeTask(TASKNAME_MCU_MGR);
    }
}

unsigned char kernelGetTaskSuspendState(char *taskName)
{
    unsigned char i = 0;
    unsigned char suspended = 0;

    for (i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (strcmp(taskName, taskList[i].taskName) == 0)
        {
            if ((taskList[i].taskMask & TASK_MASK_USED) == 0)
            {
                suspended = 1;
            }
        }
    }

    return suspended;
}

/* run kernel */
void kernelRun(void)
{
    unsigned char i;

    /* task init */
    for (i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (taskList[i].taskMask & TASK_MASK_USED)
        {
            currentTaskPtr = &taskList[i];
            dbgTraceTaskId(currentTaskPtr->taskID, TASK_SYS_EVENT_INIT, DBG_TRACE_ENTER);
            taskList[i].taskHandler(TASK_SYS_EVENT_INIT);
            dbgTraceTaskId(currentTaskPtr->taskID, TASK_SYS_EVENT_INIT, DBG_TRACE_EXIT);
            currentTaskPtr = NULL;
        }
    }


    /* loop forever */
    while (1)
    {
        for (i = 0; i < MAX_TASK_NUMBER; i++)
        {
            /* check the high priority task first */
            if ((taskHighPriNo != INVALID_TASK_NUMBER)
                && (taskList[taskHighPriNo].taskMask & TASK_MASK_USED)
                && (taskList[taskHighPriNo].taskMask & TASK_SYS_EVENT_MASK))
            {
                kernelRunTask(taskHighPriNo);
            }

            /* reset WDG */
            driverWDGFeed();

            if ((taskList[i].taskMask & TASK_MASK_USED)
                && (taskList[i].taskMask & TASK_SYS_EVENT_MASK))
            {

                kernelRunTask(i);
            }

            /* reset WDG */
            driverWDGFeed();
        }
    }
}

/* get current task debug level */
unsigned char kernelGetTaskLevel(void)
{
    if (currentTaskPtr == NULL)
    {
        printk(PRINTK_LEVEL_ERROR, "This function should be called in task!");
        return 0;
    }

    return (currentTaskPtr->debugLevel);
}

/* set current task debug level */
void kernelSetTaskLevel(unsigned char level)
{
    if (currentTaskPtr == NULL)
    {
        printk(PRINTK_LEVEL_ERROR, "This function should be called in task!");
        return;
    }

    currentTaskPtr->debugLevel = level;

    return;
}

/* get current task name */
char *kernelGetTaskName(void)
{

	static unsigned char sInitTask[] = "Init task";
	if (currentTaskPtr == NULL)
    {
        printk(PRINTK_LEVEL_ERROR, "This function should be called in task!");
        return sInitTask;
    }

    return (currentTaskPtr->taskName);
}

/* get current task id */
unsigned char kernelGetTaskID(void)
{
    if (currentTaskPtr == NULL)
    {
        printk(PRINTK_LEVEL_ERROR, "This function should be called in task!");
        return 0;
    }

    return (currentTaskPtr->taskID);
}

unsigned char kernelGetTaskIdByName(char *name)
{
    unsigned char i = 0;

    for (i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (strcmp(name, taskList[i].taskName) == 0)
        {
            return taskList[i].taskID;
        }
    }

    return 0;
}

/*according to the name of task,get the task ID*/
unsigned char kernelGetTaskNameID(const char *taskname)
{
    unsigned char i;
    unsigned char id = INVALID_TASK_NUMBER;    //invalid value

    for (i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (taskList[i].taskMask & TASK_MASK_USED)
        {
            if (strcmp(taskList[i].taskName, taskname) == 0)
            {
                id = taskList[i].taskID;
                break;
            }
        }
    }

    return id;
}

/* set task event (used in ISR) */
void kernelSetTaskEvent(unsigned char taskID, unsigned int event)
{
    /* disable interrupt */
    INT_SYS_DisableIRQGlobal();
    taskList[taskID % MAX_TASK_NUMBER].taskMask |= (event & TASK_SYS_EVENT_MASK);
    /* enable interrupt */
    INT_SYS_EnableIRQGlobal();
    return;
}

/* unset task event */
void kernelUnsetTaskEvent(unsigned char taskID, unsigned int event)
{
    taskList[taskID % MAX_TASK_NUMBER].taskMask &= (~(event & TASK_SYS_EVENT_MASK));
    return;
}

/* show task info */
int kernelShowTask(unsigned int argc, unsigned char **argv)
{
    unsigned char i = 0;
    int integer, decimals;
    double ftemp;
    unsigned int total_uptime = 0;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - show system task info.r\n", argv[1]);
        return 0;
    }

    for (i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (taskList[i].taskMask & TASK_MASK_USED)
        {
            ftemp = taskList[i].uptime * 100.0 / SysTick_GetTickCount();
            integer = (int)(ftemp);
            decimals = (int)((ftemp - integer) * 1000);
            shellPrintf(" Task %02d: %02d.%03d%%   %s\r\n", i, integer, decimals,
                        taskList[i].taskName);
            total_uptime += taskList[i].uptime;
        }
    }

    ftemp = total_uptime * 100.0 / SysTick_GetTickCount();
    integer = (int)(ftemp);
    decimals = (int)((ftemp - integer) * 1000);
    shellPrintf(" Task All: %02d.%03d%%\r\n", integer, decimals);

    return 0;
}

/* show debug level string */
static char *convertDgbString(unsigned char level)
{
    switch (level)
    {
        case TASK_LEVEL_DEBUG:
            return "Debug Level";

        case TASK_LEVEL_LOG:
            return "Log Level";

        case TASK_LEVEL_ERROR:
            return "Error Level";

        default:
            return "Ooops...Unknown!";
    }
}

/* show debug level */
int kernelShowDbg(unsigned int argc, unsigned char **argv)
{
    unsigned char i = 0;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - show task debug level.\r\n", argv[1]);
        return 0;
    }

    for (i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (taskList[i].taskMask & TASK_MASK_USED)
        {
            shellPrintf(" Task %02d %8s: %s\r\n", i, taskList[i].taskName,
                        convertDgbString(taskList[i].debugLevel));
        }
    }

    return 0;
}

/* set debug level */
int kernelSetdbg(unsigned int argc, unsigned char **argv)
{
    unsigned char i = 0;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - set task debug level.\r\n", argv[1]);
        return 0;
    }

    if (argc != 2)
    {
        shellPrintf(" usage:  taskname debugLevel");
        return 0;
    }

    if (strcmp("printk", (char const *) argv[0]) == 0)
    {
        switch (argv[1][0])
        {
            case 'd':
            case 'D':
                kernelSetDebugLevel(PRINTK_LEVEL_DEBUG);
                break;

            case 'e':
            case 'E':
                kernelSetDebugLevel(PRINTK_LEVEL_ERROR);
                break;

            default:
                kernelSetDebugLevel(PRINTK_LEVEL_LOG);
                break;
        }

        return 0;
    }

    for (i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (taskList[i].taskMask & TASK_MASK_USED)
        {
            if (strcmp(taskList[i].taskName, (char const *) argv[0]) == 0)
            {
                break;
            }
        }
    }

    if (i == MAX_TASK_NUMBER)
    {
        shellPrintf(" Cannot find the task: %s\r\n", argv[0]);
        return 0;
    }

    switch (argv[1][0])
    {
        case 'd':
        case 'D':
            taskList[i].debugLevel = TASK_LEVEL_DEBUG;
            break;

        case 'e':
        case 'E':
            taskList[i].debugLevel = TASK_LEVEL_ERROR;
            break;

        default:
            taskList[i].debugLevel = TASK_LEVEL_LOG;
            break;
    }

    return 0;
}

/* update CPU usage of each task */
void kernelUpdateTaskUsage(void)
{
    if (currentTaskPtr != NULL)
    {
        currentTaskPtr->uptime++;
    }
}

