/**
 * @Title: can_flexcan.c
 * @author yuzhimin
 * @date Jul 17, 2017
 * @version V1.0
 */

#include <app/can_mgr/can_mgr.h>
#include <can_api.h>
#include <devassert.h>
#include <flexcan_driver.h>
#include <flexcan_hal.h>
#include <ipc_spi.h>
#include <mid/kernel/kernel.h>
#include <osif.h>
#include <status.h>
#include <stdbool.h>
#include <string.h>
#include <gpio_hal.h>
#include <gpio.h>
#include <sys/_stdint.h>
#include <S32K144.h>
#include <mid/can_flexcan/can_flexcan.h>
#include <app/nm_osek/nm_osekCore.h>
#include <mid/kernel/task.h>
#include <app/can_mgr/can_basic_cfg.h>
#include <app/nm_autosar/nm_autosar.h>

void flexcan_ecallbl_ctrl(CAN_MSG msg);
flexcan_state_t can_flexcan_State[NATIVE_CAN_MAX];

static volatile uint32_t txComplete[NATIVE_CAN_MAX];
static flexcan_msgbuff_t rxFrame[NATIVE_CAN_MAX];

#if USED_NM_OSEK
extern NMControlBlock_ST  nm_OsekCB[2];
#endif

#if USED_NM_AUTOSAR
extern CAN_PORT_STATE nm_canState[2];
#endif
//extern unsigned char canBasic_taskid ;//
static void flexcan_recv_data(uint8_t instance, uint32_t status)
{
    DEV_ASSERT(instance < NATIVE_CAN_MAX);
    uint8_t i;
    CAN_MSG *can_buf = NULL;
    CAN_MSG msg;
	CAN_SEND_MSG can;
    flexcan_msgbuff_t *frame;
    frame = &rxFrame[instance];
    irq_cnt[3 + instance]++;

    uint32_t uptimeWhenGot, miscUptimeWhenGot, flag, check;
    uptimeWhenGot       = SysTick_GetTickCount();
    miscUptimeWhenGot   = SysTick_GetCurCounter();
    flag                = SysTick_GetCountFlag();
    check               = SysTick_GetCurCounter();

    if (flag && (check <= miscUptimeWhenGot))
    {
        uptimeWhenGot += 1;
    }

    if (frame->cs & CAN_CS_RTR_MASK)
    {
        memset(frame, 0, sizeof(flexcan_msgbuff_t));

        if (status == RX_CAN_STD)
        {
            FLEXCAN_DRV_Receive(instance, RX_CAN_STD, frame);
        }

        if (status == RX_CAN_EXT)
        {
            FLEXCAN_DRV_Receive(instance, RX_CAN_EXT, frame);
        }

        return;
    }

    msg.miscUptime  = miscUptimeWhenGot >> 4;
    msg.uptime     = uptimeWhenGot;
    msg.type       = 'C';
    msg.dlc        = frame->dataLen;
    msg.MsgID      = (frame->cs & CAN_CS_IDE_MASK) ? (frame->msgId | (1u << 31)) : (frame->msgId);
    msg.brs        = !!(frame->cs & CAN_MB_BRS_MASK);
    msg.isEID      = !!(frame->cs & CAN_CS_IDE_MASK);
    msg.canFD      = !!(frame->cs & CAN_MB_EDL_MASK);
    msg.esi        = !!(frame->cs & CAN_MB_ESI_MASK);
    msg.isRTR      = !!(frame->cs & CAN_CS_RTR_MASK);

    for (i = 0; i < frame->dataLen; i++)
    {
        msg.Data[i] = frame->data[i];
    }

    for (; i < sizeof(can_buf->Data); i++)
    {
        msg.Data[i] = 0;
	}
	flexcan_ecallbl_ctrl(msg);
#if USED_NM_OSEK   // OSEK ?ï¿½æ”¶ï¿??æ»¤å‡½ï¿??
    NM_RxIndication(instance, &msg);
#endif

#if USED_NM_AUTOSAR // AUTOSAR ?ï¿½æ”¶ï¿??æ»¤å‡½ï¿??
	NM_PDU_ReceiveCheck(instance, &msg);
#endif

    if ((can_buf = (CAN_MSG *)ipc_can_handle(SPI_GET_FRAME)) != NULL)
    {
        memcpy((unsigned char *)can_buf, (unsigned char *)&msg, sizeof(msg));
        if (can_receive(instance, can_buf))
        {
            ipc_can_handle(SPI_SET_FRAME);
        }
        else
        {
            ipc_can_handle(SPI_DROP_FRAME);
        }
    }
    else
    {
        irq_cnt[16]++;
    }

    if (status == RX_CAN_STD)
    {
        FLEXCAN_DRV_Receive(instance, RX_CAN_STD, frame);
    }

    if (status == RX_CAN_EXT)
    {
        FLEXCAN_DRV_Receive(instance, RX_CAN_EXT, frame);
    }

    return;
}


