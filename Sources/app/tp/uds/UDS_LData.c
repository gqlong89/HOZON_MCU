/*****************************************************************************
 *   Include Files
 *****************************************************************************/

#include <app/tp/uds/UDS_Config.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <osif.h>
#include <sys/_stdint.h>

/*****************************************************************************
 *   Global Data Declarations
 *****************************************************************************/

/*****************************************************************************
 *   Local Data Definitions
 *****************************************************************************/
#define      UDS_FF_PUD_DLC_MAX         (6)
#define      UDS_SF_PDU_DLC_MAX         (7)
#define      CAN_DLC                    (8)



/*****************************************************************************
 * Note: define the Frame protocol control information in uds netlayer
 * SF_PCI  : single frame
 * FF_PCI  : frist frame in the multiple frame
 * CF_PCI  : consecutive frame in the multiple frame
 * FC_PCI  : flow control
 *****************************************************************************/
#define  SF_PCI          (0x00)
#define  FF_PCI          (0x10)
#define  CF_PCI          (0x20)
#define  FC_PCI          (0x30)

/*****************************************************************************
 * Note: define the flow state of the FlowControl Frame
 * CTS  :  ContinusToSend
 * WT   :  Wait
 * OVFLW:  Overflow
 *****************************************************************************/
#define  FC_FS_CTS       (0x00)
#define  FC_FS_WT        (0x01)
#define  FC_FS_OVFLW     (0x02)

/*****************************************************************************
 *   Local Function Declarations
 *****************************************************************************/
uint32_t SF_Request__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t *pu8_Data);
uint32_t FF_Request__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t *pu8_Data);

static void SF_Indication__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t u8TOTAL_CANDL,
                            uint8_t *pu8_Data);
static void FF_Indication__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t u8TOTAL_CANDL,
                            uint8_t *pu8_Data);
static void FC_Indication__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t *pu8_Data);
static void CF_Indication__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t u8TOTAL_CANDL,
                            uint8_t *pu8_Data);

static void SetNLTimer__(uds_t *tUDS, uint8_t u8TimerType, uint8_t u8Switch);

static void NTimeout_Bs__(void *para);
static void NTimeout_Cr__(void *para);

/*****************************************************************************
 *   Function   :    L_Data_Init
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
void L_Data_Init(uds_t *tUDS)
{
    uint8_t u8i;

    tUDS->LData_FS_Ind = FC_FS_WT;
    tUDS->LData_eFrameStatus = NL_FRAME_IDLE;
    tUDS->LData_eWorkStatus = NL_IDLE_STATUS;

    for (u8i = 0; u8i < NL_Timer_MAX; u8i++)
    {
        tUDS->tNLTimer[u8i].u8SwitchValue = 0;
        tUDS->tNLTimer[u8i].u64StartTimeValue = 0;
    }

    tUDS->tNLTimer[NL_TimeBs].u16TimeValue = tUDS->u16N_Bs;
    tUDS->tNLTimer[NL_TimeCr].u16TimeValue = tUDS->u16N_Cr;

    tUDS->tNLTimer[NL_TimeBs].u8NResult = N_TIMEOUT_Bs;
    tUDS->tNLTimer[NL_TimeCr].u8NResult = N_TIMEOUT_Cr;

    tUDS->tNLTimer[NL_TimeBs].func_NTimeout = NTimeout_Bs__;
    tUDS->tNLTimer[NL_TimeCr].func_NTimeout = NTimeout_Cr__;

}

/*****************************************************************************
 *   Function   :    L_Data_request
 *   Description:    This function is called by Net Layer for Server Request
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
/*
uint32_t L_Data_request(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t *pu8_Data)
{
    uint32_t Ret = 1;
    uint16_t u16i;

    if ((tUDS->LData_eWorkStatus == NL_IDLE_STATUS)
        || (tUDS->LData_eWorkStatus == NL_SEGMENT_READ_STATUS))
    {
        tUDS->u32ReqCANID = u32AI;

        if (u32PDU_DLC <= UDS_SF_PDU_DLC_MAX)
        {
            Ret = SF_Request__(tUDS, u32AI, u32PDU_DLC, pu8_Data);
        }
        else
        {
            for (u16i = 0; u16i < u32PDU_DLC; u16i++)
            {
                tUDS->PDU_MF[u16i] = pu8_Data[u16i];
            }

            Ret = FF_Request__(tUDS, u32AI, u32PDU_DLC, tUDS->PDU_MF);
        }
    }

    return (Ret);
}
*/
/*****************************************************************************
 *   Function   :    L_Data_Indication
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    None
 *****************************************************************************/


