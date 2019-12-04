



/*

 */

#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <stdint.h>
#include <timer.h>
#include <app/nm_autosar/nm_autosar.h>
#include <pm.h>
#include <call.h>
#include <app/power/power.h>
#include <app/can_mgr/can_mgr.h>
#include <adc.h>
#include <rtc.h>
#include <app/power/power.h>
#include <config.h>
#include <app/can_mgr/canbus_off.h>
#include <app/can_mgr/can_basic_cfg.h>
#include <app/can_mgr/can_broadCast.h>
#include <app/can_mgr/can_basic.h>
#include "S32K144/include/S32K144.h"
#include <mid/can_flexcan/can_flexcan.h>



#if USED_NM_AUTOSAR 

extern LOCAL_WAKEUPCOND_ST NM_Local_WakeupCondition;//CAN ���籾�ػ�������

extern CAN_BASIC_CFG_ST can_basic_cfg;  // �ⲿ�����CAN���� ���� ���� ��

extern unsigned char canBasic_taskid;
extern CAN_Type *const g_flexcanBase[];


unsigned char  nm_AutosarEN[2];      //  �������˱��� ����ʾCANͨ���Ƿ�����ʹ��NM���ܡ�
CAN_PORT_STATE nm_canState[2];/*��������CAN����״̬����*/


unsigned char  receive_NM_PDU_Flag[2];               // �յ�NM����
unsigned char  receive_RMB_Flag[2];                  //  REPEAT MESSAGE BIT RECEIVE
unsigned char  request_NM_Repeat_Message_Flag[2];    // ��������REPEAT����


unsigned char  T_REPEAT_MESSAG_ExpiredFlag[2];  // T_REPEAT_MESSAG ��ʱ�� �����־
unsigned char  T_NM_TIMEROUT_ExpiredFlag[2];    // T_NM_TIMEROUT  ��ʱ�� �����־
unsigned char  T_WAIT_BUS_SLEEP_ExpiredFlag[2]; //T_WAIT_BUS_SLEEP ��ʱ�� �����־��

unsigned char  T_NM_PDU_ExpiredFlag[2];       //  NM  PDU ��ʱ�������־

unsigned char  accelerate_PDU_Flag[2];    // PDU���ٷ��ͱ�־
unsigned char  accelerate_PDU_count[2];   // ����PDU�Ѿ����͵Ĵ���

unsigned char  autosar_busSleep_Error[2];  //������״̬�£��յ����ߴ����п������б��ġ�
unsigned char  can_status[2];              //CAN ��״̬��

CAN_SEND_MSG   NM_PDU[2];  //  pdu����

unsigned int   ID_NM_PDU_MIN[2];    //CAN0ͨ����NM_PDU������Сֵ������ݲ�ͬӦ������������
unsigned int   ID_NM_PDU_MAX[2];


//  nm���õĶ�ʱ��
static int8_t timer_T_NM_TIMEROUT_CanPort0 = TIMER_INVALID;   //WORK�޹���ʱ��
static int8_t timer_T_REPEAT_MESSAGE_CanPort0 = TIMER_INVALID;//
static int8_t timer_T_WAIT_BUS_SLEEP_CanPort0 = TIMER_INVALID;   //�ȴ����߶�ʱ��

static int8_t timer_T_NM_TIMEROUT_CanPort1 = TIMER_INVALID;   //WORK�޹���ʱ��
static int8_t timer_T_REPEAT_MESSAGE_CanPort1 = TIMER_INVALID;//
static int8_t timer_T_WAIT_BUS_SLEEP_CanPort1 = TIMER_INVALID;   //�ȴ����߶�ʱ��

//  ����PDU���õĶ�ʱ��
static int8_t timer_SEND_PDU_CanPort0 = TIMER_INVALID;// CanPort0 ����PDU���Ķ�ʱ��
static int8_t timer_SEND_PDU_CanPort1 = TIMER_INVALID;// CanPort1 ����PDU���Ķ�ʱ��

#if  1   // �˲���ΪUDS��� ��־ �� ������õ��Ķ�ʱ��
unsigned char  receive_UDS_Message[2];               // ���յ�UDS��ϱ���
unsigned char  during_UDS[2];//  ��UDS����ڼ�
static int8_t timer_UDS_Type1[2] = {TIMER_INVALID, TIMER_INVALID}; // uds��϶�ʱ������������־�����ɡ�TYPE1Ϊ�����������й�UDS�����Ķ�ʱ��
#endif

void restart_timer_UDS_Type1(unsigned       char canport, unsigned int timeout)
{
    if (canport > 1)
    {
        return;
    }

    ltimer_stop(timer_UDS_Type1[canport]);
    ltimer_start(timer_UDS_Type1[canport], timeout);
}

void stop_timer_UDS_Type1(unsigned char canport)
{
    if (canport > 1)
    {
        return;
    }

    ltimer_stop(timer_UDS_Type1[canport]);
}

static void USD_Type1_PORT0_isr(int8_t handle)
{
    if (handle != timer_UDS_Type1[0])
    {
        printk(PRINTK_LEVEL_ERROR, "timer_UDS_Type1[0]");
        return;
    }

    during_UDS[0] = 0;// ��ʱ����� ����ʾ5S�ѹ�������UDS����ڼ�
    ltimer_stop(timer_UDS_Type1[0]);
    printk(PRINTK_LEVEL_ERROR, "CanPort0 UDS Finished!!!");
}

static void USD_Type1_PORT1_isr(int8_t handle)
{
    if (handle != timer_UDS_Type1[1])
    {
        printk(PRINTK_LEVEL_ERROR, "timer_UDS_Type1[1]");
        return;
    }

    during_UDS[1] = 0;// ��ʱ����� ����ʾ5S�ѹ�������UDS��� �ڼ�
    ltimer_stop(timer_UDS_Type1[1]);
    printk(PRINTK_LEVEL_ERROR, "CanPort1 UDS Finished!!!");
}

void start_timer_T_NM_TIMEROUT(unsigned char canport, unsigned int timeout) //  ����T_NM_TIMEROUT��ʱ�������趨���ʱ��
{
    if (canport == 0)
    {
        ltimer_start(timer_T_NM_TIMEROUT_CanPort0, timeout);
    }

    if (canport == 1)
    {
        ltimer_start(timer_T_NM_TIMEROUT_CanPort1, timeout);
    }
}

void stop_timer_T_NM_TIMEROUT(unsigned char canport)  //ֹͣT_NM_TIMEROUT��ʱ��
{
    if (canport == 0)
    {
        ltimer_stop(timer_T_NM_TIMEROUT_CanPort0);
    }

    if (canport == 1)
    {
        ltimer_stop(timer_T_NM_TIMEROUT_CanPort1);
    }	 
}

void restart_timer_T_NM_TIMEROUT(unsigned char canport,        unsigned int timeout) //����T_NM_TIMEROUT������ͣ�ٿ������趨���ʱ��
{
    stop_timer_T_NM_TIMEROUT(canport);
    start_timer_T_NM_TIMEROUT(canport, timeout);
}

