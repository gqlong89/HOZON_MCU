#ifndef _UDS_CONFIG_H_
#define _UDS_CONFIG_H_

#include <app/tp/proxy/Proxy.h>
#include <can_api.h>
#include <sys/_stdint.h>

#define  UDSTP_VER_NO        "UDSTP V2.0"
#define UDSRUNMODE     (1)   //0:can  1:canfd


/**********************************************************************
 *  UDS                  : 协议栈的类型选择 ：
 *   UDS_TYPE_HOST        : 协议栈 只作为 服务器端
 *   UDS_TYPE_CLIENT      : 协议栈 只作为 客户端
 *   UDS_TYPE_HOST_CLIENT : 协议栈 同时作为 服务器端和客户端 ，双卡双待
 ***********************************************************************/
#define  UDS_TYPE_SERVER                  (1)
#define  UDS_TYPE_CLIENT                  (2)
#define  UDS_TYPE_SERVER_CLIENT           (3)
#define  UDS_TYPE_SEL                     UDS_TYPE_SERVER_CLIENT

/* 修改报文接收缓存大小 200 ->10 后期调整 */
#define     CAN_RX_BUFF_MAX               10

#define   UDS_SERVER_MAX_BYTE             (0xFFF)
#define   UDS_CLIENT_MAX_BYTE             (0x800)   //2K

#define    NULL_PTR     (void*) 0

typedef void (*Uds_Appl_Confirm_Func)(void *uds_para, uint32_t u32AI, uint8_t eNResult);
typedef void (*Uds_Appl_Indication_Func)(void *uds_para, uint32_t u32AI, uint32_t u32PDU_DLC,
        uint8_t *pu8_PDUData,
        uint8_t eNResult);
typedef void (*Uds_NTimeout_Func)(void *uds_para);

/*-----------------------------------------------------------------------
 uds  LData  Layer , Frame Status
 -----------------------------------------------------------------------*/
typedef enum
{
    NL_FRAME_IDLE,
    SF_REQ_STATE,
    FF_REQ_STATE,
    FC_REQ_STATE,
    CF_REQ_STATE,
    MF_OVER_REQ_STATE,
    MF_BLOCK_OVER_REQ_STATE,
    FC_WAIT_STATE,
    SF_IND_STATE,
    FF_IND_STATE,
    FC_IND_STATE,
    CF_IND_STATE,
    MF_OVER_IND_STATE,
} NL_Frame_Status_E;

/*-----------------------------------------------------------------------
 Network Layer Result
 -----------------------------------------------------------------------*/
typedef enum
{
    N_RESULT_NULL,
    N_OK,
    N_TIMEOUT_A,
    N_TIMEOUT_Bs,
    N_TIMEOUT_Cr,
    N_WRONG_SN,
    N_INVALID_FS,
    N_UNEXP_PDU,
    N_WFT_OVRN,
    N_BUFFER_OVFLW,
    N_ERROR,
    N_DRIVER_ERR
} N_Result_E;

/*-----------------------------------------------------------------------
 Network Layer Status
 -----------------------------------------------------------------------*/
typedef enum
{
    NL_IDLE_STATUS,
    NL_SEGMENT_SEND_STATUS,
    NL_SEGMENT_READ_STATUS
} NL_STATE_E;

/*-----------------------------------------------------------------------
 the uds timing parameters
 -----------------------------------------------------------------------*/
typedef enum
{
    //NL_TimeAs,
    //NL_TimeAr,
    NL_TimeBs,
    //NL_TimeBr,
    //NL_TimeCs,
    NL_TimeCr,
    NL_Timer_MAX
} NL_Timer_E;

/*-----------------------------------------------------------------------
 uds  can
 -----------------------------------------------------------------------*/
typedef struct
{
    uint32_t u32CANID;
    uint8_t u8DLC;
    uint8_t Ar_u8CANData[64];
} UDS_CANMsg_T;

typedef struct
{
    uint8_t u8SwitchValue;
    uint8_t u8NResult;
    uint16_t u16TimeValue;
    uint64_t u64StartTimeValue;
    Uds_NTimeout_Func func_NTimeout;
} NL_Timer_T;