void L_Data_Indication(uds_t *tUDS)
{
    uint8_t ii = 0;
    uint8_t u8PCI;
    uint32_t u32PDU_DLC;
    uint8_t  u8TOTAL_CANDL;
    UDS_CANMsg_T *pt_UDSCANMsg = NULL_PTR;

    pt_UDSCANMsg = UDS_CANRead(tUDS);

    if (pt_UDSCANMsg != NULL_PTR)
    {
        u8TOTAL_CANDL = pt_UDSCANMsg->u8DLC;
        u8PCI = pt_UDSCANMsg->Ar_u8CANData[0] & 0xF0;
        u32PDU_DLC = pt_UDSCANMsg->Ar_u8CANData[0] & 0x0F;
        taskPrintf(TASK_LEVEL_DEBUG, "reciver data:");

        for (ii = 0; ii < u8TOTAL_CANDL; ii++)
        {
            taskPrintf(TASK_LEVEL_DEBUG, "0x%x ", pt_UDSCANMsg->Ar_u8CANData[ii]);
        }

        taskPrintf(TASK_LEVEL_DEBUG, "\r\n");

        switch (u8PCI)
        {
            case SF_PCI :
                if ((((tUDS->LData_eFrameStatus == NL_FRAME_IDLE) || (tUDS->LData_eFrameStatus == SF_REQ_STATE)
                      || (tUDS->LData_eFrameStatus == MF_OVER_REQ_STATE) || (tUDS->LData_eFrameStatus == FC_REQ_STATE)
                      || (tUDS->LData_eFrameStatus == SF_IND_STATE) || (tUDS->LData_eFrameStatus == MF_OVER_IND_STATE))
                     && (tUDS->mode == UDS_TYPE_SERVER)) || (tUDS->mode == UDS_TYPE_CLIENT))
                {
                    SF_Indication__(tUDS , pt_UDSCANMsg->u32CANID, u32PDU_DLC, u8TOTAL_CANDL ,
                                    pt_UDSCANMsg->Ar_u8CANData);
                }

                break;

            case FF_PCI :
                if ((((tUDS->LData_eFrameStatus == NL_FRAME_IDLE) || (tUDS->LData_eFrameStatus == SF_REQ_STATE)
                      || (tUDS->LData_eFrameStatus == MF_OVER_REQ_STATE) || (tUDS->LData_eFrameStatus == FC_REQ_STATE)
                      || (tUDS->LData_eFrameStatus == SF_IND_STATE) || (tUDS->LData_eFrameStatus == MF_OVER_IND_STATE))
                     && (pt_UDSCANMsg->u32CANID == tUDS->u32Phy_CANID) && (tUDS->mode == UDS_TYPE_SERVER))
                    || (tUDS->mode == UDS_TYPE_CLIENT))
                {
                    u32PDU_DLC = (u32PDU_DLC << 8) + pt_UDSCANMsg->Ar_u8CANData[1];
                    FF_Indication__(tUDS , pt_UDSCANMsg->u32CANID, u32PDU_DLC , u8TOTAL_CANDL ,
                                    pt_UDSCANMsg->Ar_u8CANData);
                }

                break;

            case CF_PCI :
                if ((tUDS->mode == UDS_TYPE_SERVER) && (pt_UDSCANMsg->u32CANID != tUDS->u32Phy_CANID))
                {
                    break ;
                }

                if ((tUDS->LData_eFrameStatus == FC_REQ_STATE) || (tUDS->LData_eFrameStatus == CF_IND_STATE) ||
                    (tUDS->LData_eFrameStatus == SF_REQ_STATE) || (tUDS->LData_eFrameStatus == SF_IND_STATE))
                {

                    CF_Indication__(tUDS , pt_UDSCANMsg->u32CANID, u32PDU_DLC , u8TOTAL_CANDL,
                                    pt_UDSCANMsg->Ar_u8CANData);

                }

                break;

            case FC_PCI :
                if ((tUDS->mode == UDS_TYPE_SERVER) && (pt_UDSCANMsg->u32CANID != tUDS->u32Phy_CANID))
                {
                    break ;
                }

                if ((tUDS->LData_eFrameStatus == FF_REQ_STATE) ||
                    (tUDS->LData_eFrameStatus == MF_BLOCK_OVER_REQ_STATE) ||
                    (tUDS->LData_eFrameStatus == FC_WAIT_STATE))
                {
                    // if(Uds_Run_Mode == UDS_RUN_CANFD)
                    //  FC_Indication__(tUDS , pt_UDSCANMsg->u32CANID, u32PDU_DLC , u8TOTAL_CANDL,pt_UDSCANMsg->Ar_u8CANData);
                    // else if(Uds_Run_Mode == UDS_RUN_CAN)
                    FC_Indication__(tUDS , pt_UDSCANMsg->u32CANID, u32PDU_DLC , pt_UDSCANMsg->Ar_u8CANData);
                }

                break;
        }
    }

}

/*****************************************************************************
 *   Function   :    LData_TimerManager
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    void
 *   Notes      :
 *****************************************************************************/
void LData_TimerManager(uds_t *tUDS)
{
    uint8_t u8i;

    for (u8i = 0; u8i < NL_Timer_MAX; u8i++)
    {
        if ((tUDS->tNLTimer[u8i].u8SwitchValue == 1)
            &&
            (driverGetUptime() * 10 - tUDS->tNLTimer[u8i].u64StartTimeValue
             > tUDS->tNLTimer[u8i].u16TimeValue))
        {
            tUDS->tNLTimer[u8i].u8SwitchValue = 0;
            tUDS->tNLTimer[u8i].func_NTimeout(tUDS);
            N_USData_confirm(tUDS, tUDS->u32ReqCANID, tUDS->tNLTimer[u8i].u8NResult);
        }
    }
}
/*****************************************************************************
 *   Function   :    CF_Request
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    void
 *   Notes      :
 *****************************************************************************/
/*
void CF_Request_CANFD(uds_t *tUDS)
{
    uint8_t u8i;
    uint8_t Ar_u8CanData[64];
    uint32_t u32DataIndex;
    uint8_t  Can_Dl;
    if ((tUDS->LData_FS_Ind == FC_FS_CTS) && (tUDS->LData_MF_DataIndex < tUDS->LData_PDU_DLC))
    {
        if ((tUDS->LData_BlockCnt < tUDS->LData_BS_Ind) || (tUDS->LData_BS_Ind == 0))
        {
            Ar_u8CanData[0] = CF_PCI + (tUDS->LData_u8SN & 0x0F);
            u32DataIndex = tUDS->LData_MF_DataIndex;
            for (u8i = 1; u8i < 64; u8i++)
            {
                if (u32DataIndex < tUDS->LData_PDU_DLC)
                {
                    Ar_u8CanData[u8i] = tUDS->PDU_MF[u32DataIndex++];
                }
                else if( u8i < 8)
                {
                  Can_Dl = 8;
                  for(; u8i < 8; u8i++)
                    Ar_u8CanData[u8i] = tUDS->u8FillValue;
                    break;
                }
                else if( u8i < 12)
                {
                     Can_Dl = 12;
                    for(; u8i < 12; u8i++)
                    Ar_u8CanData[u8i] = tUDS->u8FillValue;
                    break;
                }
                else if( u8i < 16)
                {
                    Can_Dl = 16;
                    for(; u8i < 16; u8i++)
                    Ar_u8CanData[u8i] = tUDS->u8FillValue;
                    break;
                }
                else if( u8i < 20)
                {
                     Can_Dl = 20;
                    for(; u8i < 20; u8i++)
                    Ar_u8CanData[u8i] = tUDS->u8FillValue;
                    break;
                }
                else if( u8i < 24)
                {
                     Can_Dl = 24;
                    for(; u8i < 24; u8i++)
                    Ar_u8CanData[u8i] = tUDS->u8FillValue;
                    break;
                }
                else if( u8i < 32)
                {
                     Can_Dl = 32;
                    for(; u8i < 32; u8i++)
                    Ar_u8CanData[u8i] = tUDS->u8FillValue;
                    break;
                }
                else if( u8i < 48)
                {
                    Can_Dl = 48;
                    for(; u8i < 48; u8i++)
                    Ar_u8CanData[u8i] = tUDS->u8FillValue;
                    break;
                }
                else
                    Ar_u8CanData[u8i] = tUDS->u8FillValue;

                Can_Dl = 64;
            }

            if (0 == UDS_CANSend(tUDS, tUDS->u32ReqCANID, Can_Dl, Ar_u8CanData))
            {
                tUDS->LData_u8SN++;
                tUDS->LData_MF_DataIndex += (Can_Dl-1);
                tUDS->LData_BlockCnt++;
                tUDS->LData_eFrameStatus = CF_REQ_STATE;

                if (tUDS->LData_MF_DataIndex >= tUDS->LData_PDU_DLC)
                {
                    UDS_TimerClose(tUDS);
                    tUDS->LData_PDU_DLC = 0;
                    tUDS->LData_FS_Ind = FC_FS_WT;
                    tUDS->LData_eFrameStatus = MF_OVER_REQ_STATE;
                    tUDS->LData_eWorkStatus = NL_IDLE_STATUS;
                    N_USData_confirm(tUDS, tUDS->u32ReqCANID, N_OK);
                    return;
                }

                if ((tUDS->LData_BlockCnt >= tUDS->LData_BS_Ind) && (tUDS->LData_BS_Ind != 0))
                {
                    UDS_TimerClose(tUDS);
                    tUDS->LData_FS_Ind = FC_FS_WT;
                    tUDS->LData_eWorkStatus = NL_IDLE_STATUS;
                    tUDS->LData_eFrameStatus = MF_BLOCK_OVER_REQ_STATE;
                    SetNLTimer__(tUDS, NL_TimeBs, 1);
                }
            }
        }
    }
}
*/
/*****************************************************************************
 *   Function   :    CF_Request
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    void
 *   Notes      :
 *****************************************************************************/
