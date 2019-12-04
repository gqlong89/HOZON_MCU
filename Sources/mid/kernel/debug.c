/****************************************************
 *          debug function file                     *
 *                             www.intest.com.cn    *
 ****************************************************/

#include <interrupt_manager.h>
#include <mid/kernel/kernel.h>
#include <osif.h>
#include <stdio.h>
#include <string.h>
#include <sys/_stdint.h>
#include <S32K144.h>

unsigned int irq_cnt[20];

static unsigned char kDebugLevel = 0;
#define SYS_VER_NO        "MCU_J311HZCOMM003[A-11]"
#define SYS_VERSION       (SYS_VER_NO "\\" __DATE__ "\\" __TIME__)

static IRQ_PRIOR_MAP irq_pri[] =
{
    {   SysTick_IRQn,               0 },
    {   DMA_Error_IRQn,             1 },
    {   DMA0_IRQn,                  1 },    // dma for lpuart0tx(dbgUart)
    {   LPUART0_RxTx_IRQn,          2 },    // dbguart
    {   DMA1_IRQn,                  3 },    // dma for lpspi0rx(ipcSpi0)
    {   DMA2_IRQn,                  3 },    // dma for lpspi1tx(ipcSpi0)
    {   CAN0_ORed_IRQn,             4 }, /**< CAN0 OR'ed [Bus Off OR Transmit Warning OR Receive Warning] */
    {   CAN0_Error_IRQn,            4 }, /**< CAN0 Interrupt indicating that errors were detected on the CAN bus */
    {   CAN0_Wake_Up_IRQn,          4 }, /**< CAN0 Interrupt asserted when Pretended Networking operation is enabled, and a valid message matches the selected filter criteria during Low Power mode */
    {   CAN0_ORed_0_15_MB_IRQn,     4 }, /**< CAN0 OR'ed Message buffer (0-15) */
    {   CAN0_ORed_16_31_MB_IRQn,    4 }, /**< CAN0 OR'ed Message buffer (16-31) */
    {   CAN1_ORed_IRQn,             4 }, /**< CAN1 OR'ed [Bus Off OR Transmit Warning OR Receive Warning] */
    {   CAN1_Error_IRQn,            4 }, /**< CAN1 Interrupt indicating that errors were detected on the CAN bus */
    {   CAN1_ORed_0_15_MB_IRQn,     4 }, /**< CAN1 OR'ed Interrupt for Message buffer (0-15) */
    {   CAN2_ORed_IRQn,             4 }, /**< CAN2 OR'ed [Bus Off OR Transmit Warning OR Receive Warning] */
    {   CAN2_Error_IRQn,            4 }, /**< CAN2 Interrupt indicating that errors were detected on the CAN bus */
    {   CAN2_ORed_0_15_MB_IRQn,     4 }, /**< CAN2 OR'ed Message buffer (0-15) */
    {   LPSPI0_IRQn,                4 },
    {   LPSPI1_IRQn,                3 }, /**< SPI1 for SPICan1 higher than porta and porte */
    {   LPSPI2_IRQn,                3 }, /**< SPI2 for SPICan2 higher than porta and porte */
    {   PORTA_IRQn,                 4 },    // spi can 3
    {   PORTB_IRQn,                 6 },
    {   PORTC_IRQn,                 6 },
    {   PORTD_IRQn,                 6 },
    {   PORTE_IRQn,                 4 }, // spi can 4
    {   ADC0_IRQn,                  6 },
    {   ADC1_IRQn,                  6 },
    {   FTFC_IRQn,                  6 },
    {   LPI2C0_Master_IRQn,         6 },
    {   WDOG_EWM_IRQn,              6 },
    {   FTM1_Ovf_Reload_IRQn,       7 },    // low  timer
    {   FTM2_Ovf_Reload_IRQn,       7 },    // high timer
    {   FTM3_Ovf_Reload_IRQn,       4 },    // cantag timer
};

void irq_priority_init(void)
{
    memset(irq_cnt, 0, sizeof(irq_cnt));
    uint8_t i = (sizeof(irq_pri) / sizeof((irq_pri)[0]));

    for (i = 0; i < (sizeof(irq_pri) / sizeof((irq_pri)[0])); i++)
    {
        /* Set interrupt priorities. */
        INT_SYS_SetPriority(irq_pri[i].irqnum, irq_pri[i].priority);
    }
}

/* get current kernel debug level */
unsigned char kernelGetDebugLevel(void)
{
    return kDebugLevel;
}

/* set current kernel debug level */
void kernelSetDebugLevel(unsigned char level)
{
    kDebugLevel = level;
    return;
}

/* get software version */
char *kernelGetVersion(void)
{
    return SYS_VERSION;
}

/* get software version number */
char *kernelGetVerNo(void)
{
    return SYS_VER_NO;
}