// Can not add any printf in this function
static void flexcan_irq_err(uint8_t instance, uint32_t result)
{
    DEV_ASSERT(instance < NATIVE_CAN_MAX);
    UNUSED(instance);
	
#if USED_NM_OSEK	  
if(nm_OsekCB[instance].currentState == NM_stBusSleep)
    {		
		flexcan_clear_errstatus(instance);	
		flexcan_set_errint(instance, false);
		CanBus_ErrorCheck(instance);
    }
#endif

#if USED_NM_AUTOSAR
	if(nm_canState[instance].nm_currentState == NM_SLEEP_MODE_STATE)
	{
		flexcan_clear_errstatus(instance);		
		flexcan_set_errint(instance, false);
		CanBus_ErrorCheck_AUTUSAR(instance);
	}
#endif

    if (result & kFLEXCAN_ErrorIntFlag)     // Any error occurs
    {
        can_err_set(instance, STATE_ERR);
    }

    if (result & kFLEXCAN_BusOffIntFlag)     // bus off
    {
        can_err_set(instance, STATE_BUSOFF);
    }
}
extern unsigned char canBasic_taskid;

static void flexcan_callback(uint8_t instance, flexcan_event_type_t eventType,
                             flexcan_state_t *flexcanState)
{
    DEV_ASSERT(instance < NATIVE_CAN_MAX);

    switch (eventType)
    {
        case FLEXCAN_EVENT_RXFIFO_COMPLETE:
            flexcan_recv_data(instance, flexcanState->result);
            can_err_set(instance, STATE_RECEIVE);
            can_rectime_set(instance);
            break;

        case FLEXCAN_EVENT_TX_COMPLETE:
            if (flexcanState->result <= TX_CAN_BUFF_MAX)
            {
                txComplete[instance] |= (0x1U << flexcanState->result);
            }

#if USED_NM_OSEK           
			/*ÍøÂç¹ÜÀí±¨ÎÄ·¢ËÍÍê³É*/
			if (flexcanState->result == TX_CAN_NM_OSEK)
            {    
                NM_TxConformation(instance, flexcanState->result);
            }
#endif     
			/*Ó¦ÓÃ±¨ÎÄ·¢ËÍÍê³É*/
			if (flexcanState->result >= TX_CAN_GENERAL1)
            {
                kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST2);
            }

            can_err_set(instance, STATE_SEND);
            break;

        case FLEXCAN_EVENT_ERROR_STATUS:
            flexcan_irq_err(instance, flexcanState->result);

            break;

        case FLEXCAN_EVENT_RX_COMPLETE:
            flexcan_recv_data(instance, flexcanState->result);
            can_err_set(instance, STATE_RECEIVE);
            can_rectime_set(instance);
            break;

        default:
            break;
    }
}