void CF_Request(uds_t *tUDS)
{
    if(tUDS->canport == CAN_PORT0)
    {

    //#if UDSRUNMODE
    uint8_t u8i;
    uint8_t Ar_u8CanData[64];
    uint32_t u32DataIndex;
    uint8_t  Can_Dl;

        if ((tUDS->LData_FS_Ind == FC_FS_CTS) && (tUDS->LData_MF_DataIndex < tUDS->LData_PDU_DLC))
        {
            if ((tUDS->LData_BlockCnt < tUDS->LData_BS_Ind) || (tUDS->LData_BS_Ind == 0))
            {
                Ar_u8CanData[0] = CF_PCI + (tUDS->LData_u8SN & 0x0F);
                u32DataIndex = tUDS->LData_MF_DataIndex;

                for (u8i = 1; u8i < 64; u8i++)
                {
                    if (u32DataIndex < tUDS->LData_PDU_DLC)
                    {
                        Ar_u8CanData[u8i] = tUDS->PDU_MF[u32DataIndex++];
                    }
                    else
                    {
                        Ar_u8CanData[u8i] = tUDS->u8FillValue;
                    }

                    Can_Dl = 64;
                }

                if (0 == UDS_CANSend(tUDS, tUDS->u32ReqCANID, Can_Dl, Ar_u8CanData))
                {
                    tUDS->LData_u8SN++;
                    tUDS->LData_MF_DataIndex += (Can_Dl - 1);
                    tUDS->LData_BlockCnt++;
                    tUDS->LData_eFrameStatus = CF_REQ_STATE;

                    if (tUDS->LData_MF_DataIndex >= tUDS->LData_PDU_DLC)
                    {
                        UDS_TimerClose(tUDS);
                        tUDS->LData_PDU_DLC = 0;
                        tUDS->LData_FS_Ind = FC_FS_WT;
                        tUDS->LData_eFrameStatus = MF_OVER_REQ_STATE;
                        tUDS->LData_eWorkStatus = NL_IDLE_STATUS;
                        N_USData_confirm(tUDS, tUDS->u32ReqCANID, N_OK);
                        return;
                    }

                    if ((tUDS->LData_BlockCnt >= tUDS->LData_BS_Ind) && (tUDS->LData_BS_Ind != 0))
                    {
                        UDS_TimerClose(tUDS);
                        tUDS->LData_FS_Ind = FC_FS_WT;
                        tUDS->LData_eWorkStatus = NL_IDLE_STATUS;
                        tUDS->LData_eFrameStatus = MF_BLOCK_OVER_REQ_STATE;
                        SetNLTimer__(tUDS, NL_TimeBs, 1);
                    }
                }
            }
        }
    }
    else
    {
        //#else

        uint8_t u8i;
        uint8_t Ar_u8CanData[8];
        uint32_t u32DataIndex;

        if ((tUDS->LData_FS_Ind == FC_FS_CTS) && (tUDS->LData_MF_DataIndex < tUDS->LData_PDU_DLC))
        {
            if ((tUDS->LData_BlockCnt < tUDS->LData_BS_Ind) || (tUDS->LData_BS_Ind == 0))
            {
                Ar_u8CanData[0] = CF_PCI + (tUDS->LData_u8SN & 0x0F);
                u32DataIndex = tUDS->LData_MF_DataIndex;

                for (u8i = 1; u8i < 8; u8i++)
                {
                    if (u32DataIndex < tUDS->LData_PDU_DLC)
                    {
                        Ar_u8CanData[u8i] = tUDS->PDU_MF[u32DataIndex++];
                    }
                    else
                    {
                        Ar_u8CanData[u8i] = tUDS->u8FillValue;
                    }
                }

                if (0 == UDS_CANSend(tUDS, tUDS->u32ReqCANID, CAN_DLC, Ar_u8CanData))
                {
                    tUDS->LData_u8SN++;
                    tUDS->LData_MF_DataIndex += 7;
                    tUDS->LData_BlockCnt++;
                    tUDS->LData_eFrameStatus = CF_REQ_STATE;

                    if (tUDS->LData_MF_DataIndex >= tUDS->LData_PDU_DLC)
                    {
                        UDS_TimerClose(tUDS);
                        tUDS->LData_PDU_DLC = 0;
                        tUDS->LData_FS_Ind = FC_FS_WT;
                        tUDS->LData_eFrameStatus = MF_OVER_REQ_STATE;
                        tUDS->LData_eWorkStatus = NL_IDLE_STATUS;
                        N_USData_confirm(tUDS, tUDS->u32ReqCANID, N_OK);
                        return;
                    }

                    if ((tUDS->LData_BlockCnt >= tUDS->LData_BS_Ind) && (tUDS->LData_BS_Ind != 0))
                    {
                        UDS_TimerClose(tUDS);
                        tUDS->LData_FS_Ind = FC_FS_WT;
                        tUDS->LData_eWorkStatus = NL_IDLE_STATUS;
                        tUDS->LData_eFrameStatus = MF_BLOCK_OVER_REQ_STATE;
                        SetNLTimer__(tUDS, NL_TimeBs, 1);
                    }
                }
            }
        }

        //#endif
    }
}
/*****************************************************************************
 *   Function   :    SF_Request_CANFD__
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    None
 *****************************************************************************/