void start_timer_T_REPEAT_MESSAGE(unsigned char canport, unsigned int timeout) //  ����T_REPEAT_MESSAGE��ʱ�������趨���ʱ��
{
    if (canport == 0)
    {
        ltimer_start(timer_T_REPEAT_MESSAGE_CanPort0, timeout);
    }

    if (canport == 1)
    {
        ltimer_start(timer_T_REPEAT_MESSAGE_CanPort1, timeout);
    }
}

void stop_timer_T_REPEAT_MESSAGE(unsigned char canport)  //ֹͣT_REPEAT_MESSAGE��ʱ��
{
    if (canport == 0)
    {
        ltimer_stop(timer_T_REPEAT_MESSAGE_CanPort0);
    }

    if (canport == 1)
    {
        ltimer_stop(timer_T_REPEAT_MESSAGE_CanPort1);
    }
}

void restart_timer_T_REPEAT_MESSAGE(unsigned char canport,
                                    unsigned int timeout) //����T_REPEAT_MESSAGE������ͣ�ٿ������趨���ʱ��
{
    stop_timer_T_REPEAT_MESSAGE(canport);
    start_timer_T_REPEAT_MESSAGE(canport, timeout);
}

void start_timer_T_WAIT_BUS_SLEEP(unsigned char canport, unsigned int timeout) //
{
    if (canport == 0)
    {
        ltimer_start(timer_T_WAIT_BUS_SLEEP_CanPort0, timeout);
    }

    if (canport == 1)
    {
        ltimer_start(timer_T_WAIT_BUS_SLEEP_CanPort1, timeout);
    }
}

void stop_timer_T_WAIT_BUS_SLEEP(unsigned char canport)
{
    if (canport == 0)
    {
        ltimer_stop(timer_T_WAIT_BUS_SLEEP_CanPort0);
       // printk(PRINTK_LEVEL_ERROR, "stop  timer timer_T_WAIT_BUS_SLEEP_CanPort0");
    }

    if (canport == 1)
    {
        ltimer_stop(timer_T_WAIT_BUS_SLEEP_CanPort1);
       // printk(PRINTK_LEVEL_ERROR, "stop  timer timer_T_WAIT_BUS_SLEEP_CanPort1");
    }
}

void restart_timer_T_WAIT_BUS_SLEEP(unsigned char canport, unsigned int timeout)
{
    stop_timer_T_WAIT_BUS_SLEEP(canport);
    start_timer_T_WAIT_BUS_SLEEP(canport, timeout);
}

static void CanPort0_T_NM_TIMEROUT_isr(int8_t handle)
{
    if (handle != timer_T_NM_TIMEROUT_CanPort0)
    {
        printk(PRINTK_LEVEL_ERROR, "not timer_T_NM_TIMEROUT");
        return;
    }

    T_NM_TIMEROUT_ExpiredFlag[0] = Expired_OK;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//��������

}

static void CanPort0_T_REPEAT_MESSAGE_isr(int8_t handle)
{
    if (handle != timer_T_REPEAT_MESSAGE_CanPort0)
    {
        printk(PRINTK_LEVEL_ERROR, "not timer_T_REPEAT_MESSAGE");
        return;
    }

    T_REPEAT_MESSAG_ExpiredFlag[0] = Expired_OK;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//��������
}

static void CanPort0_T_WAIT_BUS_SLEEP_isr(int8_t handle)
{
    if (handle != timer_T_WAIT_BUS_SLEEP_CanPort0)
    {
        printk(PRINTK_LEVEL_ERROR, "not");
        return;
    }

    T_WAIT_BUS_SLEEP_ExpiredFlag[0] = Expired_OK;
	kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//��������
}

static void CanPort1_T_NM_TIMEROUT_isr(int8_t handle)
{
    if (handle != timer_T_NM_TIMEROUT_CanPort1)
    {
        printk(PRINTK_LEVEL_ERROR, "not timer_T_NM_TIMEROUT");
        return;
    }

    T_NM_TIMEROUT_ExpiredFlag[1] = Expired_OK;
	kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//��������

}

static void CanPort1_T_REPEAT_MESSAGE_isr(int8_t handle)
{
    if (handle != timer_T_REPEAT_MESSAGE_CanPort1)
    {
        printk(PRINTK_LEVEL_ERROR, "not timer_T_REPEAT_MESSAGE");
        return;
    }

    T_REPEAT_MESSAG_ExpiredFlag[1] = Expired_OK;
	kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//��������
}

static void CanPort1_T_WAIT_BUS_SLEEP_isr(int8_t handle)
{
    if (handle != timer_T_WAIT_BUS_SLEEP_CanPort1)
    {
        printk(PRINTK_LEVEL_ERROR, "not");
        return;
    }

    T_WAIT_BUS_SLEEP_ExpiredFlag[1] = Expired_OK;
	kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//��������
}

void canPort_Listen(unsigned     char canport) //  ʹCAN�ڴ��� ����״̬
{
	CAN_Type *base = g_flexcanBase[canport];
	FLEXCAN_HAL_SetOperationMode(base, FLEXCAN_LISTEN_ONLY_MODE); //ʹCAN�ڴ�����״̬
	can_status[canport] = CAN_STATUS_LISTEN;
  	flexcan_clear_errstatus(canport);
    flexcan_set_errint(canport, true);                           //��������CAN�����ж�//  Deinit can��������20190903
    autosar_busSleep_Error[canport] = 0;   //  ��ա������־��
}

void canPort_Normal(unsigned char canport)   // ʹCAN�ڴ��� ����״̬
{
	CAN_Type *base = g_flexcanBase[canport];
    FLEXCAN_HAL_SetOperationMode(base, FLEXCAN_NORMAL_MODE); //ʹCAN�ڴ�����������״̬
    can_status[canport] = CAN_STATUS_NORMAL;
}

void CanBus_ErrorCheck_AUTUSAR(unsigned char canport)
{
    autosar_busSleep_Error[canport] = 1;    //  �������д���
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);
}

void start_timer_SEND_PDU(unsigned char canport,
                          unsigned int timeout) 
{
    if (canport == 0)
    {
        htimer_start(timer_SEND_PDU_CanPort0, timeout);
    }

    if (canport == 1)
    {
        htimer_start(timer_SEND_PDU_CanPort1, timeout);
    }

    if (timeout == T_NM_ImmediateCycleTime)
    {
        accelerate_PDU_Flag[canport] = accelerate_YES;
        accelerate_PDU_count[canport] = 0;
    }

    if (timeout == T_NM_MessageCycle)
    {
        accelerate_PDU_Flag[canport] = accelerate_NONE;
        accelerate_PDU_count[canport] = 0;
    }
}