int flexcan_init(uint8_t instance, tCanBaud baud, flexcan_operation_modes_t mode)
{
	flexcan_user_config_t initConfig;

	DEV_ASSERT(instance < NATIVE_CAN_MAX);
    status_t ret;

    const flexcan_time_segment_t bitrate[] =
    {
        {7, 6, 3, 79, 1 },   /* 50K      80% */
        {7, 6, 3, 39, 1 },   /* 100K     80% */
        {7, 6, 3, 31, 1 },   /* 125K     80% */
        {7, 6, 3, 15, 1 },   /* 250K     80% */
        {7, 6, 3, 7, 1 },    /* 500K     80% */
        {7, 6, 3, 3, 1 },    /* 1M       80% */
    };

    const flexcan_time_segment_t canfd_bitrate[] =
    {
        {23, 7, 7, 1, 1},    /* 1M        80% */
        {23, 7, 7, 0, 1},    /* 2M        80% */
        {11, 3, 3, 0, 1},    /* 4M        80% */
        {9, 2, 2, 0, 1},     /* 5M        81.25% */
        {5, 1, 1, 0, 1},     /* 8M        80% */
    };

    flexcan_data_info_t dataInfo =
    {
        .msg_id_type = FLEXCAN_MSG_ID_STD,
        .fd_padding  = 0U
    };
////////////////////////////////////////////////////////


	if((instance == CAN_PORT0)&&(baud.canfd_en == 1))
	{
		initConfig.fd_enable = baud.canfd_en;
        initConfig.pe_clock = FLEXCAN_CLK_SOURCE_SYS;
        initConfig.max_num_mb = TX_CAN_BUFF_MAX + 1;  //    Ê¹ï¿½ï¿½CAN FDï¿½ï¿½ï¿½ï¿½ÏµÍ³
        initConfig.num_id_filters = FLEXCAN_RX_FIFO_ID_FILTERS_8;
        initConfig.is_rx_fifo_needed = false;
        initConfig.flexcanMode = mode;
        initConfig .payload = FLEXCAN_PAYLOAD_SIZE_64;
        initConfig.transfer_type = FLEXCAN_RXFIFO_USING_INTERRUPTS;
        initConfig.rxFifoDMAChannel = 0U;
	}
	else
	{
		initConfig.fd_enable = false;
		initConfig.pe_clock = FLEXCAN_CLK_SOURCE_SYS;
		initConfig.max_num_mb = TX_CAN_NORMAL_BUFF_MAX + 1;	 //can0ï¿½ï¿½ï¿?2ï¿½ï¿½ï¿½ï¿½ï¿½ä£¬can1  can2ï¿½ï¿½ï¿?6ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
		initConfig.num_id_filters = FLEXCAN_RX_FIFO_ID_FILTERS_8;
		initConfig.is_rx_fifo_needed = false;
		initConfig.flexcanMode = mode;
		initConfig.payload = FLEXCAN_PAYLOAD_SIZE_8;
		initConfig.transfer_type = FLEXCAN_RXFIFO_USING_INTERRUPTS;
		initConfig.rxFifoDMAChannel = 0U;

	}
        
    if(instance) //
    {
        baud.canfd_en = 0;
        baud.canfd_brs = 0;
        baud.canfd_baud = 0;
    }
	
    switch (baud.can_normal_baud)
    {
        case 50:
            memcpy(&initConfig.bitrate, &bitrate[0], sizeof(flexcan_time_segment_t));
            memcpy(&initConfig.bitrate_cbt, &bitrate[0], sizeof(flexcan_time_segment_t));
            break;

        case 100:
            memcpy(&initConfig.bitrate, &bitrate[1], sizeof(flexcan_time_segment_t));
            memcpy(&initConfig.bitrate_cbt, &bitrate[1], sizeof(flexcan_time_segment_t));
            break;

        case 125:
            memcpy(&initConfig.bitrate, &bitrate[2], sizeof(flexcan_time_segment_t));
            memcpy(&initConfig.bitrate_cbt, &bitrate[2], sizeof(flexcan_time_segment_t));
            break;

        case 250:
            memcpy(&initConfig.bitrate, &bitrate[3], sizeof(flexcan_time_segment_t));
            memcpy(&initConfig.bitrate_cbt, &bitrate[3], sizeof(flexcan_time_segment_t));
            break;

        case 500:
            memcpy(&initConfig.bitrate, &bitrate[4], sizeof(flexcan_time_segment_t));
            memcpy(&initConfig.bitrate_cbt, &bitrate[4], sizeof(flexcan_time_segment_t));
            break;

        case 1000:
            memcpy(&initConfig.bitrate, &bitrate[5], sizeof(flexcan_time_segment_t));
            memcpy(&initConfig.bitrate_cbt, &bitrate[5], sizeof(flexcan_time_segment_t));
            break;

        default: // unsupport baudrate
            return -1;
    }


	if((instance == CAN_PORT0)&&(baud.canfd_en == 1))
	{
		switch (baud.canfd_baud)
   		{
        	case 1000:
            	memcpy(&initConfig.bitrate_cbt, &canfd_bitrate[0], sizeof(flexcan_time_segment_t));
            	break;
        	case 2000:
            	memcpy(&initConfig.bitrate_cbt, &canfd_bitrate[1], sizeof(flexcan_time_segment_t));
            	break;
        	case 4000:
            	memcpy(&initConfig.bitrate_cbt, &canfd_bitrate[2], sizeof(flexcan_time_segment_t));
           		break;
        	case 5000:
            	memcpy(&initConfig.bitrate_cbt, &canfd_bitrate[3], sizeof(flexcan_time_segment_t));
            	break;
       	 	default:
            	break;
    	}
	}
    


	if((instance == CAN_PORT0)&&(baud.canfd_en == 1))
	{
		memset(&rxFrame[instance], 0, sizeof(flexcan_msgbuff_t));
    	//txComplete[instance] = 0xFFFFFFFFU;

    	FLEXCAN_DRV_Reset(instance);
    	ret = FLEXCAN_DRV_Init(instance, &can_flexcan_State[instance], &initConfig);
    	FLEXCAN_DRV_ConfigRxMb(instance, RX_CAN_STD, &dataInfo, 0);         /* ï¿?0ï¿?5ï¿?0ï¿?7ï¿?0ï¿?8ï¿?0ï¿?9Â±Ãª?ï¿?0ï¿?4ï¿?0ï¿?0ï¿?0ï¿?3 */

   		dataInfo.msg_id_type = FLEXCAN_MSG_ID_EXT;
    	//dataInfo.msg_id_type = FLEXCAN_MSG_ID_EXT;
    	FLEXCAN_DRV_ConfigRxMb(instance, RX_CAN_EXT, &dataInfo, 0);         /* ï¿?0ï¿?5ï¿?0ï¿?7ï¿?0ï¿?8ï¿?0ï¿?9ï¿?0ï¿?8ï¿?0ï¿?8ï¿?0ï¿?9ï¿?0ï¿?1ï¿?0ï¿?0ï¿?0ï¿?3 */

    	FLEXCAN_DRV_SetRxMbGlobalMask(instance, FLEXCAN_MSG_ID_STD, 0x0);
    	FLEXCAN_DRV_SetRxMbGlobalMask(instance, FLEXCAN_MSG_ID_EXT, 0x0);

    	FLEXCAN_DRV_InstallEventCallback(instance, flexcan_callback, NULL);
   		FLEXCAN_DRV_Receive(instance, RX_CAN_STD, &rxFrame[instance]);
    	FLEXCAN_DRV_Receive(instance, RX_CAN_EXT, &rxFrame[instance]);
	}

	
	else
	{
		memset(&rxFrame[instance], 0, sizeof(flexcan_msgbuff_t));
    //	txComplete[instance] = 0xFFFFFFFFU;

    	ret = FLEXCAN_DRV_Init(instance, &can_flexcan_State[instance], &initConfig);
    	FLEXCAN_DRV_ConfigRxMb(instance, RX_CAN_NORMAL_STD, &dataInfo, 0);

    	dataInfo.msg_id_type = FLEXCAN_MSG_ID_EXT;
    	FLEXCAN_DRV_ConfigRxMb(instance, RX_CAN_NORMAL_EXT, &dataInfo, 0);

    	FLEXCAN_DRV_SetRxMbGlobalMask(instance, FLEXCAN_MSG_ID_STD, 0x0);
    	FLEXCAN_DRV_SetRxMbGlobalMask(instance, FLEXCAN_MSG_ID_EXT, 0x0);

    	FLEXCAN_DRV_InstallEventCallback(instance, flexcan_callback, NULL);
    	FLEXCAN_DRV_Receive(instance, RX_CAN_NORMAL_STD, &rxFrame[instance]);
    	FLEXCAN_DRV_Receive(instance, RX_CAN_NORMAL_EXT, &rxFrame[instance]);
	}
    return (int)ret;
}






