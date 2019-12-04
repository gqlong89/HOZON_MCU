
/*

 */

#ifndef _NM_AUTOSAR_H_
#define _NM_AUTOSAR_H_

#include <app/can_mgr/can_mgr.h>
#include <app/can_mgr/can_basic_cfg.h>

#if USED_NM_AUTOSAR


/*定义总线状态*/
/*其中，repeat msg状态分为NORMA和immediate两种情况*/
typedef enum
{
    NM_SLEEP_MODE_STATE                = (1 << 0),
    NM_PREPARE_SLEEP_MODE_SATATE       = (1 << 1),
    NM_REPEATE_MSG_NORMAL_STATE        = (1 << 2),
    NM_REPEATE_MSG_IMMEDIATE_STATE     = (1 << 3),
    NM_NORMAL_STATE                    = (1 << 4),
    NM_READY_SLEEP_STATE               = (1 << 5),
    NM_VOD                             = (0),       /*表示没有进入NM(网络管理)时的状态*/
} NM_STATE;

/*定义结构体类型，表示CAN当前状态与上一个状态*/
typedef struct
{
    unsigned char nm_lastState;           // 前一个状态
    unsigned char nm_currentState;        // 当前状态
    unsigned char WakeUp_reason;          //　唤醒原因
    unsigned char StayAwake_reason;
    unsigned char AWB;                   // 主动唤醒位。0：被动唤醒，1：主动唤醒// active wakeup bit
    unsigned char stateChange;           // 状态变化，即从什么状态变为什么状态。
} CAN_PORT_STATE;

/*唤醒类型*/
typedef enum
{
    NM_WAKEUP_PASSIVE      = 0,/*被动唤醒*/
    NM_WAKEUP_ACTIVE,          /*主动唤醒*/
} NM_WAKEUP_TYPE;

#define T_NM_ImmediateCycleTime (20*1000) // 快速发送PDU周期,高精度定时器，单位 US
#define T_NM_MessageCycle (500*1000)  // 正常发送PDU周期，高精度定时器，单位 US

#define N_ImmediateNM_TIMES  (5)// 快速发送PDU周期
#define T_NM_TIMEOUT       (2000)
#define T_REPEAT_MESSAGE   (1600)
#define T_WAIT_BUS_SLEEP   (2000)

#define ON_OK  (1)   // 检测到ON线
#define ON_dis (0)   // 没有检测到ON线

#define SignalLine_OK   (!0) //硬线信号至少存在一个
#define SignalLine_DIS  (0)  //硬线信号一个都不存在 

#define RECEIVE_OK   (1)  //收到NM_PDU报文或是 收到REPEAT MESSAGE BIT
#define RECEIVE_NONE (0)

#define Expired_OK   (1)  // 定时器溢出成功
#define Expired_NONE (0)

#define Request_Repeat_YES   (1<<0)   // 主动请求REPEAT_MESSAGE//
#define Request_Repeat_NONE  (0)

#define accelerate_YES  (1)    //pdu  加速发送
#define accelerate_NONE (0)    //pdu  正常发送

#define CAN_STATUS_NORMAL   (0)  //  can口状态 normal
#define CAN_STATUS_LISTEN   (1)  //  can口状态 listen  


#define PDUTYPE1    (1<<0) // pdu 类型1
#define PDUTYPE2    (1<<1) // pdu 类型2   //PS ，最多向下移8位，因为相应的标志就8位。

#define WakeUp_Default                   (0x00)
#define WakeUp_KL15                      (0x01)
#define WakeUp_ReceivePDU                (0x02)
#define WakeUp_Charge                    (0x03)
#define WakeUp_ACC                       (0x04)
#define WakeUp_Telematics_service        (0x05)

//#define Signal_ON       StayAwake_KL15
//#define Signal_ACC      StayAwake_ACC
//#define Signal_S_Charge StayAwake_Charge
//#define Signal_F_Charge StayAwake_Charge   // SignalLine_State

#define Passive_WakeUp   (0)     // 被动唤醒
#define Active_WakeUp    (1<<4)     // 主动唤醒  // 左移三们是因为会用在PDU发送中

#define UDS_TYPE1         (1<<0)   //  
#define T_WAIT_DIAG_REQ   (5000)    // uds定时器，5S


#define RM_FROM_SLEEP          (1<<0)
#define RM_FROM_P_SLEEP        (1<<1)
#define RM_FROM_NORMAL         (1<<5)
#define RM_FROM_R_SLEEP        (1<<4)
#define NORMAL_FROM_RM         (1<<2)
#define NORMAL_FROM_R_SLEEP    (1<<3)



extern void nm_AutosarInit(unsigned char canport);

void stop_timer_SEND_PDU(unsigned char canport);
void start_timer_SEND_PDU(unsigned char canport, unsigned int timeout);
unsigned char NM_PDU_ReceiveCheck(unsigned char canport, CAN_MSG *tCANMsg);
void PDU_CAN_Send(unsigned char canport);

void nm_AutosarMainFunction(void);
void Autosar_PDU_CAN0_SendHandle(void);
void Autosar_PDU_CAN1_SendHandle(void);

void CanBus_ErrorCheck_AUTUSAR(unsigned char canport);



#endif


#endif /* APP_CAN_NM_AUTOSAR_H_ */


