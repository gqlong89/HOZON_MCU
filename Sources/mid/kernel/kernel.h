/****************************************************
 *          header file for kernel                  *
 *                             www.intest.com.cn    *
 ****************************************************/
#ifndef KERNEL_H
#define KERNEL_H

#define __BUFSIZ__  252     //  DMA MAX_SIZE is 256

#define MAX(a, b)           ((a) > (b) ? (a) : (b))
#define MIN(a, b)           ((a) < (b) ? (a) : (b))

extern unsigned char g_xModemCommEnable;

extern unsigned int shellRedirectBufSize;
extern char *shellRedirectBuf;
extern unsigned int redirectLen;

/* --------------- kernel printf ------------- */

/* used by printf */
extern int _write(int fd, char *ptr, int len);

/* from IAR library */
extern int printf(char const *, ...);

extern int snprintf(char *s, unsigned int size, const char *fmt, ...);
extern void dumphex(unsigned char *buf, int len, const char *funcname, int line);

/* printk for kernel and driver */
#define printk(level, ...) do {                         \
        if( ( !g_xModemCommEnable )                         \
            && (level) >= kernelGetDebugLevel() )         \
        {                                               \
            if(level == PRINTK_LEVEL_ERROR)             \
            {printf( "\033[47;31m" );}             \
            printf(" Kernel>");                         \
            printf("%s() ", __func__);                  \
            printf( __VA_ARGS__ );                      \
            printf("\r\n");                             \
            if(level == PRINTK_LEVEL_ERROR)             \
            {printf( "\033[0m" );}                 \
        }  } while (0)

/* printf for task */
#define taskPrintf(level, ...) do {                     \
        if( ( !g_xModemCommEnable )                         \
            && (level) >= kernelGetTaskLevel() )          \
        {                                               \
            if(level == TASK_LEVEL_ERROR)               \
            {printf( "\033[47;31m" );}             \
            printf(" %s>", kernelGetTaskName());        \
            printf("%s() ", __func__);                  \
            printf( __VA_ARGS__ );                      \
            printf("\r\n");                             \
            if(level == TASK_LEVEL_ERROR)               \
            {printf( "\033[0m" );}                 \
        }  } while (0)

/* printf for shell */
#define shellPrintf(...) do {                           \
        if( !g_xModemCommEnable )                           \
        {                                                   \
            printf( __VA_ARGS__ );                          \
            if( shellRedirectBufSize > 1 )       \
            {                                           \
                redirectLen = snprintf( (char *)shellRedirectBuf, shellRedirectBufSize , __VA_ARGS__ );   \
                if ( (redirectLen > 0) && ( redirectLen < shellRedirectBufSize ) )                        \
                {                                                      \
                    shellRedirectBufSize -= redirectLen; \
                    shellRedirectBuf += redirectLen;     \
                }                                                      \
                else                                    \
                {                                       \
                    shellRedirectBufSize = 0;    \
                }                                       \
            }                                           \
        }  } while (0)

#define DUMPHEX(level, buf, len)     do {               \
        if( ( ! g_xModemCommEnable ) &&                     \
            (level) >= kernelGetTaskLevel() )               \
        {                                                   \
            dumphex(buf, len, __func__, __LINE__);         \
        }  } while(0)

typedef struct
{
    int irqnum; /* interrupt num */
    unsigned char priority;
} IRQ_PRIOR_MAP;

/* GPS Ant status */
enum
{
    GPSANT_UNKNOW = 0,
    GPSANT_SHORT,
    GPSANT_OPEN,
    GPSANT_OK
};

/* MIC VOLTAGE status */
enum
{
    MICV_ERROR = 0,
    MICV_OK
};

enum
{
    MIC_UNKNOW = 0,
    MIC_SHORT_GND,
    MIC_SHORT_POWER,
    MIC_OPEN,
    MIC_OK
};

/*SOS 按键弹起，按下状态*/
enum
{
    SOS_UNKNOW = 0,
    SOS_UP,
    SOS_DOWN,
};

enum
{
    SPK_UNKNOW = 0,
    SPK_SHORT,
    SPK_OPEN,
    SPK_OK
};

enum
{
    ACC_R_UNKNOW = 0,
    ACC_R_OK,
    ACC_R_ERROR
};


enum
{
    BAT_POWER_UNKNOW = 0,
    BAT_POWER_OK,
    BAT_POWER_LOW,
    BAT_POWER_HIGH,
    BAT_POWER_NONE
};

#define FAULT_NOT_SUPPORT (0xff)


#define UNUSED( para ) ((void)para )

/* get gps ant status */
extern unsigned int kernelGetGpsAntStatus(void);
extern unsigned int kernelGetMICVStatus(void);
extern unsigned int kernelGetMICStatus(void);

/*sos butn*/
extern unsigned int kernelGetSOSStatus(void);

/*spkstatus*/
extern unsigned int kernelGetSPKStatus(void);

extern void initUTCTime(void);
extern unsigned int kernelGetCurrentUTCTime(void);
extern unsigned int kernelGetSleepUTCTime(void);
extern void kernelSetSleepUTCTime(unsigned int sleeptime);

extern void kernelMcumgrReinit(void);

extern float kernelGetTemp(void);

void float_to_string(double data, char *str);

unsigned char kernelGetDebugLevel(void);
void kernelSetDebugLevel(unsigned char level);
char *kernelGetVersion(void);
char *kernelGetVerNo(void);
char *kernelGetCurTimeStr(void);
void irq_priority_init(void);

extern void Delay(unsigned int ticks);
#endif