/*
uint32_t SF_Request_CANFD__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t *pu8_Data)
{
   uint32_t Ret = 1;
   uint8_t u8i;
   uint8_t Ar_u8CanData[64];
   uint8_t  u8Can_Dl;
 //  uint8_t  u8PDU_Dlc;

   if(u32PDU_DLC  < 8)
   {
     u8Can_Dl = 8;
     Ar_u8CanData[0] = (uint8_t)u32PDU_DLC;
    for (u8i = 0; u8i < u32PDU_DLC; u8i++)
    {
         Ar_u8CanData[u8i + 1] = pu8_Data[u8i];
    }
    for (; u8i < (u8Can_Dl-1); u8i++)
    {
         Ar_u8CanData[u8i + 1] = tUDS->u8FillValue;
    }
   }
   else
   {
        Ar_u8CanData[0] = 0;
        Ar_u8CanData[1] = (uint8_t)u32PDU_DLC;
        if(u32PDU_DLC <= 10)
           u8Can_Dl = 12;
        else if(u32PDU_DLC <= 14)
           u8Can_Dl = 16;
        else if(u32PDU_DLC <= 18)
           u8Can_Dl = 20;
        else if(u32PDU_DLC <= 22)
           u8Can_Dl = 24;
        else if(u32PDU_DLC <= 30)
           u8Can_Dl = 32;
        else if(u32PDU_DLC <= 46)
           u8Can_Dl = 48;
        else if(u32PDU_DLC <= 62)
           u8Can_Dl = 64;
       for (u8i = 0; u8i < u32PDU_DLC; u8i++)
       {
           Ar_u8CanData[u8i + 2] = pu8_Data[u8i];
       }
       for (; u8i < (u8Can_Dl-2); u8i++)
       {
           Ar_u8CanData[u8i + 2] = tUDS->u8FillValue;
       }
   }
   if (0 == UDS_CANSend(tUDS, u32AI,u8Can_Dl, Ar_u8CanData))
   {
       tUDS->LData_eFrameStatus = SF_REQ_STATE;
       Ret = 0;
       N_USData_confirm(tUDS, tUDS->u32ReqCANID, N_OK);
       taskPrintf(TASK_LEVEL_DEBUG, "LData.SF_Req");
   }
   return (Ret);
}
*/
/*****************************************************************************
 *   Function   :    SF_Request__
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    None
 *****************************************************************************/
uint32_t SF_Request__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t *pu8_Data)
{
    //#if UDSRUNMODE
    if(tUDS->canport == CAN_PORT0)
    {
        uint32_t Ret = 1;
        uint8_t u8i;
        uint8_t Ar_u8CanData[64];
        uint8_t  u8Can_Dl;
        //  uint8_t  u8PDU_Dlc;

        u8Can_Dl = 64;
        Ar_u8CanData[0] = (uint8_t)u32PDU_DLC;

        for (u8i = 0; u8i < u32PDU_DLC; u8i++)
        {
            Ar_u8CanData[u8i + 1] = pu8_Data[u8i];
        }

        for (; u8i < (u8Can_Dl - 1); u8i++)
        {
            Ar_u8CanData[u8i + 1] = tUDS->u8FillValue;
        }


        if (0 == UDS_CANSend(tUDS, u32AI, u8Can_Dl, Ar_u8CanData))
        {
            tUDS->LData_eFrameStatus = SF_REQ_STATE;
            Ret = 0;
            N_USData_confirm(tUDS, tUDS->u32ReqCANID, N_OK);
            taskPrintf(TASK_LEVEL_DEBUG, "LData.SF_Req");
        }

        return (Ret);
    }
    else
    {
        //#else
        uint32_t Ret = 1;
        uint8_t u8i;
        uint8_t Ar_u8CanData[8];

        Ar_u8CanData[0] = (uint8_t) u32PDU_DLC;

        for (u8i = 0; u8i < u32PDU_DLC; u8i++)
        {
            Ar_u8CanData[u8i + 1] = pu8_Data[u8i];
        }

        for (; u8i < 7; u8i++)
        {
            Ar_u8CanData[u8i + 1] = tUDS->u8FillValue;
        }

        if (0 == UDS_CANSend(tUDS, u32AI, CAN_DLC, Ar_u8CanData))
        {
            tUDS->LData_eFrameStatus = SF_REQ_STATE;
            Ret = 0;
            N_USData_confirm(tUDS, tUDS->u32ReqCANID, N_OK);
            taskPrintf(TASK_LEVEL_DEBUG, "LData.SF_Req");
        }

        return (Ret);
        //#endif
    }
}
/*****************************************************************************
 *   Function   :    FF_Request_CANFD__
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    void
 *   Notes      :
 *****************************************************************************/
/*
uint32_t FF_Request_CANFD__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t *pu8_Data)
{
   uint32_t Ret = 1;
   uint8_t u8i;
   uint8_t Ar_u8CanData[64];
   if( u32PDU_DLC < 4096 )
   {
       Ar_u8CanData[0] = FF_PCI + (uint8_t)(u32PDU_DLC >> 8);
       Ar_u8CanData[1] = (uint8_t)(u32PDU_DLC & 0x00FF);
       for (u8i = 0; u8i < 62; u8i++)
       {
           Ar_u8CanData[u8i + 2] = pu8_Data[u8i];
       }

       if (0 == UDS_CANSend(tUDS, u32AI,64, Ar_u8CanData))
       {
           tUDS->LData_PDU_DLC = u32PDU_DLC;
           tUDS->LData_MF_DataIndex = 62;
           tUDS->LData_u8SN = 1;
           tUDS->LData_eFrameStatus = FF_REQ_STATE;
           SetNLTimer__(tUDS, NL_TimeBs, 1);
           Ret = 0;
           taskPrintf(TASK_LEVEL_DEBUG, "LData.FF_Req");
        }

    }
    else
    {
        Ar_u8CanData[0] = FF_PCI ;
        Ar_u8CanData[1] = 0;
        Ar_u8CanData[2] = (uint8_t)(u32PDU_DLC >> 24);
        Ar_u8CanData[3] = (uint8_t)((u32PDU_DLC >> 16)&0x000000FF);
        Ar_u8CanData[4] = (uint8_t)((u32PDU_DLC >> 8)&0x000000FF);
        Ar_u8CanData[5] = (uint8_t)(u32PDU_DLC&0x000000FF);
        for (u8i = 0; u8i < 62; u8i++)
        {
            Ar_u8CanData[u8i + 6] = pu8_Data[u8i];
        }
        if (0 == UDS_CANSend(tUDS, u32AI, 64, Ar_u8CanData))
        {
            tUDS->LData_PDU_DLC = u32PDU_DLC;
            tUDS->LData_MF_DataIndex = 58;
            tUDS->LData_u8SN = 1;
            tUDS->LData_eFrameStatus = FF_REQ_STATE;
            SetNLTimer__(tUDS, NL_TimeBs, 1);
            Ret = 0;
            taskPrintf(TASK_LEVEL_DEBUG, "LData.FF_Req");
        }
    }

    return (Ret);
}
*/
/*****************************************************************************
 *   Function   :    FF_Request__
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    void
 *   Notes      :
 *****************************************************************************/