void stop_timer_SEND_PDU(unsigned char canport)  
{
    if (canport == 0)
    {
        htimer_stop(timer_SEND_PDU_CanPort0);
		T_NM_PDU_ExpiredFlag[canport] = 0;// ����ȷ��͵�PDU
    }

    if (canport == 1)
    {
        htimer_stop(timer_SEND_PDU_CanPort1);
		T_NM_PDU_ExpiredFlag[canport] = 0;// ����ȷ��͵�PDU
    }	
}

void restart_timer_SEND_PDU(unsigned char canport, unsigned int timeout)
{
    stop_timer_SEND_PDU(canport);
    start_timer_SEND_PDU(canport, timeout);

    if (timeout == T_NM_ImmediateCycleTime)
    {
        accelerate_PDU_Flag[canport] = accelerate_YES;
        accelerate_PDU_count[canport] = 0;
    }

    if (timeout == T_NM_MessageCycle)
    {
        accelerate_PDU_Flag[canport] = accelerate_NONE;
        accelerate_PDU_count[canport] = 0;
    }
}

static void CanPort0_SEND_PDU_isr(int8_t handle)
{
    if (handle != timer_SEND_PDU_CanPort0)
    {
        printk(PRINTK_LEVEL_ERROR, "not ");
        return;
    }

    T_NM_PDU_ExpiredFlag[0] = T_NM_PDU_ExpiredFlag[0] | PDUTYPE1;// Ҫ���͵�һ�����͵�PDU

    Autosar_PDU_CAN0_SendHandle();//ֻ�����ж��﷢��PDU�������ڿ���PDUʱ��20MS�����޷��� ��׼ȷ��

   // kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST3);//��������
}

static void CanPort1_SEND_PDU_isr(int8_t handle)
{
    if (handle != timer_SEND_PDU_CanPort1)
    {
        printk(PRINTK_LEVEL_ERROR, "not");
        return;
    }

    T_NM_PDU_ExpiredFlag[1] = T_NM_PDU_ExpiredFlag[1] | PDUTYPE1;
	
	Autosar_PDU_CAN1_SendHandle();//ֻ�����ж��﷢��PDU�������ڿ���PDUʱ��20MS�����޷��� ��׼ȷ��

	//kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST3);//��������
}


void Autosar_PDU_CAN0_SendHandle(void)
{
    if (T_NM_PDU_ExpiredFlag[0] == 0) // û���������û�з�������
    {
        return;
    }

    if (T_NM_PDU_ExpiredFlag[0] & PDUTYPE1)
    {
        T_NM_PDU_ExpiredFlag[0] &= (~PDUTYPE1); //���PDU��־

        //  can_send(CAN_PORT0, MAIL_TRY, &NM_PDU[0]);  // ����PDU
        PDU_CAN_Send(CAN_PORT0);//����PDU
        
		restart_timer_T_NM_TIMEROUT(CAN_PORT0, T_NM_TIMEOUT);// ֻҪ����NM���� ����������ʱ��

        if (accelerate_PDU_Flag[0] == accelerate_YES) // �ж� �ǲ��Ǽ���PDU
        {
            accelerate_PDU_count[0] = accelerate_PDU_count[0] + 1;

            if (accelerate_PDU_count[0] >= (N_ImmediateNM_TIMES - 1)) // ��Ϊ��״̬ת���������ֶ�����һ��
            {
                accelerate_PDU_count[0] = 0;
                accelerate_PDU_Flag[0] = accelerate_NONE;
                //accelerate_PDU_count[0] = 0;
                restart_timer_SEND_PDU(CAN_PORT0, T_NM_MessageCycle);

                nm_canState[0].nm_lastState = nm_canState[0].nm_currentState;//����ط�Ҫ ����һ��״̬ת����
                nm_canState[0].nm_currentState = NM_REPEATE_MSG_NORMAL_STATE;
                printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN 0 Be in NORMAL REPEAT MESSAGE STATE");
            }
        }
        else
        {
            accelerate_PDU_count[0] = 0;
        }
    }

    if (T_NM_PDU_ExpiredFlag[0] & PDUTYPE2)
    {
        T_NM_PDU_ExpiredFlag[0] &= (~PDUTYPE2);
    }
}

void Autosar_PDU_CAN1_SendHandle(void)
{
    if (T_NM_PDU_ExpiredFlag[1] == 0) // û���������û�з�������
    {
        return;
    }

    if (T_NM_PDU_ExpiredFlag[1] & PDUTYPE1)
    {
        T_NM_PDU_ExpiredFlag[1] &= (~PDUTYPE1); //  ���PDU��־

        //  can_send(CAN_PORT1, MAIL_TRY, &NM_PDU[1]);// ����PDU
        PDU_CAN_Send(CAN_PORT1);//����PDU
        restart_timer_T_NM_TIMEROUT(CAN_PORT1, T_NM_TIMEOUT);// ֻҪ����NM���� ��������ʱ��

        if (accelerate_PDU_Flag[1] == accelerate_YES) // �ж� �ǲ��Ǽ���PDU
        {
            accelerate_PDU_count[1] = accelerate_PDU_count[1] + 1;

            if (accelerate_PDU_count[1] >= (N_ImmediateNM_TIMES - 1)) // ��Ϊ��״̬ת���������ֶ�����һ��
            {
                accelerate_PDU_count[1] = 0;
                accelerate_PDU_Flag[1] = accelerate_NONE;
                //  accelerate_PDU_count[1] = 0;
                restart_timer_SEND_PDU(CAN_PORT1, T_NM_MessageCycle);

                nm_canState[1].nm_lastState = nm_canState[1].nm_currentState;//����ط�Ҫ ����һ��״̬ת����
                nm_canState[1].nm_currentState = NM_REPEATE_MSG_NORMAL_STATE;
                printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN 1 Be in NORMAL REPEAT MESSAGE STATE");
            }
        }
        else
        {
            accelerate_PDU_count[1] = 0;
        }
    }

    if (T_NM_PDU_ExpiredFlag[1] & PDUTYPE2)
    {
        T_NM_PDU_ExpiredFlag[1] &= (~PDUTYPE2);
    }
}

void PDU_CAN_Send(unsigned char canport)//
{
    if (canport > 1)
    {
        printk(PRINTK_LEVEL_LOG, "ERROR CanPort!!!");
        return;
    }

    if (check_busOffState(canport) == 1) // �鿴�Ƿ���BUSOFF�� ��Ϊ������֡ PDU ��ʱҪ�ô˺��� ����Ҫ�ж�BUSOFF
    {
        return;
    }

    NM_PDU[canport].Data[1] = 0;// ������������REPEAT MSG��־�����������ѱ�־//����0,�ٷ�װ
    
    NM_PDU[canport].Data[1] |= request_NM_Repeat_Message_Flag[canport];
	NM_PDU[canport].Data[1] |= nm_canState[canport].AWB;
    NM_PDU[canport].Data[2] = nm_canState[canport].WakeUp_reason;        // ����ԭ��

 //   if ((nm_canState[canport].nm_currentState == NM_REPEATE_MSG_IMMEDIATE_STATE)
 //       || (nm_canState[canport].nm_currentState == NM_REPEATE_MSG_NORMAL_STATE))
 //   {
 //       NM_PDU[canport].Data[3] = 0;// ��REPEAT MESSAGE ״̬
 //   }
  //  else
  //  {
  //      NM_PDU[canport].Data[3] = 1;  // ��REPEAT MESSAGE ״̬
  //  }
    NM_PDU[canport].Data[3] = nm_canState[canport].stateChange;
    NM_PDU[canport].Data[4] = nm_canState[canport].StayAwake_reason;

    can_send(canport, TX_CAN_NORMAL_NM_AUTOSAR, &NM_PDU[canport]);//�ֶ���һ��PDU����
    return ;
}

