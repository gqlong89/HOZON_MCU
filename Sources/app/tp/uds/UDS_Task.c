/*****************************************************************************
 *   Include Files
 *****************************************************************************/

#include <app/tp/proxy/Proxy.h>
#include <app/tp/uds/UDS_Config.h>
#include <can_api.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <mid/kernel/timing.h>

#define UDS_CAN_PORT                                 (1)
#define UDS_CAN_ID_PHY                               (0x7A0)
#define UDS_CAN_ID_FUN                               (0x7DF)
#define UDS_CAN_ID_RES                               (0x7B0)
#define UDS_P2_SERVER_T                              (50)
#define UDS_P2E_SERVER_T                             (2000)
#define UDS_S3_SERVER_T                              (5000)
#define UDS_FC_STMIN                                 (10)
#define UDS_FC_BS                                    (0)
#define UDS_FILL_VALUE                               (0x00)
#define UDS_N_BR                                     (50)
#define UDS_N_CR                                     (150)
#define UDS_N_BS                                     (90)



/*****************************************************************************
 *   Local Data Definitions
 *****************************************************************************/
/* timer */

#define POLL_TIMER                TASK_SYS_EVENT_TIMER0
#define POLL_TIME_VALUE          (10)
static unsigned char PollingTimerNo;

#if ( ( UDS_TYPE_SEL == UDS_TYPE_SERVER ) ||    ( UDS_TYPE_SEL == UDS_TYPE_SERVER_CLIENT ) )
static void UDS_Server_Init__(uds_t *tUDS)
{
    tUDS->enable = 0;
    tUDS->mode = UDS_TYPE_SERVER;
    #if UDSRUNMODE
    tUDS->u8FillValue = UDS_FILL_VALUE;
    #else
    tUDS->u8FillValue = UDS_FILL_VALUE;
    #endif
    tUDS->u8FC_STmin = UDS_FC_STMIN;
    tUDS->u8FC_BS = UDS_FC_BS;
    tUDS->u16N_Bs = UDS_N_BS;
    tUDS->u16N_Cr = UDS_N_CR;

    tUDS->u32Phy_CANID = UDS_CAN_ID_PHY;
    tUDS->u32Fun_CANID = UDS_CAN_ID_FUN;
    tUDS->u32Respond_CANID = UDS_CAN_ID_RES;

    tUDS->canport = UDS_CAN_PORT;

    tUDS->PDU_MF = UDS_DATA_S + UDS_DATA_HEAD_LENGTH;

    N_USData_Init(tUDS);

    N_USData_SetIndicationCBFunc(tUDS, uds_data_indication);
    N_USData_SetComfirmCBFunc(tUDS, uds_result_confirm);
}
#endif

#if ( ( UDS_TYPE_SEL == UDS_TYPE_CLIENT ) || ( UDS_TYPE_SEL == UDS_TYPE_SERVER_CLIENT ) )
static void UDS_Client_Init__(uds_t *tUDS)
{
    tUDS->enable = 0;
    tUDS->mode = UDS_TYPE_CLIENT;
    #if UDSRUNMODE
    tUDS->u8FillValue = 0xCC;
    #else
    tUDS->u8FillValue = 0xAA;
    #endif
    tUDS->u8FC_STmin = 20;
    tUDS->u8FC_BS = 0;
    tUDS->u16N_Bs = 100;
    tUDS->u16N_Cr = 100;

    tUDS->u32Phy_CANID = 0x002;
    tUDS->u32Fun_CANID = 0x002;
    tUDS->u32Respond_CANID = 0x002;

    tUDS->canport = CAN_PORT0;

    tUDS->PDU_MF = UDS_DATA_C + UDS_DATA_HEAD_LENGTH;

    N_USData_Init(tUDS);

    N_USData_SetIndicationCBFunc(tUDS, uds_data_indication);
    N_USData_SetComfirmCBFunc(tUDS, uds_result_confirm);
}
#endif

static void UDS_Init__(void)
{

    #if ( UDS_TYPE_SEL == UDS_TYPE_SERVER )
    UDS_Server_Init__(pt_ServerUDS);
    #elif ( UDS_TYPE_SEL == UDS_TYPE_CLIENT )
    UDS_Client_Init__(pt_ClientUDS);
    #elif ( UDS_TYPE_SEL == UDS_TYPE_SERVER_CLIENT )
    UDS_Server_Init__(pt_ServerUDS);
    UDS_Client_Init__(pt_ClientUDS);
    #endif
    /* request timer */
    kernelRequestTimer(&PollingTimerNo);
    kernelEnableTimer(PollingTimerNo, POLL_TIME_VALUE, POLL_TIMER);
    taskPrintf(TASK_LEVEL_LOG, "UDS task is ready to work.");
}

unsigned char task_uds_id;
unsigned int uds_task(unsigned int mask)
{
    /* task init */
    if (mask & TASK_SYS_EVENT_INIT)
    {
        task_uds_id = kernelGetTaskID();
        UDS_Init__();
        return 0;
    }
    else if (mask & POLL_TIMER)
    {
        mask &= (~POLL_TIMER);

        #if ( UDS_TYPE_SEL == UDS_TYPE_SERVER )
        N_USData_Poll(pt_ServerUDS);
        #elif ( UDS_TYPE_SEL == UDS_TYPE_CLIENT )
        N_USData_Poll(pt_ClientUDS);
        #elif ( UDS_TYPE_SEL == UDS_TYPE_SERVER_CLIENT )
        N_USData_Poll(pt_ServerUDS);
        N_USData_Poll(pt_ClientUDS);
        #endif

        kernelEnableTimer(PollingTimerNo, POLL_TIME_VALUE, POLL_TIMER);
    }
    else if (mask & TASK_SYS_EVENT_MASK)
    {
        if (mask & TASK_SYS_EVENT_READ)
        {
            mask &= (~ TASK_SYS_EVENT_READ);
            #if ( UDS_TYPE_SEL == UDS_TYPE_SERVER )
            N_USData_Poll(pt_ServerUDS);
            #elif ( UDS_TYPE_SEL == UDS_TYPE_CLIENT )
            N_USData_Poll(pt_ClientUDS);
            #elif ( UDS_TYPE_SEL == UDS_TYPE_SERVER_CLIENT )
            N_USData_Poll(pt_ServerUDS);
            N_USData_Poll(pt_ClientUDS);
            #endif
        }
    }

    if (mask & TASK_UDS_UPLOAD)
    {
        mask &= (~ TASK_UDS_UPLOAD);
        proxy_upload_retry();
    }

    return mask;
}