// return: 0 indicates success, others indicates failed
int flexcan_deinit(uint8_t instance)
{
    status_t ret;
    FLEXCAN_DRV_Reset(instance);
    ret = FLEXCAN_DRV_Deinit(instance);
    can_flexcan_State[instance].callback = NULL;
    return (int)ret;
}

uint8_t flexcan_set_baud(uint8_t instance, uint32_t baud)
{
    flexcan_time_segment_t bitrate[6] =
    {
        {7, 3, 2, 9, 1 },    // 50K      81.25%
        {7, 3, 2, 4, 1 },    // 100K     81.25%
        {7, 3, 2, 3, 1 },    // 125K     81.25%
        {7, 3, 2, 1, 1 },    // 250K     81.25%
        {7, 3, 2, 0, 1 },    // 500K     81.25%
        {2, 1, 1, 0, 1 },    // 1M       75%
    };
    uint8_t tmp = 0;

    if (50000 == baud)
    {
        tmp = 0;
    }
    else if (100000 == baud)
    {
        tmp = 1;
    }
    else if (125000 == baud)
    {
        tmp = 2;
    }
    else if (250000 == baud)
    {
        tmp = 3;
    }
    else if (1000000 == baud)
    {
        tmp = 5;
    }
    else
    {
        tmp = 4;
    }

    FLEXCAN_DRV_SetBitrate(instance, &bitrate[tmp]);
    return 0;
}