char *kernelGetCurTimeStr(void)
{
    static char timeStr[24];
    /* <-begin RF00000001-yuzm-2017/07/07-t3_comment. */
    #if 0
    RTCTime localtime;

    memset(&localtime, 0, sizeof(localtime));
    driverRTCGetTime(&localtime);

    memset(timeStr, 0, sizeof(timeStr));
    snprintf(timeStr, sizeof(timeStr), "%02d/%02d/%02d %02d:%02d:%02d.%03u",
             localtime.RTC_Year, localtime.RTC_Mon, localtime.RTC_Mday,
             localtime.RTC_Hour, localtime.RTC_Min, localtime.RTC_Sec,
             OSIF_GetMilliseconds() % 1000);
    #endif
    /* RF00000001-yuzm end-> */

    return timeStr;
}

void dumphex(unsigned char *buf, int len, const char *funcname, int line)
{
    static char membuf[100];
    unsigned char *ptr = (unsigned char *) buf;
    char *ibuf = membuf;
    int i, j;
    int len1, len2;

    printf("\r\nDUMPHEX: func %s, line %d, len %d\r\n", funcname, line, len);
    len1 = len / 16 * 16;
    len2 = len % 16;
    ibuf[0] = 0;

#define BIN2CHAR(ch) (((ch) > ' ' && (ch) <= '~') ? (ch) : '.')

    for (i = 0; i < len1; i += 16)
    {
        ibuf = membuf;
        sprintf(ibuf, "0x%08X  ", (int) buf + i);
        ibuf += 12;

        for (j = 0; j < 16; j++)
        {
            sprintf(ibuf, "%02X ", ptr[j]);
            ibuf += 3;
        }

        sprintf(ibuf, "  ");
        ibuf += 2;

        for (j = 0; j < 16; j++)
        {
            sprintf(ibuf, "%c", BIN2CHAR(ptr[j]));
            ibuf++;
        }

        sprintf(ibuf, "\r\n");
        printf("%s", membuf);
        ptr += 16;
    }

    if (len2 == 0)
    {
        return;
    }

    ibuf = membuf;
    sprintf(ibuf, "0x%08X  ", (int) buf + ((len >> 4) << 4));
    ibuf += 12;

    for (i = 0; i < len2; i++)
    {
        sprintf(ibuf, "%02X ", ptr[i]);
        ibuf += 3;
    }

    for (i = len2; i < 16; i++)
    {
        sprintf(ibuf, "   ");
        ibuf += 3;
    }

    sprintf(ibuf, "  ");
    ibuf += 2;

    for (i = 0; i < len2; i++)
    {
        sprintf(ibuf, "%c", BIN2CHAR(ptr[i]));
        ibuf += 1;
    }

    sprintf(ibuf, "\r\n");

    printf("%s", membuf);
}

void float_to_string(double data, char *str)
{
    int i, j, k;
    long temp, tempoten;
    char intpart[10], dotpart[10];

    memset(str, 0, 10);

    if (data < 0)
    {
        str[0] = '-';
        data = -data;
    }
    else
    {
        str[0] = '+';
    }

    temp = (long) data;

    i = 0;
    tempoten = temp / 10;

    while (tempoten != 0)
    {
        intpart[i] = temp - 10 * tempoten + 48;
        temp = tempoten;
        tempoten = temp / 10;
        i++;
    }

    intpart[i] = temp + 48;

    data = data - (long) data;

    for (j = 0; j < 5; j++)
    {
        dotpart[j] = (int)(data * 10) + 48;
        data = data * 10.0;
        data = data - (long) data;
    }

    for (k = 1; k <= i + 1; k++)
    {
        str[k] = intpart[i + 1 - k];
    }

    str[i + 2] = '.';

    for (k = i + 3; k < i + j + 3; k++)
    {
        str[k] = dotpart[k - i - 3];
    }

}


static unsigned int irq_last[20];
static unsigned int last_tick = 0;
extern void ipc_list_show(void);
/* irq cnt */
int showirq(unsigned int argc, unsigned char **argv)
{
    unsigned int i;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - show irq cnt.\r\n", argv[1]);
        return 0;
    }

    shellPrintf(" ****IRQ Count Begin****\r\n");

    for (i = 0; i < sizeof(irq_cnt) / sizeof(irq_cnt[0]); i++)
    {
        if (irq_cnt[i])
        {
            shellPrintf(" irq[%03u]=[%010u,%010u],[%u]/Sec \r\n", i, irq_cnt[i], irq_last[i],
                        (irq_cnt[i] - irq_last[i]) * 100 / (SysTick_GetTickCount() - last_tick));
            irq_last[i] = irq_cnt[i];
        }
    }

    last_tick = SysTick_GetTickCount();
    shellPrintf(" tick = %d, cnt = %d\r\n", last_tick, SysTick_GetCurCounter());
    ipc_list_show();
    shellPrintf(" ****IRQ Count End****\r\n");
    return 1;
}