uint32_t FF_Request__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t *pu8_Data)
{
    if(tUDS->canport == CAN_PORT0)
    {
        //#if UDSRUNMODE
        uint32_t Ret = 1;
        uint8_t u8i;
        uint8_t Ar_u8CanData[64];

        if (u32PDU_DLC < 4096)
        {
            Ar_u8CanData[0] = FF_PCI + (uint8_t)(u32PDU_DLC >> 8);
            Ar_u8CanData[1] = (uint8_t)(u32PDU_DLC & 0x00FF);

            for (u8i = 0; u8i < 62; u8i++)
            {
                Ar_u8CanData[u8i + 2] = pu8_Data[u8i];
            }

            if (0 == UDS_CANSend(tUDS, u32AI, 64, Ar_u8CanData))
            {
                tUDS->LData_PDU_DLC = u32PDU_DLC;
                tUDS->LData_MF_DataIndex = 62;
                tUDS->LData_u8SN = 1;
                tUDS->LData_eFrameStatus = FF_REQ_STATE;
                SetNLTimer__(tUDS, NL_TimeBs, 1);
                Ret = 0;
                taskPrintf(TASK_LEVEL_DEBUG, "LData.FF_Req");
            }

        }
        else
        {
            Ar_u8CanData[0] = FF_PCI ;
            Ar_u8CanData[1] = 0;
            Ar_u8CanData[2] = (uint8_t)(u32PDU_DLC >> 24);
            Ar_u8CanData[3] = (uint8_t)((u32PDU_DLC >> 16) & 0x000000FF);
            Ar_u8CanData[4] = (uint8_t)((u32PDU_DLC >> 8) & 0x000000FF);
            Ar_u8CanData[5] = (uint8_t)(u32PDU_DLC & 0x000000FF);

            for (u8i = 0; u8i < 62; u8i++)
            {
                Ar_u8CanData[u8i + 6] = pu8_Data[u8i];
            }

            if (0 == UDS_CANSend(tUDS, u32AI, 64, Ar_u8CanData))
            {
                tUDS->LData_PDU_DLC = u32PDU_DLC;
                tUDS->LData_MF_DataIndex = 58;
                tUDS->LData_u8SN = 1;
                tUDS->LData_eFrameStatus = FF_REQ_STATE;
                SetNLTimer__(tUDS, NL_TimeBs, 1);
                Ret = 0;
                taskPrintf(TASK_LEVEL_DEBUG, "LData.FF_Req");
            }
        }

        return (Ret);
    }
    else
    {
        //#else

        uint32_t Ret = 1;
        uint8_t u8i;
        uint8_t Ar_u8CanData[8];

        Ar_u8CanData[0] = FF_PCI + (uint8_t)(u32PDU_DLC >> 8);
        Ar_u8CanData[1] = (uint8_t)(u32PDU_DLC & 0x00FF);

        for (u8i = 0; u8i < 6; u8i++)
        {
            Ar_u8CanData[u8i + 2] = pu8_Data[u8i];
        }

        if (0 == UDS_CANSend(tUDS, u32AI, CAN_DLC, Ar_u8CanData))
        {
            tUDS->LData_PDU_DLC = u32PDU_DLC;
            tUDS->LData_MF_DataIndex = 6;
            tUDS->LData_u8SN = 1;
            tUDS->LData_eFrameStatus = FF_REQ_STATE;
            SetNLTimer__(tUDS, NL_TimeBs, 1);
            Ret = 0;
            taskPrintf(TASK_LEVEL_DEBUG, "LData.FF_Req");
        }

        return (Ret);
        //#endif
    }
}

/*****************************************************************************
 *   Function   :    FC_Request_CANFD__
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    void
 *   Notes      :
 *****************************************************************************/
/*
static uint32_t FC_Request_CANFD__(uds_t *tUDS, uint8_t u8Fs, uint8_t u8Bs, uint8_t u8ST)
{
   uint8_t u8i;
   uint8_t u8Ar_SendData[64];
   uint32_t u32FC_CANID;
   uint32_t Ret = 1;

   u8Ar_SendData[0] = FC_PCI + (u8Fs & 0x0F);
   u8Ar_SendData[1] = u8Bs;
   u8Ar_SendData[2] = u8ST;

   if (tUDS->mode == UDS_TYPE_SERVER)
   {
       u32FC_CANID = tUDS->u32Respond_CANID;
   }
   else if (tUDS->mode == UDS_TYPE_CLIENT)
   {
       u32FC_CANID = tUDS->u32Phy_CANID;
   }
   else
   {
       return 1;
   }

   for (u8i = 3; u8i < 8; u8i++)
   {
       u8Ar_SendData[u8i] = tUDS->u8FillValue;
   }

   if (0 == UDS_CANSend(tUDS, u32FC_CANID, CAN_DLC, u8Ar_SendData))
   {
       tUDS->LData_eWorkStatus = NL_SEGMENT_READ_STATUS;
       tUDS->LData_eFrameStatus = FC_REQ_STATE;
       SetNLTimer__(tUDS, NL_TimeCr, 1);
       taskPrintf(TASK_LEVEL_DEBUG, "LData.FC_Req");
       Ret = 0;
   }
   else
   {
       N_USData_confirm(tUDS, u32FC_CANID, N_ERROR);
   }

   return Ret;
}
*/
/*****************************************************************************
 *   Function   :    FC_Request__
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    void
 *   Notes      :
 *****************************************************************************/
