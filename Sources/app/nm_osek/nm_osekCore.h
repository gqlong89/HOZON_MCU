


#ifndef NM_OSEKCORE_H
#define NM_OSEKCORE_H

#include <app/can_mgr/can_basic_cfg.h>

#if USED_NM_OSEK
/*NM地址信息*/
#define NM_BASE_ID          0x400
#define NM_MIN_ID           0
#define NM_MAX_ID           0xff
#define NM_LOCAL_ID         0x02


#define RX_ERROR_LIMIT      4   // 接收错误阈值
#define TX_ERROR_LIMTT      8   // 发送错误阈值


/*各定时器的值，单位是MS*/
#define TTyp_Value                  100
#define TMax_Value                  260
#define TError_Value                1000
#define TWBS_Value                  5000
#define TTx_Value                   20
#define TSleepReqMin_LimpHome_Value 5000
#define TSleepReqMin_Normal_Value   5000

#define TAppActive_Value            90  // 应用报文 首帧定时器

/*PDU报文内容功能码，按位操作的*/
#define NM_MaskAlive                0x01
#define NM_MaskRing                 0x02
#define NM_MaskLimphome             0x04
#define NM_MaskSleepInd             0x10   /* sleep ind */
#define NM_MaskSleepAck             0x20   /* sleep ack */


#define Rx_PDU_MAX                  (5)// PDU最大接收缓存


#define  GOTOMODE_AWAKE_MASK        (1<<0)
#define  GOTOMODE_BUSSLEEP_MASK     (1<<1)



typedef enum
{
    NM_stOff = 0,
    NM_stInit,
    NM_stInitReset,
    NM_stNormal,
    NM_stNormalPrepSleep,      /* sleep indication */
    NM_stTwbsNormal,           /* wait bus sleep   */
    NM_stBusSleep,             /* bus sleep        */
    NM_stLimphome,             /* limp hone        */
    NM_stLimphomePrepSleep,    /* limphome sleep indication */
    NM_stTwbsLimphome,         /* limphone wait bus sleep */
    NM_stOn                    /* in fact if not Off then ON. */
} NMState_E;                 // 网络状态


typedef enum
{
    NM_BusSleep = 1,
    NM_Awake
} NMMode_E;

typedef enum
{
    NM_ckNormal = 1,              /* supported by direct and indirect NM */
    NM_ckNormalExtended,          /* only supported by indirect NM */
    NM_ckLimphome                 /* only supported by direct NM */
} ConfigKindName_E;



typedef struct
{
    unsigned char source;
    unsigned char destination;
    union
    {
        unsigned char byte;
        struct
        {
            unsigned char alive     : 1;
            unsigned char ring      : 1;
            unsigned char limphome  : 1;
            unsigned char reserved1 : 1;
            unsigned char sleepInd  : 1;
            unsigned char sleepAck  : 1;
            unsigned char reserved2 : 2;
        } bit;
    } opCode;
    unsigned char ringData[6] ;
} NMPDU_ST; //  PDU结构体

typedef struct
{
    unsigned char p_in;//写指针
    unsigned char p_out;//读指针
    NMPDU_ST BUF[Rx_PDU_MAX];//已经接收到的PDU
} CircularBuffer_ST;   //定义一个环形缓冲区结构体


typedef union
{
    unsigned int word;
    struct
    {
        unsigned int NMactive: 1;                /* 允许NM通讯 */
        unsigned int NMAPPactive: 1;             /*允许应用通讯*/
        unsigned int canModeNormal: 1;           /*1：CAN口处于正工作状态，0：CAN口处于监听状态*/
        unsigned int bussleep: 1;                /*表示本地休眠条件是成立*/
        unsigned int configurationstable: 1;
        unsigned int presleepenable: 1;          /* 预休眠使能 */
        unsigned int sleepAck: 1;                // 发送SLEEPACK条件成立
    } bit;
} NetworkStatus_U;         //当前网络状态


typedef union
{
    unsigned int word;
    struct
    {
        unsigned int stable : 1;// 环稳定，即不发或不收ALIVE，不在LIMPHOME状态
        unsigned int limphome : 1;  //对外已经发送过limphome状态
    } bit;
} NMMerker_U; //  网络稳定




typedef struct
{
    unsigned char nm_osekEN;

    unsigned char nmLocalID;  // 本地节点地址

    unsigned int nmBaseADDR;   // nm ID基地址

    unsigned char nmMinID;     //nm ID范围最小地址
    unsigned char nmMaxID;     //nm ID范围最大地址

    unsigned int  remoteAwakeMinID;//远程唤醒最小ID
    unsigned int  remoteAwakeMaxID;//远程唤醒最大ID

    unsigned char normalList[32];      // 处于normal状态的节点
    unsigned char
    limphomeList[32];    // 处于limehome状态下的节点，按位来表示，最多支持32*8= 256个节点。

    NMState_E currentState;    // 本节点当前状态
    NMState_E lastState;       // 本节点上一个状态

    unsigned char logical_successor;  // 逻辑继承者

    unsigned int TTx;
    unsigned int TTyp;
    unsigned int TMax;
    unsigned int TError;
    unsigned int TWbs;
    unsigned int TSleepReqMin_LimpHome;
    unsigned int TSleepReqMin_Normal;       //这些都是定时器的值

    unsigned int TAppActive;                //应用报文帧时间

    unsigned char appActiveFlag;            //应用报文首帧标志

    unsigned int nmTimerOverFlow;     //  NM 定时器材溢出标志
    unsigned char
    goToModeUsedFlag;   // gotoMODE 调用标志，第一位：调用过AWAKE，第二位，调用过BUSSLEEP。
    unsigned char NM_TX_OKFLAG;       //1:发送 成功，0没有发送成功。
    unsigned char awake_ByApp_Flag;        //1:被应用报文唤醒，0，没有被应用报文唤醒
    unsigned char busSleep_ErrFlag;         //在休眠状态下，收到总线错误，有可能是有报文。
    unsigned char receive_NM_Flag;      // 收到网络管理报文标志。1为收到，0为没有收到

    unsigned char nmRxCount;  //RX接收错误次数
    unsigned char nmTxCount;  //发送错误次数

    NetworkStatus_U NetworkStatus;// 当前网络工作状态。
    NMMerker_U   nmMerker;        //逻辑环稳定状态


    NMPDU_ST RxPDU;//接收到的PDU

    NMPDU_ST TxPDU;//要发送的PDU

} NMControlBlock_ST;

void nmAddtoPresent(unsigned char canport, unsigned char nodeId);
void nmAddtoLimphome(unsigned char canport, unsigned char nodeId);



/*对外函数接口*/
extern void nm_OsekMainFunction(void);
extern void NM_TxConformation(unsigned char canport, FXCAN_BUFF_TYPE mail_n);
extern void NM_RxIndication(unsigned char canport, CAN_MSG *msg);
extern void GotoMode(unsigned char canport, NMMode_E newMode);
extern void Start_NM_Osek(unsigned char canport);
extern void D_Stop(unsigned char canport);
extern void D_Start(unsigned char canport);


extern void wakeUpNM_ByAPP(unsigned char canport);
extern void gotoLimpHome_ByBusOff(unsigned char canport);
extern void CanBus_ErrorCheck(unsigned char canport);



#endif

#endif