void NM_Autosar_PDU_Init(unsigned char canport)//PDU���ݳ�ʼ����
{
    NM_PDU[canport].DLC = 8;
    NM_PDU[canport].isRTR = 0;
    NM_PDU[canport].MsgID = can_basic_cfg.nm_cfg[canport].nm_autosar_cfg.nma_base_id+can_basic_cfg.nm_cfg[canport].nm_autosar_cfg.nma_locol_id;

    if (NM_PDU[canport].MsgID > 0x7ff)
    {
        NM_PDU[canport].isEID = 1;
    }
    else
    {
        NM_PDU[canport].isEID = 0;
    }

    NM_PDU[canport].Data[0] = 0x01;     //  TBOX�ڵ��ʶ��
    //  NM_PDU[0].Data[1] = 1;        // ������������REPEAT MSG��־�����������ѱ�־
    //  NM_PDU[0].Data[2] = 1;        // ����ԭ��
    NM_PDU[canport].Data[3] = 0;        //  �Ƿ���REPEAT MESSAGE״̬
    //  NM_PDU[0].Data[4] = 1;        // ���ֻ��ѵ�ԭ��
    NM_PDU[canport].Data[5] = 0;
    NM_PDU[canport].Data[6] = 0;
    NM_PDU[canport].Data[7] = 0;

}


//  ����0 û���յ�NM_PDU
//  ����1 �յ�NM_PDU
unsigned char NM_PDU_ReceiveCheck(unsigned char canport, CAN_MSG *tCANMsg)
{
    if (nm_AutosarEN[canport] == ENABLE)
    {
        if ((ID_NM_PDU_MIN[canport] <= (tCANMsg->MsgID & (unsigned int)0x7ff))
            && ((tCANMsg->MsgID & (unsigned int)0x7ff)) <= ID_NM_PDU_MAX[canport])
        {
			
			restart_timer_T_NM_TIMEROUT(canport, T_NM_TIMEOUT);//  ֻҪ�յ�NM���ģ�������ʱ��

			receive_NM_PDU_Flag[canport] = RECEIVE_OK;
            kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//��������
       //     printk(PRINTK_LEVEL_LOG, "can %d receive NM_PDU", canport);

            if ((tCANMsg->Data[1] & 0x01) != 0) // �յ� REPEAT MESSAGE BIT
            {
                receive_RMB_Flag[canport] = RECEIVE_OK;
                kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//��������
               // printk(PRINTK_LEVEL_LOG, "can %d receive NM_RMB", canport);
            }

            return (1);
        }
    }
    return (0);
}

void nm_AutosarInit(unsigned char canport)//
{

	if(can_basic_cfg.nm_cfg[canport].nm_type == NM_TYPE_AUTOSAR)
	{
		nm_AutosarEN[canport] = 1;	
	}
	
    ID_NM_PDU_MIN[canport] = can_basic_cfg.nm_cfg[canport].nm_autosar_cfg.nma_base_id+can_basic_cfg.nm_cfg[canport].nm_autosar_cfg.nma_mix_id;
    ID_NM_PDU_MAX[canport] =can_basic_cfg.nm_cfg[canport].nm_autosar_cfg.nma_base_id+can_basic_cfg.nm_cfg[canport].nm_autosar_cfg.nma_max_id;

    if (nm_AutosarEN[canport] == ENABLE)
    {
        nm_canState[canport].nm_lastState = NM_VOD;
        nm_canState[canport].nm_currentState = NM_SLEEP_MODE_STATE;
        nm_canState[canport].WakeUp_reason = WakeUp_Default;         //��ʼ�� ����ԭ��Ϊ�գ�Ĭ��Ϊ�� ��
        nm_canState[canport].StayAwake_reason = StayAwake_Default;   //��ʼ�� ���ֻ���ԭ��
        nm_canState[canport].AWB = Passive_WakeUp;                   //��ʼ�� Ĭ��Ϊ�������� ��DSC��// ��������λ
        printk(PRINTK_LEVEL_LOG, "POWER ON!CAN %d Be in sleep Mode", canport);
    }
    else
    {
        printk(PRINTK_LEVEL_LOG, "CAN %d has no AutoSar Function!", canport);
    }

    NM_Autosar_PDU_Init(canport);      // PDU��ʼ��
//    PDU_CAN_Send(canport);             // ������֡PDU��


 #if 1  // �ⲿ��Ϊͨ������´�������״̬�ı��7����־

    autosar_busSleep_Error[canport] = 0;  //  ��ʼ����û���յ����ߴ���

	can_status[canport] = CAN_STATUS_NORMAL;   //  ��ʼ����can ��״̬Ϊ��NORMAL

    receive_NM_PDU_Flag[canport] = RECEIVE_NONE;// ��ʼ����û���յ�PDU����

    receive_RMB_Flag[canport] = RECEIVE_NONE;   // ��ʼ����û��û���յ� REPEAT ��־λ

    request_NM_Repeat_Message_Flag[canport] = Request_Repeat_NONE;//��ʼ����û����������PEPEAT MESSAGE

    receive_UDS_Message[canport] = 0;// ��ʼ����CAN0û���յ���ϱ���

    during_UDS[canport] = 0;//��ʼ������ʾ CAN0û����UDS����ڼ�

    T_NM_TIMEROUT_ExpiredFlag[canport] = Expired_NONE;  // ��ʼ������ʱ��û�����

    T_REPEAT_MESSAG_ExpiredFlag[canport] = Expired_NONE;  // ��ʼ������ʱ��û�����

    T_WAIT_BUS_SLEEP_ExpiredFlag[canport] = Expired_NONE;  //��ʼ������ʱ��û�����

    #endif

    accelerate_PDU_Flag[canport] = accelerate_NONE;//��ʼ����PDU ������


    accelerate_PDU_count[canport] = 0;  // ��ʼ�����ѷ��͵ļ���PDU����Ϊ0

    T_NM_PDU_ExpiredFlag[canport] = 0;

    //����NM��ʱ��

    if (nm_AutosarEN[0] == ENABLE)
    {
        if (TIMER_INVALID == timer_T_NM_TIMEROUT_CanPort0)
        {
            timer_T_NM_TIMEROUT_CanPort0 = ltimer_create(CanPort0_T_NM_TIMEROUT_isr);//ֻ������û�п���
        }

        if (TIMER_INVALID == timer_T_REPEAT_MESSAGE_CanPort0)
        {
            timer_T_REPEAT_MESSAGE_CanPort0 = ltimer_create(CanPort0_T_REPEAT_MESSAGE_isr);//ֻ������û�п���
        }

        if (TIMER_INVALID == timer_T_WAIT_BUS_SLEEP_CanPort0)
        {
            timer_T_WAIT_BUS_SLEEP_CanPort0 = ltimer_create(CanPort0_T_WAIT_BUS_SLEEP_isr);//ֻ������û�п���
        }

        if (TIMER_INVALID == timer_SEND_PDU_CanPort0)
        {
            timer_SEND_PDU_CanPort0 = htimer_create(CanPort0_SEND_PDU_isr);//ֻ������û�п���
        }

        if (TIMER_INVALID == timer_UDS_Type1[0])
        {
            timer_UDS_Type1[0] = ltimer_create(USD_Type1_PORT0_isr);//ֻ������û�п���
        }
    }

    if (nm_AutosarEN[1] == ENABLE)
    {
        if (TIMER_INVALID == timer_T_NM_TIMEROUT_CanPort1)
        {
            timer_T_NM_TIMEROUT_CanPort1 = ltimer_create(CanPort1_T_NM_TIMEROUT_isr);//ֻ������û�п���

        }

        if (TIMER_INVALID == timer_T_REPEAT_MESSAGE_CanPort1)
        {
            timer_T_REPEAT_MESSAGE_CanPort1 = ltimer_create(CanPort1_T_REPEAT_MESSAGE_isr);//ֻ������û�п���
        }

        if (TIMER_INVALID == timer_T_WAIT_BUS_SLEEP_CanPort1)
        {
            timer_T_WAIT_BUS_SLEEP_CanPort1 = ltimer_create(CanPort1_T_WAIT_BUS_SLEEP_isr);//ֻ������û�п���
        }

        if (TIMER_INVALID == timer_SEND_PDU_CanPort1)
        {
            timer_SEND_PDU_CanPort1 = htimer_create(CanPort1_SEND_PDU_isr);//ֻ������û�п���
        }

        if (TIMER_INVALID == timer_UDS_Type1[1])
        {
            timer_UDS_Type1[1] = ltimer_create(USD_Type1_PORT1_isr);//ֻ������û�п���
        }
    }
}