int flexcan_tx_mb_no_block(uint8_t instance, uint8_t *bufferPtr, uint8_t mbIdx)
{
    DEV_ASSERT(instance < NATIVE_CAN_MAX);
    DEV_ASSERT(mbIdx <= TX_CAN_BUFF_MAX);

    CAN_SEND_MSG *can_msg;
    can_msg = (CAN_SEND_MSG *) bufferPtr;
    flexcan_data_info_t dataInfo =
    {
        .data_length = can_msg->DLC,
        .msg_id_type = can_msg->isEID,
        .enable_brs = can_msg->BRS,
        .fd_enable = can_msg->canFD,
        .fd_padding = can_msg->PAD,
        .is_remote = can_msg->isRTR,
    };

    return FLEXCAN_DRV_Send(instance, mbIdx, &dataInfo, can_msg->MsgID, can_msg->Data);
}

void flexcan_tx_buff_no_block(uint8_t instance, uint8_t *bufferPtr)
{
    DEV_ASSERT(instance < NATIVE_CAN_MAX);

    uint8_t i;
    CAN_SEND_MSG *can_msg;

    can_msg = (CAN_SEND_MSG *) bufferPtr;
    flexcan_data_info_t dataInfo =
    {
        .data_length = can_msg->DLC,
        .msg_id_type = can_msg->isEID,
        .enable_brs = can_msg->BRS,
        .fd_enable = can_msg->canFD,
        .fd_padding = 0U,
        .is_remote = can_msg->isRTR,
    };

    for (i = TX_CAN_GENERAL1; i <= TX_CAN_BUFF_MAX; i++)
    {
        if (FLEXCAN_DRV_GetTransferStatus(instance, i) == STATUS_SUCCESS)
        {
            FLEXCAN_DRV_Send(instance, i, &dataInfo, can_msg->MsgID, can_msg->Data);
           // txComplete[instance] &= ~(0x1U << i);
            break;
        }
    }
}

uint8_t flexcan_tx_mb_block(uint8_t instance, uint8_t *bufferPtr, uint8_t mbIdx, uint32_t timeout)
{
    DEV_ASSERT(instance < NATIVE_CAN_MAX);
    DEV_ASSERT(mbIdx <= TX_CAN_BUFF_MAX);

    CAN_SEND_MSG *can_msg;
    can_msg = (CAN_SEND_MSG *) bufferPtr;

    flexcan_data_info_t dataInfo =
    {
        .data_length = can_msg->DLC,
        .msg_id_type = can_msg->isEID,
        .enable_brs = can_msg->BRS,
        .fd_enable = can_msg->canFD,
        .fd_padding = can_msg->PAD,
        .is_remote = can_msg->isRTR,
    };

    return (uint8_t) FLEXCAN_DRV_SendBlocking(instance, mbIdx, &dataInfo, can_msg->MsgID,
            can_msg->Data, timeout);
}

bool flexcan_get_tx_status(uint8_t instance, uint8_t mbIdx)
{
    DEV_ASSERT(instance < NATIVE_CAN_MAX);
    DEV_ASSERT(mbIdx <= TX_CAN_BUFF_MAX);

    if (txComplete[instance] & (0x1U << mbIdx))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*****************************************************************************
 *   Function   :    flexcan_get_errstatus
 *   Description:    ?ï¿½å–CAN?ï¿½çº¿?ï¿½ï¿½ *   Inputs     :
 *   Outputs    :
 ****************************************************************************/
unsigned int flexcan_get_errstatus(uint8_t instance)
{
    return (kFLEXCAN_ErrorIntFlag & FLEXCAN_HAL_GetErrStatus(g_flexcanBase[instance]));
}

/*****************************************************************************
 *   Function   :    flexcan_clear_errstatus
 *   Description:    æ¸???AN?ï¿½çº¿?ï¿½ï¿½ *   Inputs     :
 *   Outputs    :
 ****************************************************************************/
void flexcan_clear_errstatus(uint8_t instance)
{
    FLEXCAN_HAL_ClearErrIntStatusFlag(g_flexcanBase[instance]);
}

/*****************************************************************************
 *   Function   :    flexcan_set_errint
 *   Description:    ï¿???/?ï¿½ï¿½ *   Inputs     :
 *   Outputs    :
 ****************************************************************************/
void flexcan_set_errint(uint8_t instance, bool enable)
{
    FLEXCAN_HAL_SetErrIntCmd(g_flexcanBase[instance], FLEXCAN_INT_ERR, enable);
}

/*****************************************************************************
 *   Function   :    flexcan_ecallbl_ctrl
 *   Description:    ECALL_BL 
 *   Outputs    :
 ****************************************************************************/
void flexcan_ecallbl_ctrl(CAN_MSG msg)
{
   	if(msg.MsgID == 0x339)
	{
		if(msg.Data[2] & 0x02)   
		{
			GPIO_HAL_WritePin(GP_K5,15,1);  
		}
		else
		{
			GPIO_HAL_WritePin(GP_K5,15,0); 
		}
	}
}