static uint32_t FC_Request__(uds_t *tUDS, uint8_t u8Fs, uint8_t u8Bs, uint8_t u8ST)
{
    uint8_t u8i;
    uint8_t u8Ar_SendData[8];
    uint32_t u32FC_CANID;
    uint32_t Ret = 1;

    if(tUDS->canport == CAN_PORT0)
    {
        //#if UDSRUNMODE
        u8Ar_SendData[0] = FC_PCI + (u8Fs & 0x0F);
        u8Ar_SendData[1] = u8Bs;
        u8Ar_SendData[2] = u8ST;

        if (tUDS->mode == UDS_TYPE_SERVER)
        {
            u32FC_CANID = tUDS->u32Respond_CANID;
        }
        else if (tUDS->mode == UDS_TYPE_CLIENT)
        {
            u32FC_CANID = tUDS->u32Phy_CANID;
        }
        else
        {
            return 1;
        }

        for (u8i = 3; u8i < 64; u8i++)
        {
            u8Ar_SendData[u8i] = tUDS->u8FillValue;
        }

        if (0 == UDS_CANSend(tUDS, u32FC_CANID, 64, u8Ar_SendData))
        {
            tUDS->LData_eWorkStatus = NL_SEGMENT_READ_STATUS;
            tUDS->LData_eFrameStatus = FC_REQ_STATE;
            SetNLTimer__(tUDS, NL_TimeCr, 1);
            taskPrintf(TASK_LEVEL_DEBUG, "LData.FC_Req");
            Ret = 0;
        }
        else
        {
            N_USData_confirm(tUDS, u32FC_CANID, N_ERROR);
        }
    }
    else
    {
    //#else

        u8Ar_SendData[0] = FC_PCI + (u8Fs & 0x0F);
        u8Ar_SendData[1] = u8Bs;
        u8Ar_SendData[2] = u8ST;

        if (tUDS->mode == UDS_TYPE_SERVER)
        {
            u32FC_CANID = tUDS->u32Respond_CANID;
        }
        else if (tUDS->mode == UDS_TYPE_CLIENT)
        {
            u32FC_CANID = tUDS->u32Phy_CANID;
        }
        else
        {
            return 1;
        }

        for (u8i = 3; u8i < 8; u8i++)
        {
            u8Ar_SendData[u8i] = tUDS->u8FillValue;
        }

        if (0 == UDS_CANSend(tUDS, u32FC_CANID, CAN_DLC, u8Ar_SendData))
        {
            tUDS->LData_eWorkStatus = NL_SEGMENT_READ_STATUS;
            tUDS->LData_eFrameStatus = FC_REQ_STATE;
            SetNLTimer__(tUDS, NL_TimeCr, 1);
            taskPrintf(TASK_LEVEL_DEBUG, "LData.FC_Req");
            Ret = 0;
        }
        else
        {
            N_USData_confirm(tUDS, u32FC_CANID, N_ERROR);
        }
    //#endif
    }
    return Ret;
}


/*****************************************************************************
 *   Function   :    SF_Indication__
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    void
 *   Notes      :
 *****************************************************************************/
static void SF_Indication__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t u8TOTAL_CANDL,
                            uint8_t *pu8_Data)
{
    uint8_t u8i;
    if(tUDS->canport == CAN_PORT0)
    {
        //#if UDSRUNMODE
        uint8_t u8PDU[62];

        if (0 != u32PDU_DLC)
        {
            u32PDU_DLC = pu8_Data[0] & 0x0F;
        }
        else
        {
            u32PDU_DLC = pu8_Data[1];
        }

        if (u8TOTAL_CANDL == 8)
        {
            if ((u32PDU_DLC > 0) && (u32PDU_DLC <= 7))
            {
                for (u8i = 0; u8i < u32PDU_DLC; u8i++)
                {
                    u8PDU[u8i] = pu8_Data[u8i + 1];
                }

                tUDS->LData_eFrameStatus = SF_IND_STATE;
                taskPrintf(TASK_LEVEL_DEBUG, "LData.SF_Ind");
                N_USData_Indication(tUDS, u32AI, u32PDU_DLC, u8PDU, N_OK);
            }
        }
        else if ((u8TOTAL_CANDL == 12) || (u8TOTAL_CANDL == 16) || (u8TOTAL_CANDL == 20)
                 || (u8TOTAL_CANDL == 24)  \
                 || (u8TOTAL_CANDL == 32) || (u8TOTAL_CANDL == 48) || (u8TOTAL_CANDL == 64))

        {
            if ((u32PDU_DLC > 0) && (u32PDU_DLC <= u8TOTAL_CANDL - 1))
            {
                for (u8i = 0; u8i < u32PDU_DLC; u8i++)
                {
                    u8PDU[u8i] = pu8_Data[u8i + 1];
                }

                tUDS->LData_eFrameStatus = SF_IND_STATE;
                taskPrintf(TASK_LEVEL_DEBUG, "LData.SF_Ind");
                N_USData_Indication(tUDS, u32AI, u32PDU_DLC, u8PDU, N_OK);
            }
        }
    }
    else
    {
        //#else
        UNUSED(u8TOTAL_CANDL);
        uint8_t u8PDU[UDS_SF_PDU_DLC_MAX];

        if ((u32PDU_DLC > 0) && (u32PDU_DLC <= UDS_SF_PDU_DLC_MAX))
        {
            for (u8i = 0; u8i < u32PDU_DLC; u8i++)
            {
                u8PDU[u8i] = pu8_Data[u8i + 1];
            }

            tUDS->LData_eFrameStatus = SF_IND_STATE;
            taskPrintf(TASK_LEVEL_DEBUG, "LData.SF_Ind");
            N_USData_Indication(tUDS, u32AI, u32PDU_DLC, u8PDU, N_OK);
        }

        //#endif
    }
}



/*****************************************************************************
 *   Function   :    FC_Indication__
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    void
 *   Notes      :
 *****************************************************************************/
