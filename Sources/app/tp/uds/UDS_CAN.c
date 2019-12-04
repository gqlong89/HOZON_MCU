/*****************************************************************************
 *   Include Files
 *****************************************************************************/

#include <app/tp/uds/UDS_Config.h>
#include <can_api.h>
#include <mid/kernel/task.h>
#include <sys/_stdint.h>

extern uint8_t task_uds_id ;

/*****************************************************************************
 *   Function   :    UDS_CAN_Init
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
void UDS_CAN_Init(uds_t *tUDS)
{
    tUDS->canReadBuf_Header = 0;
    tUDS->canReadBuf_Tail = 0;
    tUDS->canReadBuf_ValidCnt = 0;
    tUDS->enable = 1;
    #if 0
    can_init(tUDS->canport, tUDS->u32CANBaud);
    #endif
}

/*****************************************************************************
 *   Function   :    UDS_CANSend
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 ****************************************************************************/
uint32_t UDS_CANSend(uds_t *tUDS, uint32_t u32CANID, uint8_t u8Can_Dl, uint8_t *Ar_u8CANData)
{
    uint8_t Ret = 1;
    uint8_t i = 0;
    //   uint8_t u8Dlc;
    CAN_SEND_MSG canMsg;

    canMsg.DLC = u8Can_Dl;
    canMsg.MsgID = u32CANID & 0x1FFFFFFF;
    canMsg.isRTR = 0;
    canMsg.isEID = (u32CANID & (1u << 31)) ? 1 : 0;
    
    if(tUDS->canport == CAN_PORT0)
    {
        canMsg.BRS = 1;
        canMsg.canFD = 1;
    }
    else
    {
        canMsg.canFD = 0;
    }

    for (i = 0; i < u8Can_Dl; i++)
    {
        canMsg.Data[i] = Ar_u8CANData[i];
    }

    extern uint8_t mailrange[5][3];

    Ret = can_send(tUDS->canport, mailrange[tUDS->canport][0], &canMsg);

    return (Ret);
}

/*****************************************************************************
 *   Function   :    UDS_CANRead
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
UDS_CANMsg_T *UDS_CANRead(uds_t *tUDS)
{
    UDS_CANMsg_T *pt_UDSCANMsg = NULL_PTR;

    if (tUDS->canReadBuf_ValidCnt > 0)
    {
        pt_UDSCANMsg = &tUDS->canReadBuf[tUDS->canReadBuf_Tail];
        tUDS->canReadBuf_Tail = (tUDS->canReadBuf_Tail + 1) % CAN_RX_BUFF_MAX;
        tUDS->canReadBuf_ValidCnt--;
        kernelSetTaskEvent(task_uds_id, TASK_SYS_EVENT_READ);
    }

    return (pt_UDSCANMsg);
}

/*****************************************************************************
 *   Function   :    UDS_CANAccept_Int
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
uint32_t UDS_CANAccept(uint32_t canport, CAN_MSG *tCANMsg)
{
    uint32_t i = 0;
    uds_t *tUDS = NULL_PTR;

    #if ( UDS_TYPE_SEL == UDS_TYPE_SERVER )

    if ((tUDS_Server.enable == 1) && (canport == tUDS_Server.canport)
        && ((tCANMsg->MsgID == tUDS_Server.u32Phy_CANID) || (tCANMsg->MsgID == tUDS_Server.u32Fun_CANID)))
    {
        tUDS = &tUDS_Server;
    }

    #elif ( UDS_TYPE_SEL == UDS_TYPE_CLIENT )

    if ((tUDS_Client.enable == 1) && (canport == tUDS_Client.canport)
        && (tCANMsg->MsgID == tUDS_Client.u32Respond_CANID))
    {
        tUDS = &tUDS_Client;
    }

    #elif ( UDS_TYPE_SEL == UDS_TYPE_SERVER_CLIENT )

    if (tUDS_Server.enable == 1)
    {
        if ((canport == tUDS_Server.canport)
            && ((tCANMsg->MsgID == tUDS_Server.u32Phy_CANID)
                || (tCANMsg->MsgID == tUDS_Server.u32Fun_CANID)))
        {
            tUDS = &tUDS_Server;
        }
    }

    if (tUDS_Client.enable == 1)
    {
        if ((canport == tUDS_Client.canport)
            && (tCANMsg->MsgID == tUDS_Client.u32Respond_CANID))
        {
            tUDS = &tUDS_Client;
        }
    }

    #endif

    if (tUDS == NULL_PTR)
    {
        return 0;
    }

    tUDS->canReadBuf[tUDS->canReadBuf_Header].u32CANID = tCANMsg->MsgID;
    tUDS->canReadBuf[tUDS->canReadBuf_Header].u8DLC = tCANMsg->dlc;

    for (i = 0; i < tCANMsg->dlc; i++)
    {
        tUDS->canReadBuf[tUDS->canReadBuf_Header].Ar_u8CANData[i] = tCANMsg->Data[i];
    }

    tUDS->canReadBuf_Header = (tUDS->canReadBuf_Header + 1) % CAN_RX_BUFF_MAX;
    tUDS->canReadBuf_ValidCnt = (tUDS->canReadBuf_ValidCnt + 1) % CAN_RX_BUFF_MAX;
    kernelSetTaskEvent(task_uds_id, TASK_SYS_EVENT_READ);

    return 1;
}