//void check_Can_Net_State(void)
void nm_AutosarMainFunction(void)
{
    unsigned char i = 0;

    for (i = 0; i < 2; i++)
    {
        if (nm_AutosarEN[i] == DISABLE)
        {
            continue;  // ����ѭ����ִ����
        }

        switch (nm_canState[i].nm_currentState)
        {
            case NM_SLEEP_MODE_STATE://��ǰ״̬Ϊ����״̬

				
                /*�Ա�״̬���õĽ��������־�������ά��*/

                receive_UDS_Message[i] = RECEIVE_NONE;//����һ��UDS��ϱ�־

                if(autosar_busSleep_Error[i] == 1)
                {
                    autosar_busSleep_Error[i] = 0;
                    canPort_Normal(i);
                    break;
                }

                if (NM_Local_WakeupCondition.current_state != 0)
                {
                    nm_canState[i].AWB = Active_WakeUp;// ��������
					canPort_Normal(i);	  //  

                    //���ӻ���ԭ��
                    if (nm_canState[i].WakeUp_reason == WakeUp_Default)
                    {
                        if (NM_Local_WakeupCondition.current_state & Signal_ACC)
                        {
                            nm_canState[i].WakeUp_reason = WakeUp_ACC;
                            printk(PRINTK_LEVEL_LOG, "WakeUp by ACC!!!");
                        }
                        else if (NM_Local_WakeupCondition.current_state & Signal_ON)
                        {
                            nm_canState[i].WakeUp_reason = WakeUp_KL15;
                            printk(PRINTK_LEVEL_LOG, "WakeUp by KL15!!!");
                        }
                        else if ((NM_Local_WakeupCondition.current_state & Signal_F_Charge) || (NM_Local_WakeupCondition.current_state & Signal_S_Charge))
                        {
                            nm_canState[i].WakeUp_reason = WakeUp_Charge;
                            printk(PRINTK_LEVEL_LOG, "WakeUp by CHARGE!!!");
                        }
						else if(NM_Local_WakeupCondition.current_state & Signal_ON_VT)
						{
							nm_canState[i].WakeUp_reason = WakeUp_Telematics_service;
                            printk(PRINTK_LEVEL_LOG, "WakeUp by telematics service!!!");
						}
                        else
                        {
                        }
                    }
                    else
                    {
                        printk(PRINTK_LEVEL_LOG, "State Error!IN Sleep Mode ,WakeUp reason should be null ",
                               NM_Local_WakeupCondition.current_state);
                    }
					
                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_IMMEDIATE_STATE;//״̬�л�
                    nm_canState[i].stateChange = RM_FROM_SLEEP;//��SLPPE״̬���� REPEAT MESSAGE��HZ
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in IMMEDIATE REPEAT MESSAGE STATE", i);
                    /*��Ϊͨ�����յ�PDU�������ı�����״̬�����ȼ�û�������������ȼ���*/

                    receive_NM_PDU_Flag[i] = RECEIVE_NONE;

                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);   //����T_NM_TIMEROUT Timer
                    restart_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE); //����T_REPEAT_MESSAGE Timer

                    /*����REPEAT ״̬��Ҫ��10MS���ڷ���һ��PDU,�ö�ʱ����ʱ��̫���������ֶ���һ��*/
                    PDU_CAN_Send(i);// �ֶ�����һ��PDU����
                
                    /*�������ѣ�����Ҫ���ٷ���*/
                    start_timer_SEND_PDU(i, T_NM_ImmediateCycleTime); //��������PDU���ĵĶ�ʱ��������Ϊ�������ڡ�				

                    first_BC(i); // �ֶ�������֡����

					/*�ɹ�����һ��PUD��Ҫ����   WORK	�޹���ʱ��*/
					restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);

                    start_BC_timerByChl(i);
                    break;
                }

                if (receive_NM_PDU_Flag[i] == RECEIVE_OK)
                {
                    receive_NM_PDU_Flag[i] = RECEIVE_NONE;// ���״̬���˱����Ѿ�������
                    /*�ɹ��յ�һ��PUD��Ҫ����   WORK  �޹���ʱ��*/

                    nm_canState[i].AWB = Passive_WakeUp;// ��������

                    if (nm_canState[i].WakeUp_reason == WakeUp_Default)
                    {
                        nm_canState[i].WakeUp_reason = WakeUp_ReceivePDU;
                        printk(PRINTK_LEVEL_LOG, "WakeUp by Receive PDU!!!");
                    }
                    else
                    {
                        printk(PRINTK_LEVEL_LOG, "State Error!IN Sleep Mode ,WakeUp reason should be null ",
                               NM_Local_WakeupCondition.current_state);
                    }

                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);

                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_NORMAL_STATE;//״̬�л�
                    nm_canState[i].stateChange = RM_FROM_SLEEP;//��SLPPE״̬���� REPEAT MESSAGE��HZ
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!CAN %d Be in NM_REPEATE_MSG_NORMAL_STATE", i);

                    start_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT); //����T_NM_TIMEROUT Timer
                    start_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE);  //����T_REPEAT_MESSAGE Timer

                    /*����REPEAT ״̬��Ҫ��10MS���ڷ���һ��PDU,�ö�ʱ����ʱ��̫���������ֶ���һ��*/
                    //  can_send(i, MAIL_TRY, &NM_PDU[i]);//�ֶ���һ��PDU����
                    PDU_CAN_Send(i);
                    /*�ɹ�����һ��PUD��Ҫ����   WORK  �޹���ʱ��*/
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);

                    /*�������ѣ�����Ҫ�������ٶȷ���*/
                    start_timer_SEND_PDU(i, T_NM_MessageCycle); //��������PDU���ĵĶ�ʱ���������ٶ�

                    first_BC(i);

					start_BC_timerByChl(i);
                    break;
                }

				if((nm_canState[i].nm_currentState == NM_SLEEP_MODE_STATE)&&(can_status[i] == CAN_STATUS_NORMAL))
				{
					autosar_busSleep_Error[i] = 0;
					canPort_Listen(i);
					break;
				}

                break;

            case NM_PREPARE_SLEEP_MODE_SATATE:
                /*�Ա�״̬���õĽ��������־�������ά��*/
                receive_RMB_Flag[i] = RECEIVE_NONE;
                request_NM_Repeat_Message_Flag[i] = Request_Repeat_NONE;
                receive_UDS_Message[i] = 0;//����һ��UDS��ϱ�־

                if (NM_Local_WakeupCondition.current_state == SignalLine_OK)
                {
                    nm_canState[i].AWB = Active_WakeUp;// ��������

                    if (nm_canState[i].WakeUp_reason == WakeUp_Default)
                    {
                        if (NM_Local_WakeupCondition.current_state & Signal_ACC)
                        {
                            nm_canState[i].WakeUp_reason = WakeUp_ACC;
                            printk(PRINTK_LEVEL_LOG, "WakeUp by ACC!!!");
                        }
                        else if (NM_Local_WakeupCondition.current_state & SignalLine_OK)
                        {
                            nm_canState[i].WakeUp_reason = WakeUp_KL15;
                            printk(PRINTK_LEVEL_LOG, "WakeUp by KL15!!!");
                        }
                        else if ((NM_Local_WakeupCondition.current_state & Signal_F_Charge) || (NM_Local_WakeupCondition.current_state & Signal_S_Charge))
                        {
                            nm_canState[i].WakeUp_reason = WakeUp_Charge;
                            printk(PRINTK_LEVEL_LOG, "WakeUp by CHARGE!!!");
                        }
                        else
                        {
                        }
                    }
                    else
                    {
                        printk(PRINTK_LEVEL_LOG, "State Error!IN Prepare Sleep Mode ,WakeUp reason should be null ",
                               NM_Local_WakeupCondition.current_state);
                    }

                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_IMMEDIATE_STATE;//״̬�л�
                    nm_canState[i].stateChange = RM_FROM_P_SLEEP;//��SLPPE״̬���� REPEAT MESSAGE��HZ
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in IMMEDIATE REPEAT MESSAGE STATE", i);

                    start_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT); //����T_NM_TIMEROUT Timer
                    start_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE);  //����T_REPEAT_MESSAGE Timer

                    /*����REPEAT ״̬��Ҫ��10MS���ڷ���һ��PDU,�ö�ʱ����ʱ��̫���������ֶ���һ��*/
                    //  can_send(i, MAIL_TRY, &NM_PDU[i]);//�ֶ���һ��PDU����
                    PDU_CAN_Send(i);//�ֶ�����PDU
                    /*�ɹ�����һ��PUD��Ҫ����   WORK  �޹���ʱ��*/
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);
                    /*�������ѣ�����Ҫ���ٷ���*/
                    start_timer_SEND_PDU(i, T_NM_ImmediateCycleTime); //��������PDU���ĵĶ�ʱ��������Ϊ�������ڡ�

                   // broadcast_firstFrame(i);//  �ֶ����͹㲥����
                    first_BC(i);

                    //broadcast_startTimer(i);//����Ӧ�ñ��Ķ�ʱ��// ��һ֡PDU���һ֡Ӧ�ñ��ļ��ҪС��20MS��
                    start_BC_timerByChl(i);
                    break;
                }

                if (receive_NM_PDU_Flag[i] == RECEIVE_OK)
                {
                    receive_NM_PDU_Flag[i] = RECEIVE_NONE;// ���״̬���˱����Ѿ�������
                    /*�ɹ��յ�һ��PUD��Ҫ����   WORK  �޹���ʱ��*/
                    nm_canState[i].AWB = Passive_WakeUp;//��������

                    if (nm_canState[i].WakeUp_reason == WakeUp_Default)
                    {
                        nm_canState[i].WakeUp_reason = WakeUp_ReceivePDU;
                        printk(PRINTK_LEVEL_LOG, "WakeUp by Receive PDU!!!");
                    }
                    else
                    {
                        printk(PRINTK_LEVEL_LOG, "State Error!IN Prepare Sleep Mode ,WakeUp reason should be null ",
                               NM_Local_WakeupCondition.current_state);
                    }

                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);

                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_NORMAL_STATE;//״̬�л�
                     nm_canState[i].stateChange = RM_FROM_P_SLEEP;//��SLPPE״̬���� REPEAT MESSAGE��HZ
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!CAN %d Be in NORMAL REPEAT MESSAGE STATE", i);

                    start_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT); //����T_NM_TIMEROUT Timer
                    start_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE);  //����T_REPEAT_MESSAGE Timer

                    /*����REPEAT ״̬��Ҫ��10MS���ڷ���һ��PDU,�ö�ʱ����ʱ��̫���������ֶ���һ��*/
                    //can_send(i, MAIL_TRY, &NM_PDU[i]);//�ֶ���һ��PDU����
                    PDU_CAN_Send(i);//�ֶ�����PDU
                    /*�ɹ�����һ��PUD��Ҫ����   WORK  �޹���ʱ��*/
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);

                    /*�������ѣ�����Ҫ�������ٶȷ���*/
                    start_timer_SEND_PDU(i, T_NM_MessageCycle); //��������PDU���ĵĶ�ʱ���������ٶ�


					//broadcast_firstFrame(i);//  �ֶ����͹㲥����
					first_BC(i);
                   // broadcast_startTimer(i);//����Ӧ�ñ��Ķ�ʱ��// ��һ֡PDU���һ֡Ӧ�ñ��ļ��ҪС��20MS��
					start_BC_timerByChl(i);
				    break;
                }

                if (T_WAIT_BUS_SLEEP_ExpiredFlag[i] == Expired_OK)
                {
                    T_WAIT_BUS_SLEEP_ExpiredFlag[i] = Expired_NONE;

                    T_NM_TIMEROUT_ExpiredFlag[i] = Expired_NONE;  // ��ʼ������ʱ��û�����

                    T_REPEAT_MESSAG_ExpiredFlag[i] = Expired_NONE;  // ��ʼ������ʱ��û�����

                    /*ֹͣ����CAN���Ͷ�ʱ��*/
                    stop_timer_T_NM_TIMEROUT(i);
                    stop_timer_T_REPEAT_MESSAGE(i);
                    stop_timer_T_WAIT_BUS_SLEEP(i);
                    stop_timer_SEND_PDU(i);
                  //  broadcast_stopTimer(i);//ֹͣ�㲥���Ͷ�ʱ��
                    stop_BC_timerByChl(i);
                    //ֹͣUDS����
                    //ֹͣBUSOFF�ָ���ʱ��
                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_SLEEP_MODE_STATE;//��������״̬
                    receive_RMB_Flag[i] = RECEIVE_NONE;
                    request_NM_Repeat_Message_Flag[i] = Request_Repeat_NONE;
					canPort_Listen(i);  // add 20190903  // NM ����ʱ��ʹCAN�ڴ��ڼ���״̬��

                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!CAN %d Be in SLEEP MODE STATE", i);
                }

                break;

            case NM_REPEATE_MSG_NORMAL_STATE:
                /*�Ա�״̬���õĽ��������־�������ά��*/
                receive_NM_PDU_Flag[i] = RECEIVE_NONE;
                receive_RMB_Flag[i] = RECEIVE_NONE;

                /*��״̬���ǿ��Զ�UDS��ϱ��Ľ�����Ӧ�ģ�����Ӧ�ķ����Ǵ�UDS��ʱ��*/
                if ((receive_UDS_Message[i] & UDS_TYPE1) != 0)
                {
                    restart_timer_UDS_Type1(i, T_WAIT_DIAG_REQ);
                    printk(PRINTK_LEVEL_ERROR, "CAN %d Start UDS timer", i);
                    receive_UDS_Message[i] &= (~UDS_TYPE1);// ������յ�UDS��ϱ�־
                    during_UDS[i] = 1;// ��ʾ �Ѿ���UDS����ڼ�

                }

                if (T_REPEAT_MESSAG_ExpiredFlag[i] == Expired_OK) //T_REPEAT_MESSAG ��ʱ�������һ����Ҫ�뿪REPEAT MESSAGE״̬��
                {
                    T_REPEAT_MESSAG_ExpiredFlag[i] = Expired_NONE;// ��ն�ʱ �������־��
                    stop_timer_T_REPEAT_MESSAGE(i);//ֹͣ T_REPEAT_MESSAGE��ʱ������ΪҪ�뿪 REPEAT_REPEAT״̬��

                    if (NM_Local_WakeupCondition.current_state == SignalLine_OK)
                    {
                        nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                        nm_canState[i].nm_currentState = NM_NORMAL_STATE;
						 nm_canState[i].stateChange = NORMAL_FROM_RM;//HZ
                        printk(PRINTK_LEVEL_LOG, "STATE CHANGE!CAN %d Be in NORMAL OPERATION STATE", i);
                        //��PDU���ļ������Ϳ��ԣ�������Ӳ�����
                        //��������������ģ�����Ҫ��Ӳ�����
                        break;
                    }

                    if (NM_Local_WakeupCondition.current_state == SignalLine_DIS)
                    {
                        nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                        nm_canState[i].nm_currentState = NM_READY_SLEEP_STATE;
                        printk(PRINTK_LEVEL_LOG, "STATE CHANGE!CAN %d Be in READY SLEEP STATE STATE", i);
                        stop_timer_SEND_PDU(i);//ֹͣ����PDU����
                        break;
                    }
                    else {}
                }

                if (T_NM_TIMEROUT_ExpiredFlag[i] == Expired_OK)
                {
                    T_NM_TIMEROUT_ExpiredFlag[i] = Expired_NONE;// ��ն�ʱ�������־��
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);//  ������ʱ��
                    break;
                }

                break;

            case NM_REPEATE_MSG_IMMEDIATE_STATE:
                /*�Ա�״̬���õĽ��������־�������ά��*/
                receive_NM_PDU_Flag[i] = RECEIVE_NONE;
                receive_RMB_Flag[i] = RECEIVE_NONE;

                if ((receive_UDS_Message[i] & UDS_TYPE1) != 0)
                {
                    restart_timer_UDS_Type1(i, T_WAIT_DIAG_REQ);
                    printk(PRINTK_LEVEL_ERROR, "CAN %d Start UDS timer", i);
                    receive_UDS_Message[i] &= (~UDS_TYPE1);// ������յ�UDS��ϱ�־
                    during_UDS[i] = 1;// ��ʾ �Ѿ���UDS����ڼ�
                }

                //  ���߼�ֻ����NM_REPEATE_MSG_NORMAL_STATEת��һ�����ܣ��ڶ�ʱ������������
                break;

            case NM_NORMAL_STATE:
                /*�Ա�״̬���õĽ��������־�������ά��*/
                receive_NM_PDU_Flag[i] = RECEIVE_NONE;

                if ((receive_UDS_Message[i] & UDS_TYPE1) != 0)
                {
                    restart_timer_UDS_Type1(i, T_WAIT_DIAG_REQ);
                    printk(PRINTK_LEVEL_ERROR, "CAN %d Start UDS timer", i);
                    receive_UDS_Message[i] &= (~UDS_TYPE1);// ������յ�UDS��ϱ�־
                    during_UDS[i] = 1;// ��ʾ �Ѿ���UDS����ڼ�
                }

                if (request_NM_Repeat_Message_Flag[i] == Request_Repeat_YES)
                {
                    request_NM_Repeat_Message_Flag[i] = Request_Repeat_NONE;//��� ��������REPEAT MESSAGE ��־λ

                    /*ͬʱҲ���  ��������REPEAT MESSAGE��־λ����Ϊ�������� ��ȱ����������ȼ��ߣ������ͬʱ���֣�����������Ϊ׼*/
                    receive_RMB_Flag[i] = RECEIVE_NONE;
                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_IMMEDIATE_STATE;//������ٷ���PDU״̬
                    nm_canState[i].stateChange = RM_FROM_NORMAL;//������״̬�£�����REPEAT MESSAGE״̬��HZ.
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in IMMEDIATE REPEAT MESSAGE STATE", i);
                    restart_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE);

                    //PDU��������
                    //  can_send(i, MAIL_TRY, &NM_PDU[i]);//�ֶ���һ��PDU����
                    PDU_CAN_Send(i);//�ֶ�����PDU
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);
                    restart_timer_SEND_PDU(i, T_NM_ImmediateCycleTime); //��������PDU���ĵĶ�ʱ��������Ϊ�������ڡ�
                    break;
                }

                if (receive_RMB_Flag[i] == RECEIVE_OK)
                {
                    receive_RMB_Flag[i] = RECEIVE_NONE;//�� ����REPEAT MESSAGE BIT ��־
                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_NORMAL_STATE;//
                    nm_canState[i].stateChange = RM_FROM_NORMAL;//������״̬�£�����REPEAT MESSAGE״̬��HZ.
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in NORMAL REPEAT MESSAGE STATE", i);
                    restart_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE);

                    //PDU��������
                    //  can_send(i, MAIL_TRY, &NM_PDU[i]);//�ֶ���һ��PDU����
                    PDU_CAN_Send(i);//�ֶ�����PDU
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);
                 //   restart_timer_SEND_PDU(i, T_NM_ImmediateCycleTime); //��������PDU���ĵĶ�ʱ��������Ϊ���ٷ���PDU
                    restart_timer_SEND_PDU(i, T_NM_MessageCycle); //��������PDU���ĵĶ�ʱ�� 20190903
                    break;
                }

                /*������������״̬�£��ܴ���״̬�ı�� ����������
                1����������REPEAT MESSAGE
                2�����յ�REPEAT MESSAGE ����
                3��ON�߶�ʧ
                �����ȼ�Ϊ���������� REPEAT MEAAGE > ���յ�REPEAT MESSAGE > ON�߶�ʧ*/

                if (NM_Local_WakeupCondition.current_state == SignalLine_DIS)
                {
                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_READY_SLEEP_STATE;//������������PDU����PDU״̬
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in READY SLEEP STATE", i);
                    stop_timer_SEND_PDU(i);//ֹͣ���� PDU
                    break;
                }

                if (T_NM_TIMEROUT_ExpiredFlag[i] == Expired_OK) // T_NM_TIMEROUT�����״̬���ı�
                {
                    T_NM_TIMEROUT_ExpiredFlag[i] = Expired_NONE;//��� ��ʱ�������־
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);
                    break;
                }

                break;

            case NM_READY_SLEEP_STATE:
				receive_NM_PDU_Flag[i] = 0;//�յ���ͨNM��������Ч�ġ�
                if ((receive_UDS_Message[i] & UDS_TYPE1) != 0)
                {
                    restart_timer_UDS_Type1(i, T_WAIT_DIAG_REQ);
                    printk(PRINTK_LEVEL_ERROR, "CAN %d Start UDS timer", i);
                    receive_UDS_Message[i] &= (~UDS_TYPE1);// ������յ�UDS��ϱ�־
                    during_UDS[i] = 1;// ��ʾ �Ѿ���UDS����ڼ�
                }

                if (request_NM_Repeat_Message_Flag[i] == Request_Repeat_YES)
                {
                    request_NM_Repeat_Message_Flag[i] = Request_Repeat_NONE;//��� ��������REPEAT MESSAGE ��־λ

                    /*ͬʱҲ���  ��������REPEAT MESSAGE��־λ����Ϊ�������� ��ȱ����������ȼ��ߣ������ͬʱ���֣�����������Ϊ׼*/
                    receive_RMB_Flag[i] = RECEIVE_NONE;
                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_IMMEDIATE_STATE;//������ٷ���PDU״̬
                    nm_canState[i].stateChange = RM_FROM_R_SLEEP;//��׼�����ߵ�״̬�£����е�REPEAT MSG״̬��HZ
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in IMMEDIATE REPEAT MESSAGE STATE", i);
                    restart_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE);

                    //PDU��������
                    //  can_send(i, MAIL_TRY, &NM_PDU[i]);//�ֶ���һ��PDU����
                    PDU_CAN_Send(i);//�ֶ�����PDU
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);
                    restart_timer_SEND_PDU(i, T_NM_ImmediateCycleTime); //��������PDU���ĵĶ�ʱ��������Ϊ�������ڡ�
                    break;
                }

                if (receive_RMB_Flag[i] == RECEIVE_OK)
                {
                    receive_RMB_Flag[i] = RECEIVE_NONE;//�� ����REPEAT MESSAGE BIT ��־
                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_NORMAL_STATE;//������������PDU����PDU״̬
                    nm_canState[i].stateChange = RM_FROM_R_SLEEP;//��׼�����ߵ�״̬�£����е�REPEAT MSG״̬��HZ
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in NORMAL REPEAT MESSAGE STATE", i);
                    restart_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE);

                    //PDU��������
                    //  can_send(i, MAIL_TRY, &NM_PDU[i]);//�ֶ���һ��PDU����
                    PDU_CAN_Send(i);//�ֶ�����PDU
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);
                    restart_timer_SEND_PDU(i, T_NM_MessageCycle); //��������PDU���ĵĶ�ʱ��������Ϊ�������ڡ�
                    break;
                }

                if (NM_Local_WakeupCondition.current_state == SignalLine_OK)
                {

					PDU_CAN_Send(i);//�ֶ�����PDUnm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_NORMAL_STATE;
					nm_canState[i].stateChange = NORMAL_FROM_R_SLEEP;//��׼�����ߵ�״̬�£����뵽����״̬����HZ
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in NORMAL OPERATION STATE", i);
                    start_timer_SEND_PDU(i, T_NM_MessageCycle); //  �˵ز���Ҫ��������
                }

                /*û����UDS����ڼ䣬�ٿ��Խ������� ��������ڼ䣬����������*/
                if ((T_NM_TIMEROUT_ExpiredFlag[i] == Expired_OK) && (during_UDS[i] == 0))
                {
                    T_NM_TIMEROUT_ExpiredFlag[i] = Expired_NONE;//��ն�ʱ����־λ
                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_PREPARE_SLEEP_MODE_SATATE;
					
                    nm_canState[i].AWB = Passive_WakeUp;// ���Ĭ��ֵ ����������
                    nm_canState[i].WakeUp_reason = WakeUp_Default;//�뿪����ģʽʱ����ջ���ԭ��
                    //  nm_canState[i].stayAwake_reason = StayAwake_Default;
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in PREPARE SLEEP  MODE STATE", i);
                    stop_timer_T_NM_TIMEROUT(i);//�����Ѿ��뿪����״̬�������ʱ���Ѿ�û�д��ڵı�Ҫ��
                    restart_timer_T_WAIT_BUS_SLEEP(i, T_WAIT_BUS_SLEEP); // �������߶�ʱ��
                    stop_timer_SEND_PDU(i);// Ԥ����״̬��ֹͣ���� PDU�ġ�
                    //broadcast_stopTimer(i);//����ֹͣ���ķ��Ͷ�ʱ����
                    stop_BC_timerByChl(i);

                    break;
                }

                break;

            default:
                break;
        }
    }
} 

#endif  