static void FC_Indication__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t *pu8_Data)
{
    UNUSED(u32AI);
    UNUSED(u32PDU_DLC);

    SetNLTimer__(tUDS, NL_TimeBs, 0);

    tUDS->LData_FS_Ind = pu8_Data[0] & 0x0F;
    tUDS->LData_BS_Ind = pu8_Data[1];

    if ((pu8_Data[2] >= 0) && (pu8_Data[2] <= 0x7F))
    {
        tUDS->LData_STMin_Ind = pu8_Data[2] * 1000; //1ms -127ms
    }
    else if ((pu8_Data[2] >= 0xF1) && (pu8_Data[2] <= 0xF9))
    {
        tUDS->LData_STMin_Ind = (pu8_Data[2] - 0xF0) * 100;   // 100us-900us
    }
    else
    {
        tUDS->LData_STMin_Ind = 127 * 1000;//127ms
    }

    tUDS->LData_BlockCnt = 0;
    tUDS->LData_eFrameStatus = FC_IND_STATE;
    taskPrintf(TASK_LEVEL_DEBUG, "LData.FC_Ind");

    if (tUDS->LData_FS_Ind == FC_FS_CTS)
    {
        tUDS->LData_eWorkStatus = NL_SEGMENT_SEND_STATUS;
        UDS_Timer_SetPeriod(tUDS, tUDS->LData_STMin_Ind);
    }
    else if (tUDS->LData_FS_Ind == FC_FS_WT)
    {
        tUDS->LData_eFrameStatus = FC_WAIT_STATE;
        SetNLTimer__(tUDS, NL_TimeBs, 1);
        taskPrintf(TASK_LEVEL_DEBUG, "LData.FC_FS_WAIT");
    }
    else if (tUDS->LData_FS_Ind == FC_FS_OVFLW)
    {
        tUDS->LData_PDU_DLC = 0;
        tUDS->LData_eWorkStatus = NL_IDLE_STATUS;
        tUDS->LData_eFrameStatus = MF_OVER_REQ_STATE;
        UDS_TimerClose(tUDS);
        taskPrintf(TASK_LEVEL_DEBUG, "LData.FC_FS_OVER");
        N_USData_confirm(tUDS, tUDS->u32ReqCANID, N_BUFFER_OVFLW);
    }
    else
    {
        tUDS->LData_PDU_DLC = 0;
        tUDS->LData_eWorkStatus = NL_IDLE_STATUS;
        tUDS->LData_eFrameStatus = MF_OVER_REQ_STATE;
        UDS_TimerClose(tUDS);
        N_USData_confirm(tUDS, tUDS->u32ReqCANID, N_INVALID_FS);
    }
}
/*****************************************************************************
 *   Function   :    FF_Indication_CANFD__
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    void
 *   Notes      :
 *****************************************************************************/
static void FF_Indication__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t u8TOTAL_CANDL,
                            uint8_t *pu8_Data)
{
    uint8_t u8i;
    UNUSED(u32AI);

    if(tUDS->canport == CAN_PORT0)
    {
        //#if UDSRUNMODE

        if (u32PDU_DLC == 0)
        {
            u32PDU_DLC = ((uint32_t)(pu8_Data[2]) << 24) + ((uint32_t)(pu8_Data[3]) << 16)  \
                         +((uint32_t)(pu8_Data[4]) << 8) + (uint32_t)(pu8_Data[5]);
        }

        if ((u8TOTAL_CANDL == 8) || (u8TOTAL_CANDL == 12) || (u8TOTAL_CANDL == 16)
            || (u8TOTAL_CANDL == 20)  \
            || (u8TOTAL_CANDL == 24) || (u8TOTAL_CANDL == 32) || (u8TOTAL_CANDL == 48) || (u8TOTAL_CANDL == 64))

        {

            if ((u32PDU_DLC > 62) && (u32PDU_DLC <= 0xFFF))
            {
                tUDS->LData_u8SN = 1;
                tUDS->LData_PDU_DLC = u32PDU_DLC;

                for (u8i = 0; u8i < (u8TOTAL_CANDL - 2); u8i++)
                {
                    tUDS->PDU_MF[u8i] = pu8_Data[u8i + 2];
                }

                tUDS->LData_MF_DataIndex = (u8TOTAL_CANDL - 2);
                tUDS->LData_eFrameStatus = FF_IND_STATE;
                taskPrintf(TASK_LEVEL_DEBUG, "LData.FF_Ind");
                // Auto Send Flow Control Frame
                FC_Request__(tUDS, FC_FS_CTS, tUDS->u8FC_BS, tUDS->u8FC_STmin);

            }
            else if (u32PDU_DLC > 0xFFF)
            {
                tUDS->LData_u8SN = 1;
                tUDS->LData_PDU_DLC = u32PDU_DLC;

                for (u8i = 0; u8i < (u8TOTAL_CANDL - 6); u8i++)
                {
                    tUDS->PDU_MF[u8i] = pu8_Data[u8i + 6];
                }

                tUDS->LData_MF_DataIndex = (u8TOTAL_CANDL - 6);
                tUDS->LData_eFrameStatus = FF_IND_STATE;
                taskPrintf(TASK_LEVEL_DEBUG, "LData.FF_Ind");
                // Auto Send Flow Control Frame
                FC_Request__(tUDS, FC_FS_CTS, tUDS->u8FC_BS, tUDS->u8FC_STmin);
            }
        }
    }
    else
    {
        //#else
        UNUSED(u8TOTAL_CANDL);

        if (u32PDU_DLC > (1024-7))
        {
            FC_Request__(tUDS, FC_FS_OVFLW, tUDS->u8FC_BS, tUDS->u8FC_STmin);
        }
        else if (u32PDU_DLC > 7)
        {
            tUDS->LData_u8SN = 1;
            tUDS->LData_PDU_DLC = u32PDU_DLC;

            for (u8i = 0; u8i < 6; u8i++)
            {
                tUDS->PDU_MF[u8i] = pu8_Data[u8i + 2];
            }

            tUDS->LData_MF_DataIndex = 6;
            tUDS->LData_eFrameStatus = FF_IND_STATE;

            taskPrintf(TASK_LEVEL_DEBUG, "LData.FF_Ind");

            // Auto Send Flow Control Frame
            FC_Request__(tUDS, FC_FS_CTS, tUDS->u8FC_BS, tUDS->u8FC_STmin);
        }

        //#endif
    }
}

/*****************************************************************************
 *   Function   :    CF_Indication_CANFD__
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    void
 *   Notes      :
 *****************************************************************************/
