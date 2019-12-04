
/*

 */

#ifndef _NM_AUTOSAR_H_
#define _NM_AUTOSAR_H_

#include <app/can_mgr/can_mgr.h>
#include <app/can_mgr/can_basic_cfg.h>

#if USED_NM_AUTOSAR


/*��������״̬*/
/*���У�repeat msg״̬��ΪNORMA��immediate�������*/
typedef enum
{
    NM_SLEEP_MODE_STATE                = (1 << 0),
    NM_PREPARE_SLEEP_MODE_SATATE       = (1 << 1),
    NM_REPEATE_MSG_NORMAL_STATE        = (1 << 2),
    NM_REPEATE_MSG_IMMEDIATE_STATE     = (1 << 3),
    NM_NORMAL_STATE                    = (1 << 4),
    NM_READY_SLEEP_STATE               = (1 << 5),
    NM_VOD                             = (0),       /*��ʾû�н���NM(�������)ʱ��״̬*/
} NM_STATE;

/*����ṹ�����ͣ���ʾCAN��ǰ״̬����һ��״̬*/
typedef struct
{
    unsigned char nm_lastState;           // ǰһ��״̬
    unsigned char nm_currentState;        // ��ǰ״̬
    unsigned char WakeUp_reason;          //������ԭ��
    unsigned char StayAwake_reason;
    unsigned char AWB;                   // ��������λ��0���������ѣ�1����������// active wakeup bit
    unsigned char stateChange;           // ״̬�仯������ʲô״̬��Ϊʲô״̬��
} CAN_PORT_STATE;

/*��������*/
typedef enum
{
    NM_WAKEUP_PASSIVE      = 0,/*��������*/
    NM_WAKEUP_ACTIVE,          /*��������*/
} NM_WAKEUP_TYPE;

#define T_NM_ImmediateCycleTime (20*1000) // ���ٷ���PDU����,�߾��ȶ�ʱ������λ US
#define T_NM_MessageCycle (500*1000)  // ��������PDU���ڣ��߾��ȶ�ʱ������λ US

#define N_ImmediateNM_TIMES  (5)// ���ٷ���PDU����
#define T_NM_TIMEOUT       (2000)
#define T_REPEAT_MESSAGE   (1600)
#define T_WAIT_BUS_SLEEP   (2000)

#define ON_OK  (1)   // ��⵽ON��
#define ON_dis (0)   // û�м�⵽ON��

#define SignalLine_OK   (!0) //Ӳ���ź����ٴ���һ��
#define SignalLine_DIS  (0)  //Ӳ���ź�һ���������� 

#define RECEIVE_OK   (1)  //�յ�NM_PDU���Ļ��� �յ�REPEAT MESSAGE BIT
#define RECEIVE_NONE (0)

#define Expired_OK   (1)  // ��ʱ������ɹ�
#define Expired_NONE (0)

#define Request_Repeat_YES   (1<<0)   // ��������REPEAT_MESSAGE//
#define Request_Repeat_NONE  (0)

#define accelerate_YES  (1)    //pdu  ���ٷ���
#define accelerate_NONE (0)    //pdu  ��������

#define CAN_STATUS_NORMAL   (0)  //  can��״̬ normal
#define CAN_STATUS_LISTEN   (1)  //  can��״̬ listen  


#define PDUTYPE1    (1<<0) // pdu ����1
#define PDUTYPE2    (1<<1) // pdu ����2   //PS �����������8λ����Ϊ��Ӧ�ı�־��8λ��

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

#define Passive_WakeUp   (0)     // ��������
#define Active_WakeUp    (1<<4)     // ��������  // ������������Ϊ������PDU������

#define UDS_TYPE1         (1<<0)   //  
#define T_WAIT_DIAG_REQ   (5000)    // uds��ʱ����5S


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


