/**
 * @Title: comSpi_task.c
 * @author yuzhimin
 * @date Aug 10, 2017
 * @version V1.0
 */

#include <app/power/power.h>
#include <app/scom/scom_can_msg.h>
#include <app/scom/scom_para_msg.h>
#include <app/scom/scom_tl.h>
#include <app/scom/scom_tl_ex.h>
#include <app/tp/proxy/Proxy.h>
#include <flash.h>
#include <ipc_spi.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <mid/kernel/timing.h>
#include <mid/msg/msg_parse.h>
#include <osif.h>
#include <scom.h>
#include <scom_msg_def.h>
#include <string.h>
#include <sys/_stdint.h>
#include <wdg.h>
#include <app/custom/custom.h>


#define SPI_TLS_TIMER               TASK_SYS_EVENT_TIMER0
#define SPI_TLS_EX_TIMER            TASK_SYS_EVENT_TIMER1
#define SPI_TLS_NM_STATE            TASK_SYS_EVENT_REQST0  // ADD BY DSC
#define SPI_TLS_BUSOFF_STATE        TASK_SYS_EVENT_REQST1  // ADD BY DSC
#define SPI_TLS_NM_LIMPHOME_STATE   TASK_SYS_EVENT_REQST2  // ADD BY DSC

#define SPI_SUPPORT_EVENT           ( TASK_SYS_EVENT_READ | \
                                      TASK_SYS_EVENT_SEND | \
                                      TASK_SYS_DATA_READ  | \
                                      SPI_TLS_TIMER       | \
                                      SPI_TLS_EX_TIMER    | \
                                      SPI_TLS_NM_STATE    | \
                                      SPI_TLS_BUSOFF_STATE| \
                                      SPI_TLS_NM_LIMPHOME_STATE \
                                      )

#define SPI_TLS_TIMER_VALUE         (200)
#define SPI_TLS_EX_TIMER_VALUE      (2000)
#define SPI_SEND_NEXT_TIMER_VALUE   (10)


unsigned char scom_taskid = INVALID_TASK_NUMBER;

static unsigned char scom_tlsTimer;
static unsigned char scom_tlsExTimer;
static unsigned char scom_sendNextTimer;


static unsigned char scom_upg_sate = MCU_CHK_VER_STATE;
static uint64_t scom_timeout_stamp;
static unsigned int  mpu_fault_cnt = 0;
static MSG_RX rx_msg;
static unsigned char rx_msgbuf[512];

static scom_custom_msg_cb scom_custom_fun = NULL;

void scom_timeout_refresh(void)
{
    scom_timeout_stamp = driverGetUptime();
}

unsigned int scom_get_timeout(void)
{
    return (driverGetUptime() - scom_timeout_stamp);
}

unsigned int scom_get_fault_cnt(void)
{
    return mpu_fault_cnt;
}

void scom_clear_fault_cnt(void)
{
    mpu_fault_cnt = 0;
}

void scom_send_upg_req(void)
{
    scom_tl_send_msg(SCOM_TL_CMD_UPG_MCU_REQ, NULL, 0);
}

void scom_send_upg_ret(unsigned int ret)
{
    scom_tl_send_msg(SCOM_TL_CMD_UPG_MCU_FIN, (unsigned char *) &ret, sizeof(ret));
}