typedef struct UDS_tag
{
    uint8_t mode;
    uint8_t enable;
    uint8_t canport;
    uint8_t u8FillValue;
    uint8_t u8FC_STmin;
    uint8_t u8FC_BS;
    uint8_t LData_eWorkStatus;
    uint8_t LData_eFrameStatus;
    uint8_t *PDU_MF;
    uint8_t LData_FS_Ind;
    uint8_t LData_BS_Ind;
    uint8_t LData_u8SN;
    uint8_t NSData_u8SID;
    uint8_t NSData_u8NRC;
    uint16_t u16N_As;
    uint16_t u16N_Bs;
    uint16_t u16N_Cr;
    uint32_t LData_PDU_DLC;

    uint32_t LData_MF_DataIndex;

    uint16_t LData_BlockCnt;
    uint16_t canReadBuf_Header;
    uint16_t canReadBuf_Tail;
    uint16_t canReadBuf_ValidCnt;
    uint32_t LData_STMin_Ind;
    uint32_t u32ReqCANID;
    uint32_t u32Phy_CANID;
    uint32_t u32Fun_CANID;
    uint32_t u32Respond_CANID;

    NL_Timer_T tNLTimer[NL_Timer_MAX];

    Uds_Appl_Confirm_Func UDS_ApplConfirm_Handle;
    Uds_Appl_Indication_Func UDS_ApplIndication_Handle;
    UDS_CANMsg_T canReadBuf[CAN_RX_BUFF_MAX];
} uds_t;

#if ( UDS_TYPE_SEL == UDS_TYPE_SERVER )
    extern uds_t tUDS_Server;
    extern uds_t *pt_ServerUDS;
    extern unsigned char UDS_DATA_S[UDS_SERVER_MAX_BYTE + UDS_DATA_HEAD_LENGTH];
#elif ( UDS_TYPE_SEL == UDS_TYPE_CLIENT )
    extern uds_t tUDS_Client;
    extern uds_t *pt_ClientUDS;
    extern unsigned char UDS_DATA_C[UDS_CLIENT_MAX_BYTE + UDS_DATA_HEAD_LENGTH];
#elif ( UDS_TYPE_SEL == UDS_TYPE_SERVER_CLIENT )
    extern uds_t tUDS_Server;
    extern uds_t tUDS_Client;

    extern uds_t *pt_ServerUDS;
    extern uds_t *pt_ClientUDS;

    extern unsigned char UDS_DATA_S[UDS_SERVER_MAX_BYTE + UDS_DATA_HEAD_LENGTH];
    extern unsigned char UDS_DATA_C[UDS_CLIENT_MAX_BYTE + UDS_DATA_HEAD_LENGTH];
#endif

/*****************************    UDS   CAN   ******************************************/
extern void UDS_CAN_Init(uds_t *tUDS);
uint32_t UDS_CANSend(uds_t *tUDS, uint32_t u32CANID, uint8_t u8Can_Dl, uint8_t *Ar_u8CANData);
extern UDS_CANMsg_T *UDS_CANRead(uds_t *tUDS);
extern uint32_t UDS_CANAccept(uint32_t canport, CAN_MSG *tCANMsg);

/*****************************    UDS   Timer   ******************************************/

extern void UDS_Timer_Init(uds_t *tUDS);
extern void UDS_TimerClose(uds_t *tUDS);
extern void UDS_Timer_SetPeriod(uds_t *tUDS, uint32_t u32PeriodValueMs);

/*****************************    UDS   LData   ******************************************/
extern void L_Data_Init(uds_t *tUDS);
extern uint32_t L_Data_request(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC,
                               uint8_t *pu8_Data);
extern void L_Data_Indication(uds_t *tUDS);
extern void LData_TimerManager(uds_t *tUDS);
extern void CF_Request(uds_t *tUDS);

extern uint32_t SF_Request__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t *pu8_Data);
extern uint32_t FF_Request__(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC, uint8_t *pu8_Data);

/*****************************    UDS   NSData   ******************************************/
extern void N_USData_Init(uds_t *tUDS);
extern void N_USData_Poll(uds_t *tUDS);
extern uint32_t N_USData_Request(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC,
                                 uint8_t *pu8_PDUData);
extern void N_USData_confirm(uds_t *tUDS, uint32_t u32AI, uint8_t eNResult);
extern void N_USData_Indication(uds_t *tUDS, uint32_t u32AI, uint32_t u32PDU_DLC,
                                uint8_t *pu8_PDUData,
                                uint8_t eNResult);

extern void N_USData_SetComfirmCBFunc(uds_t *tUDS, Uds_Appl_Confirm_Func func_ComfirmHandle);
extern void N_USData_SetIndicationCBFunc(uds_t *tUDS,
        Uds_Appl_Indication_Func func_IndicationHandle);
extern char *UDSTP_GetVersion(void);

#endif