static void CF_Indication__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t u8TOTAL_CANDL,
                            uint8_t *pu8_Data)
{
    uint8_t u8i;
    uint8_t u8SN;
    static uint16_t u16MFBlockCnt = 0;
    uint32_t CF_CANID;
    UNUSED(u32PDU_DLC);
    
    if(tUDS->canport == CAN_PORT0)
    {

        //#if UDSRUNMODE

        if ((u8TOTAL_CANDL == 8) || (u8TOTAL_CANDL == 12) || (u8TOTAL_CANDL == 16)
            || (u8TOTAL_CANDL == 20)  \
            || (u8TOTAL_CANDL == 24) || (u8TOTAL_CANDL == 32) || (u8TOTAL_CANDL == 48) || (u8TOTAL_CANDL == 64))
        {
            if (tUDS->mode == UDS_TYPE_SERVER)
            {
                CF_CANID = tUDS->u32Phy_CANID;
            }
            else if (tUDS->mode == UDS_TYPE_CLIENT)
            {
                CF_CANID = tUDS->u32Respond_CANID;
            }
            else
            {
                return;
            }

            if (CF_CANID == u32AI)
            {
                SetNLTimer__(tUDS, NL_TimeCr, 1);

                u8SN = pu8_Data[0] & 0x0F;

                if (u8SN == (tUDS->LData_u8SN & 0x0F))
                {
                    u16MFBlockCnt++;

                    for (u8i = 0; u8i < (u8TOTAL_CANDL - 1); u8i++)
                    {
                        tUDS->PDU_MF[tUDS->LData_MF_DataIndex + u8i] = pu8_Data[u8i + 1];
                    }

                    tUDS->LData_MF_DataIndex += (u8TOTAL_CANDL - 1);
                    tUDS->LData_u8SN += 1;
                    tUDS->LData_eFrameStatus = CF_IND_STATE;

                    if (tUDS->LData_MF_DataIndex >= tUDS->LData_PDU_DLC)
                    {
                        u16MFBlockCnt = 0;
                        SetNLTimer__(tUDS, NL_TimeCr, 0);
                        tUDS->LData_eWorkStatus = NL_IDLE_STATUS;
                        tUDS->LData_eFrameStatus = MF_OVER_IND_STATE;
                        taskPrintf(TASK_LEVEL_DEBUG, "LData.MF_Ind_OVER");
                        N_USData_Indication(tUDS, u32AI, tUDS->LData_PDU_DLC, tUDS->PDU_MF, N_OK);
                    }
                    else
                    {
                        if ((tUDS->u8FC_BS != 0) && (u16MFBlockCnt >= tUDS->u8FC_BS))
                        {
                            u16MFBlockCnt = 0;
                            SetNLTimer__(tUDS, NL_TimeCr, 0);
                            FC_Request__(tUDS, FC_FS_CTS, tUDS->u8FC_BS, tUDS->u8FC_STmin);
                        }
                    }
                }
                else            // SN Info is Err ,Out of the CF status and Report to the NetLayer
                {
                    u16MFBlockCnt = 0;
                    tUDS->LData_eFrameStatus = MF_OVER_IND_STATE;
                    SetNLTimer__(tUDS, NL_TimeCr, 0);
                    SetNLTimer__(tUDS, NL_TimeBs, 0);
                    #if 0
                    N_USData_Indication(tUDS, u32AI, u32PDU_DLC, NULL_PTR, N_WRONG_SN);
                    #else
                    N_USData_confirm(tUDS, u32AI, N_WRONG_SN);
                    #endif
                }
            }
        }
    }
    else
    {
        //#else
        UNUSED(u8TOTAL_CANDL);

        if (tUDS->mode == UDS_TYPE_SERVER)
        {
            CF_CANID = tUDS->u32Phy_CANID;
        }
        else if (tUDS->mode == UDS_TYPE_CLIENT)
        {
            CF_CANID = tUDS->u32Respond_CANID;
        }
        else
        {
            return;
        }

        if (CF_CANID == u32AI)
        {
            SetNLTimer__(tUDS, NL_TimeCr, 1);

            u8SN = pu8_Data[0] & 0x0F;

            if (u8SN == (tUDS->LData_u8SN & 0x0F))
            {
                u16MFBlockCnt++;

                for (u8i = 0; u8i < 7; u8i++)
                {
                    tUDS->PDU_MF[tUDS->LData_MF_DataIndex + u8i] = pu8_Data[u8i + 1];
                }

                tUDS->LData_MF_DataIndex += 7;
                tUDS->LData_u8SN += 1;
                tUDS->LData_eFrameStatus = CF_IND_STATE;

                if (tUDS->LData_MF_DataIndex >= tUDS->LData_PDU_DLC)
                {
                    u16MFBlockCnt = 0;
                    SetNLTimer__(tUDS, NL_TimeCr, 0);
                    tUDS->LData_eWorkStatus = NL_IDLE_STATUS;
                    tUDS->LData_eFrameStatus = MF_OVER_IND_STATE;
                    taskPrintf(TASK_LEVEL_DEBUG, "LData.MF_Ind_OVER");
                    N_USData_Indication(tUDS, u32AI, tUDS->LData_PDU_DLC, tUDS->PDU_MF, N_OK);
                }
                else
                {
                    if ((tUDS->u8FC_BS != 0) && (u16MFBlockCnt >= tUDS->u8FC_BS))
                    {
                        u16MFBlockCnt = 0;
                        SetNLTimer__(tUDS, NL_TimeCr, 0);
                        FC_Request__(tUDS, FC_FS_CTS, tUDS->u8FC_BS, tUDS->u8FC_STmin);
                    }
                }
            }
            else            // SN Info is Err ,Out of the CF status and Report to the NetLayer
            {
                u16MFBlockCnt = 0;
                tUDS->LData_eFrameStatus = MF_OVER_IND_STATE;
                SetNLTimer__(tUDS, NL_TimeCr, 0);
                SetNLTimer__(tUDS, NL_TimeBs, 0);
                #if 0
                N_USData_Indication(tUDS, u32AI, u16PDU_DLC, NULL_PTR, N_WRONG_SN);
                #else
                N_USData_confirm(tUDS, u32AI, N_WRONG_SN);
                #endif
            }
        }

        //#endif
    }
}

/*****************************************************************************
 *   Function   :    SetNLTimer
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
static void SetNLTimer__(uds_t *tUDS, uint8_t u8TimerType, uint8_t u8Switch)
{
    if (u8TimerType < NL_Timer_MAX)
    {
        tUDS->tNLTimer[u8TimerType].u8SwitchValue = u8Switch;

        if (u8Switch == 1)
        {
            tUDS->tNLTimer[u8TimerType].u64StartTimeValue = driverGetUptime() * 10;
        }
    }
}

// Bs Timeout: Abort message transmission and issue N_USData.confir with N_TIMEOUT_Bs
static void NTimeout_Bs__(void *para)
{
    uds_t *tUDS = (uds_t *) para;
    #if 0
    unsigned char tmp[3] = {FC_FS_CTS, 1, 20};
    FC_Indication__(tUDS , 0, 0 , tmp);
    #else
    UDS_TimerClose(tUDS);
    tUDS->LData_eFrameStatus = NL_FRAME_IDLE;
    tUDS->LData_eWorkStatus = NL_IDLE_STATUS;
    #endif
}

static void NTimeout_Cr__(void *para)
{
    uds_t *tUDS = (uds_t *) para;
    tUDS->LData_eFrameStatus = NL_FRAME_IDLE;
    tUDS->LData_eWorkStatus = NL_IDLE_STATUS;
}