bool scom_is_need_upgrade(void)
{
    if ((MCU_CHK_VER_STATE == scom_upg_sate) ||
        (MCU_NEED_UPG_STATE == scom_upg_sate))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void scom_set_mcu_upg_state(unsigned char state)
{
    scom_upg_sate = state;
}

void scom_proc_hb(unsigned char *msg, unsigned int len)
{
    UNUSED(len);
    char *ver;
    unsigned int mpu_wdg = 0;

    mpu_wdg = *((int *)msg);

    if (0xffffffff != mpu_wdg)
    {
        mpu_fault_cnt++;
    }
    else
    {
        mpu_fault_cnt = 0;
    }

    ver = (char *)(msg + 4);

    if (!strlen(ver))
    {
        scom_set_mcu_upg_state(MCU_NO_NEED_UPG_STATE);
        taskPrintf(TASK_LEVEL_LOG, "invalid version");
        return;
    }

    if (0 != strncmp((const char *) ver, kernelGetVerNo(), strlen(kernelGetVerNo())))
    {
        taskPrintf(TASK_LEVEL_LOG, "new=[%s],old=[%s]", ver, kernelGetVerNo());
        taskPrintf(TASK_LEVEL_LOG, "there is a new version");
        scom_set_mcu_upg_state(MCU_NEED_UPG_STATE);
        unsigned int tmp = UPGRADE_FLAG_RAM_USED;
        Flash_EEPROM_WriteBuffer((unsigned char *)&tmp, UPGRADE_FLAG_RAM_ADDR, 4);
        driverReset();
    }
    else
    {
        scom_set_mcu_upg_state(MCU_NO_NEED_UPG_STATE);
    }
}

void scom_msg_proc_ex(unsigned char *msg, unsigned int len)
{
    SCOM_TL_MSG_HDR *tl_hdr = (SCOM_TL_MSG_HDR *) msg;

    switch (tl_hdr->msg_type)
    {
        case SCOM_TL_CMD_SLEEP_REQ:
            taskPrintf(TASK_LEVEL_ERROR, "MPU SEND SHUTDOWN CMD");
            PowerListen();
            break;

        case SCOM_TL_CMD_MPU_FAULT_SYN:
            scom_tl_ex_mpu_fault_syn(msg, len);
            break;

        case SCOM_TL_CMD_SET_TIME:
            scom_tl_ex_set_time(msg, len);
            break;

        case SCOM_TL_CMD_GET_TIME:
            scom_tl_ex_syn_time();
            break;

        case SCOM_TL_CMD_CAN_SEND_CFG:
        case SCOM_TL_CMD_CAN_BAUD:
        case SCOM_TL_CMD_CAN_DEFAULT_BAUD:
            scom_can_proc(tl_hdr, &msg[sizeof(SCOM_TL_MSG_HDR)]);
            break;

        case SCOM_TL_CMD_PARA_CFG:
            scom_tl_ex_para_cfg(msg, len);
            break;

        case SCOM_TL_CMD_MPU_STATUS_SYN:
            scom_tl_ex_mpu_status_syn(msg, len);
            break;

            #if 0

        case SCOM_TL_CMD_CUSTOM_PARA:
            if (scom_custom_fun)
            {
                scom_custom_fun(&msg[sizeof(SCOM_TL_MSG_HDR)], tl_hdr->flen);
            }

            break;
            #endif

        case SCOM_TL_CMD_MCU_SHELL:
            scom_tl_ex_mcu_shell(msg, len);
            break;

        case SCOM_TL_CMD_WAKE_TIME:
            scom_tl_ex_wake_time(msg, len);
            break;

        default:

            //taskPrintf(TASK_LEVEL_ERROR, "unknown msgid=%u!", tl_hdr->msg_type);
            if (scom_custom_fun)
            {
                scom_custom_fun(tl_hdr->msg_type, &msg[sizeof(SCOM_TL_MSG_HDR)], tl_hdr->flen);
            }

            break;
    }
}


int scom_msg_proc(unsigned char *msg, unsigned int len)
{
    SCOM_TL_MSG_HDR *tl_hdr = (SCOM_TL_MSG_HDR *) msg;
    taskPrintf(TASK_LEVEL_DEBUG, "recv [%u,%u,%u]", tl_hdr->msg_type, tl_hdr->ftype, tl_hdr->flen);

    if (len < sizeof(SCOM_TL_MSG_HDR))
    {
        return -1;
    }

    scom_timeout_refresh();

    switch (tl_hdr->msg_type)
    {
        case SCOM_TL_CMD_HEARTBEAT:
            scom_proc_hb(msg + sizeof(SCOM_TL_MSG_HDR), len - sizeof(SCOM_TL_MSG_HDR));
            break;

        case SCOM_TL_CMD_UDS_MSG:
            proxy_download(msg , len);
            break;

        default:
            scom_msg_proc_ex(msg , len);
            break;
    }

    return 0;
}

/*******************************************************************
 function:     scom_reg_custom_msg_cb
 description:  register application message process function
 input:        scom_custom_msg_cb callback
 output:       none
 return:       none
 ********************************************************************/
void scom_reg_custom_msg_cb(scom_custom_msg_cb callback)
{
    scom_custom_fun = callback;
}

unsigned int scom_task(unsigned int mask)
{
    int len = 0;

    /* init of task */
    if (mask & TASK_SYS_EVENT_INIT)
    {
        taskPrintf(TASK_LEVEL_DEBUG, "TASK_SYS_EVENT_INIT!");
        scom_taskid = kernelGetTaskID();
        ipc_set_taskid(scom_taskid);
        msg_init_rx(&rx_msg, rx_msgbuf, sizeof(rx_msgbuf));
        scom_tl_init();
        scom_tl_reg_tx_fun(ipc_write);
        scom_tl_reg_proc_fun(scom_msg_proc);
        kernelRequestTimer(&scom_tlsTimer);
        kernelRequestTimer(&scom_tlsExTimer);
        kernelRequestTimer(&scom_sendNextTimer);
        kernelEnableTimer(scom_tlsTimer, SPI_TLS_TIMER_VALUE, SPI_TLS_TIMER);
        kernelEnableTimer(scom_tlsExTimer, SPI_TLS_EX_TIMER_VALUE, SPI_TLS_EX_TIMER);
        return 0;
    }

    if (mask & TASK_SYS_EVENT_SEND)
    {
        mask &= (~TASK_SYS_EVENT_SEND);

        if (ipc_send())
        {
            kernelEnableTimer(scom_sendNextTimer, SPI_SEND_NEXT_TIMER_VALUE, TASK_SYS_EVENT_SEND);
        }
    }

    if (mask & TASK_SYS_EVENT_READ)
    {
        if (rx_msg.used >= rx_msg.size)
        {
            rx_msg.used = 0;
            taskPrintf(TASK_LEVEL_ERROR, "rx_msg clean!");
        }

        len = ipc_read(rx_msg.data + rx_msg.used, rx_msg.size - rx_msg.used);

        if (len > 0)
        {
            rx_msg.used += len;
            msg_decode(&rx_msg, scom_tl_msg_proc);
        }
        else
        {
            mask &= (~TASK_SYS_EVENT_READ);
            //taskPrintf(TASK_LEVEL_DEBUG, "read NULL!");
        }
    }

    if (mask & SPI_TLS_TIMER)
    {
        mask &= (~SPI_TLS_TIMER);
        scom_tl_timeout_proc();
        kernelEnableTimer(scom_tlsTimer, SPI_TLS_TIMER_VALUE, SPI_TLS_TIMER);
    }

    if (mask & SPI_TLS_EX_TIMER)
    {
        mask &= (~SPI_TLS_EX_TIMER);

        scom_tl_ex_mcu_cfg_syn();

        scom_tl_ex_mcu_fault_syn();

        scom_tl_ex_mcu_status_syn();

        scom_can_baud_sync();

        scom_can_default_baud_sync();

        scom_tl_ex_report_ver();

        scom_tl_ex_report_btl_ver();

	//	scom_tl_NM_bussleep_state_syn();//ADD BY DSC
		
	//	scom_tl_can_busoff_state_syn();// ADD BY DSC

	//	scom_tl_NM_limphome_state_syn();//ADD BY DSC

        kernelEnableTimer(scom_tlsExTimer, SPI_TLS_EX_TIMER_VALUE, SPI_TLS_EX_TIMER);
        //shell_cmd_proc("showirq", strlen("showirq"));
    }

#if 0
    /*  ·¢ËÍÍøÂç¹ÜÀí×´Ì¬*/
	if (mask & SPI_TLS_NM_STATE)
	{
		mask &= (~SPI_TLS_NM_STATE);
		scom_tl_NM_bussleep_state_syn();
	}

    /* ·¢ËÍBUSOFF×´Ì¬*/
	if (mask & SPI_TLS_BUSOFF_STATE)
	{
		mask &= (~SPI_TLS_BUSOFF_STATE);
		scom_tl_can_busoff_state_syn();
	}

	/* ·¢ËÍlimphome×´Ì¬*/
	if (mask & SPI_TLS_NM_LIMPHOME_STATE)
	{
		mask &= (~SPI_TLS_NM_LIMPHOME_STATE);
		scom_tl_NM_limphome_state_syn();
	}
#endif

    /* unknown event */
    if (mask & (~SPI_SUPPORT_EVENT))
    {
        //taskPrintf(TASK_LEVEL_ERROR, "Got unknown event %08x!", mask);
        mask &= (~SPI_SUPPORT_EVENT);
    }

    return mask;
}

