/*****************************************************************************
 *   Include Files
 *****************************************************************************/

#include <app/tp/uds/UDS_Config.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <sys/_stdint.h>

/*-----------------------------------------------------------------------
 Session Layer Time parameter and timeout event
 -----------------------------------------------------------------------*/

/*****************************************************************************
 *   Function   :    N_USData_Init
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
void N_USData_Init(uds_t *tUDS)
{
    tUDS->UDS_ApplIndication_Handle = NULL_PTR;

    L_Data_Init(tUDS);
    UDS_CAN_Init(tUDS);
    UDS_Timer_Init(tUDS);

    taskPrintf(TASK_LEVEL_LOG, UDSTP_VER_NO);
}

/*****************************************************************************
 *   Function   :    N_USData_Poll
 *   Description:    this poll is period called in task
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
void N_USData_Poll(uds_t *tUDS)
{
    if (tUDS->enable == 1)
    {
        L_Data_Indication(tUDS);
        LData_TimerManager(tUDS);
    }
}

/*****************************************************************************
 *   Function   :    N_USData_Request
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
/*
uint32_t N_USData_Request(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t *pu8_PDUData)
{
    uint32_t Ret = 1;

    if ((pu8_PDUData != NULL_PTR) && (u32PDU_DLC <= UDS_SERVER_MAX_BYTE))
    {
        Ret = L_Data_request(tUDS, u32AI, u32PDU_DLC, pu8_PDUData);

        if (Ret == 0)
        {
            tUDS->NSData_u8SID = pu8_PDUData[0];
            tUDS->NSData_u8NRC = pu8_PDUData[2];
        }
    }

    return (Ret);
}
*/
/*****************************************************************************
 *   Function   :    N_USData_confirm
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
void N_USData_confirm(uds_t *tUDS, uint32_t u32AI, uint8_t eNResult)
{
    if (tUDS->UDS_ApplConfirm_Handle != NULL_PTR)
    {
        tUDS->UDS_ApplConfirm_Handle(tUDS, u32AI, eNResult);
    }
}
/*****************************************************************************
 *   Function   :    N_USData_Indication
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
void N_USData_Indication_Canfd(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC_Canfd,
                               uint8_t *pu8_PDUData,
                               uint8_t eNResult)
{
    if (tUDS->UDS_ApplIndication_Handle != NULL_PTR)
    {
        tUDS->UDS_ApplIndication_Handle(tUDS, u32AI, u32PDU_DLC_Canfd, pu8_PDUData, eNResult);
    }
}


/*****************************************************************************
 *   Function   :    N_USData_Indication
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
void N_USData_Indication(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t *pu8_PDUData,
                         uint8_t eNResult)
{
    if (tUDS->UDS_ApplIndication_Handle != NULL_PTR)
    {
        tUDS->UDS_ApplIndication_Handle(tUDS, u32AI, u32PDU_DLC, pu8_PDUData, eNResult);
    }
}

/*****************************************************************************
 *   Function   :    N_USData_SetComfirmCBFunc
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
void N_USData_SetComfirmCBFunc(uds_t *tUDS, Uds_Appl_Confirm_Func func_ComfirmHandle)
{
    if ((func_ComfirmHandle != NULL_PTR) && (tUDS != NULL_PTR))
    {
        tUDS->UDS_ApplConfirm_Handle = func_ComfirmHandle;
    }
}

/*****************************************************************************
 *   Function   :    N_USData_SetIndicationCBFunc
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
void N_USData_SetIndicationCBFunc(uds_t *tUDS, Uds_Appl_Indication_Func func_IndicationHandle)
{
    if ((func_IndicationHandle != NULL_PTR) && (tUDS != NULL_PTR))
    {
        tUDS->UDS_ApplIndication_Handle = func_IndicationHandle;
    }
}

