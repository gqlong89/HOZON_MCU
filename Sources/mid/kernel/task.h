/****************************************************
 *          task header file                        *
 *                             www.intest.com.cn    *
 ****************************************************/
#ifndef KERNEL_TASK_H
#define KERNEL_TASK_H

typedef unsigned int (*task_hander)(unsigned int mask);

typedef struct
{
    volatile unsigned int taskMask;
    unsigned long long uptime;
    task_hander taskHandler;
    char taskName[10];
    unsigned char debugLevel;
    unsigned char taskID;
} taskStruct;

#define MAX_TASK_NUMBER             5          /* task number */
#define INVALID_TASK_NUMBER         0xFF

/* used by taskMask */
#define TASK_MASK_USED              (1<<0)      /* current task item is used */

#define TASK_SYS_EVENT_MASK   0xFFFF0000
#define TASK_SYS_EVENT_INIT   (1<<16)
#define TASK_SYS_EVENT_READ   (1<<17)
#define TASK_SYS_EVENT_SEND   (1<<18)
#define TASK_SYS_EVENT_TIMER0 (1<<19)
#define TASK_SYS_EVENT_TIMER1 (1<<20)
#define TASK_SYS_EVENT_REQST0 (1<<21)
#define TASK_SYS_EVENT_REQST1 (1<<22)
#define TASK_SYS_EVENT_REQST2 (1<<23)
#define TASK_SYS_EVENT_REQST3 (1<<24)  //ADD DSC
#define TASK_SYS_EVENT_REQST4 (1<<24)  //ADD DSC


#define TASKNAME_MCU_MGR                "mcumgr"
#define TASKNAME_SPI_COM                "scom"
#define TASKNAME_ECU_UPGRADE            "ecuupg"
#define TASKNAME_UDS                    "uds"
#define TASKNAME_SHELL                  "shell"
#define TASKNAME_CANBASIC               "canBasic"


/* printk level */
enum
{
    PRINTK_LEVEL_DEBUG = 0,
    PRINTK_LEVEL_LOG,
    PRINTK_LEVEL_ERROR
};

/* printf task level */
enum
{
    TASK_LEVEL_DEBUG = 0,
    TASK_LEVEL_LOG,
    TASK_LEVEL_ERROR
};

void kernelInitTask(void);
unsigned int kernelCreateTask(task_hander taskHandler, char const *taskName);
void kernelHighPriTaskReassign(char *taskName);
void kernelRestoreHighPriTask(void);
unsigned char kernelGetTaskSuspendState(char *taskName);

unsigned char kernelGetTaskLevel(void);
void kernelSetTaskLevel(unsigned char level);

char *kernelGetTaskName(void);
unsigned char kernelGetTaskID(void);
unsigned char kernelGetTaskIdByName(char *name);
unsigned char kernelGetTaskNameID(const char *taskname);

void kernelSetTaskEvent(unsigned char taskID, unsigned int event);
void kernelUnsetTaskEvent(unsigned char taskID, unsigned int event);

int kernelShowTask(unsigned int argc, unsigned char **argv);
int kernelShowDbg(unsigned int argc, unsigned char **argv);
int kernelSetdbg(unsigned int argc, unsigned char **argv);

void kernelRun(void);
void kernelUpdateTaskUsage(void);

#endif

