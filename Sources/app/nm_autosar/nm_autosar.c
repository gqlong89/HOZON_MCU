



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

extern LOCAL_WAKEUPCOND_ST NM_Local_WakeupCondition;//CAN 网络本地唤醒条件

extern CAN_BASIC_CFG_ST can_basic_cfg;  // 外部定义的CAN基本 配置 变量 。

extern unsigned char canBasic_taskid;
extern CAN_Type *const g_flexcanBase[];


unsigned char  nm_AutosarEN[2];      //  定义两人变量 ，表示CAN通道是否允许使用NM功能。
CAN_PORT_STATE nm_canState[2];/*定义两个CAN网络状态变量*/


unsigned char  receive_NM_PDU_Flag[2];               // 收到NM报文
unsigned char  receive_RMB_Flag[2];                  //  REPEAT MESSAGE BIT RECEIVE
unsigned char  request_NM_Repeat_Message_Flag[2];    // 主动请求REPEAT报文


unsigned char  T_REPEAT_MESSAG_ExpiredFlag[2];  // T_REPEAT_MESSAG 定时器 溢出标志
unsigned char  T_NM_TIMEROUT_ExpiredFlag[2];    // T_NM_TIMEROUT  定时器 溢出标志
unsigned char  T_WAIT_BUS_SLEEP_ExpiredFlag[2]; //T_WAIT_BUS_SLEEP 定时器 溢出标志。

unsigned char  T_NM_PDU_ExpiredFlag[2];       //  NM  PDU 定时器溢出标志

unsigned char  accelerate_PDU_Flag[2];    // PDU快速发送标志
unsigned char  accelerate_PDU_count[2];   // 快速PDU已经发送的次数

unsigned char  autosar_busSleep_Error[2];  //在休眠状态下，收到总线错误，有可能是有报文。
unsigned char  can_status[2];              //CAN 口状态。

CAN_SEND_MSG   NM_PDU[2];  //  pdu数据

unsigned int   ID_NM_PDU_MIN[2];    //CAN0通道上NM_PDU报文最小值，需根据不同应用来进行设置
unsigned int   ID_NM_PDU_MAX[2];


//  nm所用的定时器
static int8_t timer_T_NM_TIMEROUT_CanPort0 = TIMER_INVALID;   //WORK无功定时器
static int8_t timer_T_REPEAT_MESSAGE_CanPort0 = TIMER_INVALID;//
static int8_t timer_T_WAIT_BUS_SLEEP_CanPort0 = TIMER_INVALID;   //等待休眠定时器

static int8_t timer_T_NM_TIMEROUT_CanPort1 = TIMER_INVALID;   //WORK无功定时器
static int8_t timer_T_REPEAT_MESSAGE_CanPort1 = TIMER_INVALID;//
static int8_t timer_T_WAIT_BUS_SLEEP_CanPort1 = TIMER_INVALID;   //等待休眠定时器

//  发送PDU所用的定时器
static int8_t timer_SEND_PDU_CanPort0 = TIMER_INVALID;// CanPort0 发送PDU报文定时器
static int8_t timer_SEND_PDU_CanPort1 = TIMER_INVALID;// CanPort1 发送PDU报文定时器

#if  1   // 此部分为UDS诊断 标志 和 诊断所用到的定时器
unsigned char  receive_UDS_Message[2];               // 接收到UDS诊断报文
unsigned char  during_UDS[2];//  在UDS诊断期间
static int8_t timer_UDS_Type1[2] = {TIMER_INVALID, TIMER_INVALID}; // uds诊断定时器，即溢出后标志诊断完成。TYPE1为了用来区分有关UDS其它的定时器
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

    during_UDS[0] = 0;// 定时器溢出 ，表示5S已过，不在UDS诊断期间
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

    during_UDS[1] = 0;// 定时器溢出 ，表示5S已过，不在UDS诊断 期间
    ltimer_stop(timer_UDS_Type1[1]);
    printk(PRINTK_LEVEL_ERROR, "CanPort1 UDS Finished!!!");
}

void start_timer_T_NM_TIMEROUT(unsigned char canport, unsigned int timeout) //  开启T_NM_TIMEROUT定时器，需设定溢出时间
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

void stop_timer_T_NM_TIMEROUT(unsigned char canport)  //停止T_NM_TIMEROUT定时器
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

void restart_timer_T_NM_TIMEROUT(unsigned char canport,        unsigned int timeout) //重启T_NM_TIMEROUT，是先停再开，需设定溢出时间
{
    stop_timer_T_NM_TIMEROUT(canport);
    start_timer_T_NM_TIMEROUT(canport, timeout);
}

void start_timer_T_REPEAT_MESSAGE(unsigned char canport, unsigned int timeout) //  开启T_REPEAT_MESSAGE定时器，需设定溢出时间
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

void stop_timer_T_REPEAT_MESSAGE(unsigned char canport)  //停止T_REPEAT_MESSAGE定时器
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
                                    unsigned int timeout) //重启T_REPEAT_MESSAGE，是先停再开，需设定溢出时间
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
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务

}

static void CanPort0_T_REPEAT_MESSAGE_isr(int8_t handle)
{
    if (handle != timer_T_REPEAT_MESSAGE_CanPort0)
    {
        printk(PRINTK_LEVEL_ERROR, "not timer_T_REPEAT_MESSAGE");
        return;
    }

    T_REPEAT_MESSAG_ExpiredFlag[0] = Expired_OK;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
}

static void CanPort0_T_WAIT_BUS_SLEEP_isr(int8_t handle)
{
    if (handle != timer_T_WAIT_BUS_SLEEP_CanPort0)
    {
        printk(PRINTK_LEVEL_ERROR, "not");
        return;
    }

    T_WAIT_BUS_SLEEP_ExpiredFlag[0] = Expired_OK;
	kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
}

static void CanPort1_T_NM_TIMEROUT_isr(int8_t handle)
{
    if (handle != timer_T_NM_TIMEROUT_CanPort1)
    {
        printk(PRINTK_LEVEL_ERROR, "not timer_T_NM_TIMEROUT");
        return;
    }

    T_NM_TIMEROUT_ExpiredFlag[1] = Expired_OK;
	kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务

}

static void CanPort1_T_REPEAT_MESSAGE_isr(int8_t handle)
{
    if (handle != timer_T_REPEAT_MESSAGE_CanPort1)
    {
        printk(PRINTK_LEVEL_ERROR, "not timer_T_REPEAT_MESSAGE");
        return;
    }

    T_REPEAT_MESSAG_ExpiredFlag[1] = Expired_OK;
	kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
}

static void CanPort1_T_WAIT_BUS_SLEEP_isr(int8_t handle)
{
    if (handle != timer_T_WAIT_BUS_SLEEP_CanPort1)
    {
        printk(PRINTK_LEVEL_ERROR, "not");
        return;
    }

    T_WAIT_BUS_SLEEP_ExpiredFlag[1] = Expired_OK;
	kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
}

void canPort_Listen(unsigned     char canport) //  使CAN口处于 监听状态
{
	CAN_Type *base = g_flexcanBase[canport];
	FLEXCAN_HAL_SetOperationMode(base, FLEXCAN_LISTEN_ONLY_MODE); //使CAN口处监听状态
	can_status[canport] = CAN_STATUS_LISTEN;
  	flexcan_clear_errstatus(canport);
    flexcan_set_errint(canport, true);                           //设置允许CAN错误中断//  Deinit can控制器。20190903
    autosar_busSleep_Error[canport] = 0;   //  清空　错误标志。
}

void canPort_Normal(unsigned char canport)   // 使CAN口处于 正常状态
{
	CAN_Type *base = g_flexcanBase[canport];
    FLEXCAN_HAL_SetOperationMode(base, FLEXCAN_NORMAL_MODE); //使CAN口处于正常工作状态
    can_status[canport] = CAN_STATUS_NORMAL;
}

void CanBus_ErrorCheck_AUTUSAR(unsigned char canport)
{
    autosar_busSleep_Error[canport] = 1;    //  总线上有错误。
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
		T_NM_PDU_ExpiredFlag[canport] = 0;// 清除等发送的PDU
    }

    if (canport == 1)
    {
        htimer_stop(timer_SEND_PDU_CanPort1);
		T_NM_PDU_ExpiredFlag[canport] = 0;// 清除等发送的PDU
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

    T_NM_PDU_ExpiredFlag[0] = T_NM_PDU_ExpiredFlag[0] | PDUTYPE1;// 要发送第一种类型的PDU

    Autosar_PDU_CAN0_SendHandle();//只能在中断里发送PDU，否则在快速PDU时，20MS周期无法达 到准确。

   // kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST3);//触发任务
}

static void CanPort1_SEND_PDU_isr(int8_t handle)
{
    if (handle != timer_SEND_PDU_CanPort1)
    {
        printk(PRINTK_LEVEL_ERROR, "not");
        return;
    }

    T_NM_PDU_ExpiredFlag[1] = T_NM_PDU_ExpiredFlag[1] | PDUTYPE1;
	
	Autosar_PDU_CAN1_SendHandle();//只能在中断里发送PDU，否则在快速PDU时，20MS周期无法达 到准确。

	//kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST3);//触发任务
}


void Autosar_PDU_CAN0_SendHandle(void)
{
    if (T_NM_PDU_ExpiredFlag[0] == 0) // 没有溢出，即没有发送请求
    {
        return;
    }

    if (T_NM_PDU_ExpiredFlag[0] & PDUTYPE1)
    {
        T_NM_PDU_ExpiredFlag[0] &= (~PDUTYPE1); //清除PDU标志

        //  can_send(CAN_PORT0, MAIL_TRY, &NM_PDU[0]);  // 发送PDU
        PDU_CAN_Send(CAN_PORT0);//发送PDU
        
		restart_timer_T_NM_TIMEROUT(CAN_PORT0, T_NM_TIMEOUT);// 只要发送NM报文 ，就重启定时器

        if (accelerate_PDU_Flag[0] == accelerate_YES) // 判断 是不是加速PDU
        {
            accelerate_PDU_count[0] = accelerate_PDU_count[0] + 1;

            if (accelerate_PDU_count[0] >= (N_ImmediateNM_TIMES - 1)) // 因为在状态转换函数中手动发了一条
            {
                accelerate_PDU_count[0] = 0;
                accelerate_PDU_Flag[0] = accelerate_NONE;
                //accelerate_PDU_count[0] = 0;
                restart_timer_SEND_PDU(CAN_PORT0, T_NM_MessageCycle);

                nm_canState[0].nm_lastState = nm_canState[0].nm_currentState;//这个地方要 进行一个状态转换。
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
    if (T_NM_PDU_ExpiredFlag[1] == 0) // 没有溢出，即没有发送请求
    {
        return;
    }

    if (T_NM_PDU_ExpiredFlag[1] & PDUTYPE1)
    {
        T_NM_PDU_ExpiredFlag[1] &= (~PDUTYPE1); //  清除PDU标志

        //  can_send(CAN_PORT1, MAIL_TRY, &NM_PDU[1]);// 发送PDU
        PDU_CAN_Send(CAN_PORT1);//发送PDU
        restart_timer_T_NM_TIMEROUT(CAN_PORT1, T_NM_TIMEOUT);// 只要发送NM报文 ，重启定时器

        if (accelerate_PDU_Flag[1] == accelerate_YES) // 判断 是不是加速PDU
        {
            accelerate_PDU_count[1] = accelerate_PDU_count[1] + 1;

            if (accelerate_PDU_count[1] >= (N_ImmediateNM_TIMES - 1)) // 因为在状态转换函数中手动发了一条
            {
                accelerate_PDU_count[1] = 0;
                accelerate_PDU_Flag[1] = accelerate_NONE;
                //  accelerate_PDU_count[1] = 0;
                restart_timer_SEND_PDU(CAN_PORT1, T_NM_MessageCycle);

                nm_canState[1].nm_lastState = nm_canState[1].nm_currentState;//这个地方要 进行一个状态转换。
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

    if (check_busOffState(canport) == 1) // 查看是否是BUSOFF了 在为发送首帧 PDU 的时要用此函数 ，故要判断BUSOFF
    {
        return;
    }

    NM_PDU[canport].Data[1] = 0;// 包涵主动请求REPEAT MSG标志，和主动唤醒标志//先清0,再封装
    
    NM_PDU[canport].Data[1] |= request_NM_Repeat_Message_Flag[canport];
	NM_PDU[canport].Data[1] |= nm_canState[canport].AWB;
    NM_PDU[canport].Data[2] = nm_canState[canport].WakeUp_reason;        // 唤醒原因

 //   if ((nm_canState[canport].nm_currentState == NM_REPEATE_MSG_IMMEDIATE_STATE)
 //       || (nm_canState[canport].nm_currentState == NM_REPEATE_MSG_NORMAL_STATE))
 //   {
 //       NM_PDU[canport].Data[3] = 0;// 是REPEAT MESSAGE 状态
 //   }
  //  else
  //  {
  //      NM_PDU[canport].Data[3] = 1;  // 非REPEAT MESSAGE 状态
  //  }
    NM_PDU[canport].Data[3] = nm_canState[canport].stateChange;
    NM_PDU[canport].Data[4] = nm_canState[canport].StayAwake_reason;

    can_send(canport, TX_CAN_NORMAL_NM_AUTOSAR, &NM_PDU[canport]);//手动发一条PDU报文
    return ;
}

void NM_Autosar_PDU_Init(unsigned char canport)//PDU数据初始化。
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

    NM_PDU[canport].Data[0] = 0x01;     //  TBOX节点标识符
    //  NM_PDU[0].Data[1] = 1;        // 包涵主动请求REPEAT MSG标志，和主动唤醒标志
    //  NM_PDU[0].Data[2] = 1;        // 唤醒原因
    NM_PDU[canport].Data[3] = 0;        //  是否是REPEAT MESSAGE状态
    //  NM_PDU[0].Data[4] = 1;        // 保持唤醒的原因
    NM_PDU[canport].Data[5] = 0;
    NM_PDU[canport].Data[6] = 0;
    NM_PDU[canport].Data[7] = 0;

}


//  返回0 没有收到NM_PDU
//  返回1 收到NM_PDU
unsigned char NM_PDU_ReceiveCheck(unsigned char canport, CAN_MSG *tCANMsg)
{
    if (nm_AutosarEN[canport] == ENABLE)
    {
        if ((ID_NM_PDU_MIN[canport] <= (tCANMsg->MsgID & (unsigned int)0x7ff))
            && ((tCANMsg->MsgID & (unsigned int)0x7ff)) <= ID_NM_PDU_MAX[canport])
        {
			
			restart_timer_T_NM_TIMEROUT(canport, T_NM_TIMEOUT);//  只要收到NM报文，重启定时器

			receive_NM_PDU_Flag[canport] = RECEIVE_OK;
            kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
       //     printk(PRINTK_LEVEL_LOG, "can %d receive NM_PDU", canport);

            if ((tCANMsg->Data[1] & 0x01) != 0) // 收到 REPEAT MESSAGE BIT
            {
                receive_RMB_Flag[canport] = RECEIVE_OK;
                kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//触发任务
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
        nm_canState[canport].WakeUp_reason = WakeUp_Default;         //初始化 唤醒原因为空（默认为空 ）
        nm_canState[canport].StayAwake_reason = StayAwake_Default;   //初始化 保持唤醒原因
        nm_canState[canport].AWB = Passive_WakeUp;                   //初始化 默认为被动唤醒 （DSC）// 主动唤醒位
        printk(PRINTK_LEVEL_LOG, "POWER ON!CAN %d Be in sleep Mode", canport);
    }
    else
    {
        printk(PRINTK_LEVEL_LOG, "CAN %d has no AutoSar Function!", canport);
    }

    NM_Autosar_PDU_Init(canport);      // PDU初始化
//    PDU_CAN_Send(canport);             // 发送首帧PDU。


 #if 1  // 这部分为通常情况下触发网络状态改变的7个标志

    autosar_busSleep_Error[canport] = 0;  //  初始化，没有收到总线错误。

	can_status[canport] = CAN_STATUS_NORMAL;   //  初始化，can 口状态为　NORMAL

    receive_NM_PDU_Flag[canport] = RECEIVE_NONE;// 初始化，没有收到PDU报文

    receive_RMB_Flag[canport] = RECEIVE_NONE;   // 初始化，没有没有收到 REPEAT 标志位

    request_NM_Repeat_Message_Flag[canport] = Request_Repeat_NONE;//初始化，没有主动请求PEPEAT MESSAGE

    receive_UDS_Message[canport] = 0;// 初始化，CAN0没有收到诊断报文

    during_UDS[canport] = 0;//初始化，表示 CAN0没有在UDS诊断期间

    T_NM_TIMEROUT_ExpiredFlag[canport] = Expired_NONE;  // 初始化，定时器没有溢出

    T_REPEAT_MESSAG_ExpiredFlag[canport] = Expired_NONE;  // 初始化，定时器没有溢出

    T_WAIT_BUS_SLEEP_ExpiredFlag[canport] = Expired_NONE;  //初始化，定时器没有溢出

    #endif

    accelerate_PDU_Flag[canport] = accelerate_NONE;//初始化，PDU 不加速


    accelerate_PDU_count[canport] = 0;  // 初始化，已发送的加速PDU数量为0

    T_NM_PDU_ExpiredFlag[canport] = 0;

    //创建NM定时器

    if (nm_AutosarEN[0] == ENABLE)
    {
        if (TIMER_INVALID == timer_T_NM_TIMEROUT_CanPort0)
        {
            timer_T_NM_TIMEROUT_CanPort0 = ltimer_create(CanPort0_T_NM_TIMEROUT_isr);//只创建，没有开启
        }

        if (TIMER_INVALID == timer_T_REPEAT_MESSAGE_CanPort0)
        {
            timer_T_REPEAT_MESSAGE_CanPort0 = ltimer_create(CanPort0_T_REPEAT_MESSAGE_isr);//只创建，没有开启
        }

        if (TIMER_INVALID == timer_T_WAIT_BUS_SLEEP_CanPort0)
        {
            timer_T_WAIT_BUS_SLEEP_CanPort0 = ltimer_create(CanPort0_T_WAIT_BUS_SLEEP_isr);//只创建，没有开启
        }

        if (TIMER_INVALID == timer_SEND_PDU_CanPort0)
        {
            timer_SEND_PDU_CanPort0 = htimer_create(CanPort0_SEND_PDU_isr);//只创建，没有开启
        }

        if (TIMER_INVALID == timer_UDS_Type1[0])
        {
            timer_UDS_Type1[0] = ltimer_create(USD_Type1_PORT0_isr);//只创建，没有开启
        }
    }

    if (nm_AutosarEN[1] == ENABLE)
    {
        if (TIMER_INVALID == timer_T_NM_TIMEROUT_CanPort1)
        {
            timer_T_NM_TIMEROUT_CanPort1 = ltimer_create(CanPort1_T_NM_TIMEROUT_isr);//只创建，没有开启

        }

        if (TIMER_INVALID == timer_T_REPEAT_MESSAGE_CanPort1)
        {
            timer_T_REPEAT_MESSAGE_CanPort1 = ltimer_create(CanPort1_T_REPEAT_MESSAGE_isr);//只创建，没有开启
        }

        if (TIMER_INVALID == timer_T_WAIT_BUS_SLEEP_CanPort1)
        {
            timer_T_WAIT_BUS_SLEEP_CanPort1 = ltimer_create(CanPort1_T_WAIT_BUS_SLEEP_isr);//只创建，没有开启
        }

        if (TIMER_INVALID == timer_SEND_PDU_CanPort1)
        {
            timer_SEND_PDU_CanPort1 = htimer_create(CanPort1_SEND_PDU_isr);//只创建，没有开启
        }

        if (TIMER_INVALID == timer_UDS_Type1[1])
        {
            timer_UDS_Type1[1] = ltimer_create(USD_Type1_PORT1_isr);//只创建，没有开启
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
            continue;  // 本次循环不执行了
        }

        switch (nm_canState[i].nm_currentState)
        {
            case NM_SLEEP_MODE_STATE://当前状态为休眠状态

				
                /*对本状态无用的解决条件标志进行清空维护*/

                receive_UDS_Message[i] = RECEIVE_NONE;//丢掉一切UDS诊断标志

                if(autosar_busSleep_Error[i] == 1)
                {
                    autosar_busSleep_Error[i] = 0;
                    canPort_Normal(i);
                    break;
                }

                if (NM_Local_WakeupCondition.current_state != 0)
                {
                    nm_canState[i].AWB = Active_WakeUp;// 主动唤醒
					canPort_Normal(i);	  //  

                    //增加唤醒原因
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
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_IMMEDIATE_STATE;//状态切换
                    nm_canState[i].stateChange = RM_FROM_SLEEP;//从SLPPE状态进行 REPEAT MESSAGE。HZ
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in IMMEDIATE REPEAT MESSAGE STATE", i);
                    /*因为通过接收到PDU报文来改变网络状态的优先级没有主动唤醒优先级高*/

                    receive_NM_PDU_Flag[i] = RECEIVE_NONE;

                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);   //开启T_NM_TIMEROUT Timer
                    restart_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE); //开启T_REPEAT_MESSAGE Timer

                    /*进入REPEAT 状态，要在10MS以内发出一条PDU,用定时器发时间太慢，所以手动发一条*/
                    PDU_CAN_Send(i);// 手动发送一个PDU报文
                
                    /*主动唤醒，所以要快速发送*/
                    start_timer_SEND_PDU(i, T_NM_ImmediateCycleTime); //开启发送PDU报文的定时器，参数为快速周期。				

                    first_BC(i); // 手动发送首帧报文

					/*成功发送一条PUD，要重启   WORK	无功定时器*/
					restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);

                    start_BC_timerByChl(i);
                    break;
                }

                if (receive_NM_PDU_Flag[i] == RECEIVE_OK)
                {
                    receive_NM_PDU_Flag[i] = RECEIVE_NONE;// 清空状态，此报文已经被处理
                    /*成功收到一条PUD，要重启   WORK  无功定时器*/

                    nm_canState[i].AWB = Passive_WakeUp;// 被动唤醒

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
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_NORMAL_STATE;//状态切换
                    nm_canState[i].stateChange = RM_FROM_SLEEP;//从SLPPE状态进行 REPEAT MESSAGE。HZ
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!CAN %d Be in NM_REPEATE_MSG_NORMAL_STATE", i);

                    start_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT); //开启T_NM_TIMEROUT Timer
                    start_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE);  //开启T_REPEAT_MESSAGE Timer

                    /*进入REPEAT 状态，要在10MS以内发出一条PDU,用定时器发时间太慢，所以手动发一条*/
                    //  can_send(i, MAIL_TRY, &NM_PDU[i]);//手动发一条PDU报文
                    PDU_CAN_Send(i);
                    /*成功发送一条PUD，要重启   WORK  无功定时器*/
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);

                    /*被动唤醒，所以要以正常速度发送*/
                    start_timer_SEND_PDU(i, T_NM_MessageCycle); //开启发送PDU报文的定时器，正常速度

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
                /*对本状态无用的解决条件标志进行清空维护*/
                receive_RMB_Flag[i] = RECEIVE_NONE;
                request_NM_Repeat_Message_Flag[i] = Request_Repeat_NONE;
                receive_UDS_Message[i] = 0;//丢掉一切UDS诊断标志

                if (NM_Local_WakeupCondition.current_state == SignalLine_OK)
                {
                    nm_canState[i].AWB = Active_WakeUp;// 主动唤醒

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
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_IMMEDIATE_STATE;//状态切换
                    nm_canState[i].stateChange = RM_FROM_P_SLEEP;//从SLPPE状态进行 REPEAT MESSAGE。HZ
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in IMMEDIATE REPEAT MESSAGE STATE", i);

                    start_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT); //开启T_NM_TIMEROUT Timer
                    start_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE);  //开启T_REPEAT_MESSAGE Timer

                    /*进入REPEAT 状态，要在10MS以内发出一条PDU,用定时器发时间太慢，所以手动发一条*/
                    //  can_send(i, MAIL_TRY, &NM_PDU[i]);//手动发一条PDU报文
                    PDU_CAN_Send(i);//手动发送PDU
                    /*成功发送一条PUD，要重启   WORK  无功定时器*/
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);
                    /*主动唤醒，所以要快速发送*/
                    start_timer_SEND_PDU(i, T_NM_ImmediateCycleTime); //开启发送PDU报文的定时器，参数为快速周期。

                   // broadcast_firstFrame(i);//  手动发送广播报文
                    first_BC(i);

                    //broadcast_startTimer(i);//开启应用报文定时器// 第一帧PDU与第一帧应用报文间隔要小于20MS。
                    start_BC_timerByChl(i);
                    break;
                }

                if (receive_NM_PDU_Flag[i] == RECEIVE_OK)
                {
                    receive_NM_PDU_Flag[i] = RECEIVE_NONE;// 清空状态，此报文已经被处理
                    /*成功收到一条PUD，要重启   WORK  无功定时器*/
                    nm_canState[i].AWB = Passive_WakeUp;//被动唤醒

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
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_NORMAL_STATE;//状态切换
                     nm_canState[i].stateChange = RM_FROM_P_SLEEP;//从SLPPE状态进行 REPEAT MESSAGE。HZ
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!CAN %d Be in NORMAL REPEAT MESSAGE STATE", i);

                    start_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT); //开启T_NM_TIMEROUT Timer
                    start_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE);  //开启T_REPEAT_MESSAGE Timer

                    /*进入REPEAT 状态，要在10MS以内发出一条PDU,用定时器发时间太慢，所以手动发一条*/
                    //can_send(i, MAIL_TRY, &NM_PDU[i]);//手动发一条PDU报文
                    PDU_CAN_Send(i);//手动发送PDU
                    /*成功发送一条PUD，要重启   WORK  无功定时器*/
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);

                    /*被动唤醒，所以要以正常速度发送*/
                    start_timer_SEND_PDU(i, T_NM_MessageCycle); //开启发送PDU报文的定时器，正常速度


					//broadcast_firstFrame(i);//  手动发送广播报文
					first_BC(i);
                   // broadcast_startTimer(i);//开启应用报文定时器// 第一帧PDU与第一帧应用报文间隔要小于20MS。
					start_BC_timerByChl(i);
				    break;
                }

                if (T_WAIT_BUS_SLEEP_ExpiredFlag[i] == Expired_OK)
                {
                    T_WAIT_BUS_SLEEP_ExpiredFlag[i] = Expired_NONE;

                    T_NM_TIMEROUT_ExpiredFlag[i] = Expired_NONE;  // 初始化，定时器没有溢出

                    T_REPEAT_MESSAG_ExpiredFlag[i] = Expired_NONE;  // 初始化，定时器没有溢出

                    /*停止所有CAN发送定时器*/
                    stop_timer_T_NM_TIMEROUT(i);
                    stop_timer_T_REPEAT_MESSAGE(i);
                    stop_timer_T_WAIT_BUS_SLEEP(i);
                    stop_timer_SEND_PDU(i);
                  //  broadcast_stopTimer(i);//停止广播发送定时器
                    stop_BC_timerByChl(i);
                    //停止UDS发送
                    //停止BUSOFF恢复定时器
                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_SLEEP_MODE_STATE;//进入休眠状态
                    receive_RMB_Flag[i] = RECEIVE_NONE;
                    request_NM_Repeat_Message_Flag[i] = Request_Repeat_NONE;
					canPort_Listen(i);  // add 20190903  // NM 休眠时，使CAN口处于监听状态。

                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!CAN %d Be in SLEEP MODE STATE", i);
                }

                break;

            case NM_REPEATE_MSG_NORMAL_STATE:
                /*对本状态无用的解决条件标志进行清空维护*/
                receive_NM_PDU_Flag[i] = RECEIVE_NONE;
                receive_RMB_Flag[i] = RECEIVE_NONE;

                /*此状态下是可以对UDS诊断报文进行响应的，且响应的方法是打开UDS定时器*/
                if ((receive_UDS_Message[i] & UDS_TYPE1) != 0)
                {
                    restart_timer_UDS_Type1(i, T_WAIT_DIAG_REQ);
                    printk(PRINTK_LEVEL_ERROR, "CAN %d Start UDS timer", i);
                    receive_UDS_Message[i] &= (~UDS_TYPE1);// 清除接收到UDS诊断标志
                    during_UDS[i] = 1;// 表示 已经在UDS诊断期间

                }

                if (T_REPEAT_MESSAG_ExpiredFlag[i] == Expired_OK) //T_REPEAT_MESSAG 定时器溢出，一定是要离开REPEAT MESSAGE状态的
                {
                    T_REPEAT_MESSAG_ExpiredFlag[i] = Expired_NONE;// 清空定时 器溢出标志。
                    stop_timer_T_REPEAT_MESSAGE(i);//停止 T_REPEAT_MESSAGE定时器，因为要离开 REPEAT_REPEAT状态。

                    if (NM_Local_WakeupCondition.current_state == SignalLine_OK)
                    {
                        nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                        nm_canState[i].nm_currentState = NM_NORMAL_STATE;
						 nm_canState[i].stateChange = NORMAL_FROM_RM;//HZ
                        printk(PRINTK_LEVEL_LOG, "STATE CHANGE!CAN %d Be in NORMAL OPERATION STATE", i);
                        //让PDU报文继续发就可以，不用另加操作。
                        //如果是立即发报文，则需要另加操作。
                        break;
                    }

                    if (NM_Local_WakeupCondition.current_state == SignalLine_DIS)
                    {
                        nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                        nm_canState[i].nm_currentState = NM_READY_SLEEP_STATE;
                        printk(PRINTK_LEVEL_LOG, "STATE CHANGE!CAN %d Be in READY SLEEP STATE STATE", i);
                        stop_timer_SEND_PDU(i);//停止发送PDU报文
                        break;
                    }
                    else {}
                }

                if (T_NM_TIMEROUT_ExpiredFlag[i] == Expired_OK)
                {
                    T_NM_TIMEROUT_ExpiredFlag[i] = Expired_NONE;// 清空定时器溢出标志。
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);//  重启定时器
                    break;
                }

                break;

            case NM_REPEATE_MSG_IMMEDIATE_STATE:
                /*对本状态无用的解决条件标志进行清空维护*/
                receive_NM_PDU_Flag[i] = RECEIVE_NONE;
                receive_RMB_Flag[i] = RECEIVE_NONE;

                if ((receive_UDS_Message[i] & UDS_TYPE1) != 0)
                {
                    restart_timer_UDS_Type1(i, T_WAIT_DIAG_REQ);
                    printk(PRINTK_LEVEL_ERROR, "CAN %d Start UDS timer", i);
                    receive_UDS_Message[i] &= (~UDS_TYPE1);// 清除接收到UDS诊断标志
                    during_UDS[i] = 1;// 表示 已经在UDS诊断期间
                }

                //  此逻辑只有向NM_REPEATE_MSG_NORMAL_STATE转换一个可能，在定时器服务函数中做
                break;

            case NM_NORMAL_STATE:
                /*对本状态无用的解决条件标志进行清空维护*/
                receive_NM_PDU_Flag[i] = RECEIVE_NONE;

                if ((receive_UDS_Message[i] & UDS_TYPE1) != 0)
                {
                    restart_timer_UDS_Type1(i, T_WAIT_DIAG_REQ);
                    printk(PRINTK_LEVEL_ERROR, "CAN %d Start UDS timer", i);
                    receive_UDS_Message[i] &= (~UDS_TYPE1);// 清除接收到UDS诊断标志
                    during_UDS[i] = 1;// 表示 已经在UDS诊断期间
                }

                if (request_NM_Repeat_Message_Flag[i] == Request_Repeat_YES)
                {
                    request_NM_Repeat_Message_Flag[i] = Request_Repeat_NONE;//清空 主动请求REPEAT MESSAGE 标志位

                    /*同时也清空  被动接收REPEAT MESSAGE标志位。因为，主动请 求比被动接收优先级高，者如果同时出现，以主动请求为准*/
                    receive_RMB_Flag[i] = RECEIVE_NONE;
                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_IMMEDIATE_STATE;//进入快速发送PDU状态
                    nm_canState[i].stateChange = RM_FROM_NORMAL;//从正常状态下，进行REPEAT MESSAGE状态。HZ.
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in IMMEDIATE REPEAT MESSAGE STATE", i);
                    restart_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE);

                    //PDU报文配置
                    //  can_send(i, MAIL_TRY, &NM_PDU[i]);//手动发一条PDU报文
                    PDU_CAN_Send(i);//手动发送PDU
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);
                    restart_timer_SEND_PDU(i, T_NM_ImmediateCycleTime); //开启发送PDU报文的定时器，参数为快速周期。
                    break;
                }

                if (receive_RMB_Flag[i] == RECEIVE_OK)
                {
                    receive_RMB_Flag[i] = RECEIVE_NONE;//清 接收REPEAT MESSAGE BIT 标志
                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_NORMAL_STATE;//
                    nm_canState[i].stateChange = RM_FROM_NORMAL;//从正常状态下，进行REPEAT MESSAGE状态。HZ.
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in NORMAL REPEAT MESSAGE STATE", i);
                    restart_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE);

                    //PDU报文配置
                    //  can_send(i, MAIL_TRY, &NM_PDU[i]);//手动发一条PDU报文
                    PDU_CAN_Send(i);//手动发送PDU
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);
                 //   restart_timer_SEND_PDU(i, T_NM_ImmediateCycleTime); //开启发送PDU报文的定时器，参数为快速发送PDU
                    restart_timer_SEND_PDU(i, T_NM_MessageCycle); //开启发送PDU报文的定时器 20190903
                    break;
                }

                /*在正常工作的状态下，能触发状态改变的 条件有三个
                1：本地请求REPEAT MESSAGE
                2：接收到REPEAT MESSAGE 请求
                3：ON线丢失
                其优先级为：本地请求 REPEAT MEAAGE > 接收到REPEAT MESSAGE > ON线丢失*/

                if (NM_Local_WakeupCondition.current_state == SignalLine_DIS)
                {
                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_READY_SLEEP_STATE;//进入正常发送PDU发送PDU状态
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in READY SLEEP STATE", i);
                    stop_timer_SEND_PDU(i);//停止发送 PDU
                    break;
                }

                if (T_NM_TIMEROUT_ExpiredFlag[i] == Expired_OK) // T_NM_TIMEROUT溢出，状态不改变
                {
                    T_NM_TIMEROUT_ExpiredFlag[i] = Expired_NONE;//清空 定时器溢出标志
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);
                    break;
                }

                break;

            case NM_READY_SLEEP_STATE:
				receive_NM_PDU_Flag[i] = 0;//收到普通NM报文是无效的。
                if ((receive_UDS_Message[i] & UDS_TYPE1) != 0)
                {
                    restart_timer_UDS_Type1(i, T_WAIT_DIAG_REQ);
                    printk(PRINTK_LEVEL_ERROR, "CAN %d Start UDS timer", i);
                    receive_UDS_Message[i] &= (~UDS_TYPE1);// 清除接收到UDS诊断标志
                    during_UDS[i] = 1;// 表示 已经在UDS诊断期间
                }

                if (request_NM_Repeat_Message_Flag[i] == Request_Repeat_YES)
                {
                    request_NM_Repeat_Message_Flag[i] = Request_Repeat_NONE;//清空 主动请求REPEAT MESSAGE 标志位

                    /*同时也清空  被动接收REPEAT MESSAGE标志位。因为，主动请 求比被动接收优先级高，者如果同时出现，以主动请求为准*/
                    receive_RMB_Flag[i] = RECEIVE_NONE;
                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_IMMEDIATE_STATE;//进入快速发送PDU状态
                    nm_canState[i].stateChange = RM_FROM_R_SLEEP;//从准备休眠的状态下，进行到REPEAT MSG状态。HZ
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in IMMEDIATE REPEAT MESSAGE STATE", i);
                    restart_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE);

                    //PDU报文配置
                    //  can_send(i, MAIL_TRY, &NM_PDU[i]);//手动发一条PDU报文
                    PDU_CAN_Send(i);//手动发送PDU
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);
                    restart_timer_SEND_PDU(i, T_NM_ImmediateCycleTime); //开启发送PDU报文的定时器，参数为快速周期。
                    break;
                }

                if (receive_RMB_Flag[i] == RECEIVE_OK)
                {
                    receive_RMB_Flag[i] = RECEIVE_NONE;//清 接收REPEAT MESSAGE BIT 标志
                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_REPEATE_MSG_NORMAL_STATE;//进入正常发送PDU发送PDU状态
                    nm_canState[i].stateChange = RM_FROM_R_SLEEP;//从准备休眠的状态下，进行到REPEAT MSG状态。HZ
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in NORMAL REPEAT MESSAGE STATE", i);
                    restart_timer_T_REPEAT_MESSAGE(i, T_REPEAT_MESSAGE);

                    //PDU报文配置
                    //  can_send(i, MAIL_TRY, &NM_PDU[i]);//手动发一条PDU报文
                    PDU_CAN_Send(i);//手动发送PDU
                    restart_timer_T_NM_TIMEROUT(i, T_NM_TIMEOUT);
                    restart_timer_SEND_PDU(i, T_NM_MessageCycle); //开启发送PDU报文的定时器，参数为正常周期。
                    break;
                }

                if (NM_Local_WakeupCondition.current_state == SignalLine_OK)
                {

					PDU_CAN_Send(i);//手动发送PDUnm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_NORMAL_STATE;
					nm_canState[i].stateChange = NORMAL_FROM_R_SLEEP;//从准备休眠的状态下，进入到正常状态。。HZ
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in NORMAL OPERATION STATE", i);
                    start_timer_SEND_PDU(i, T_NM_MessageCycle); //  此地不需要立即发送
                }

                /*没有在UDS诊断期间，再可以进行休眠 ，在诊断期间，不可以休眠*/
                if ((T_NM_TIMEROUT_ExpiredFlag[i] == Expired_OK) && (during_UDS[i] == 0))
                {
                    T_NM_TIMEROUT_ExpiredFlag[i] = Expired_NONE;//清空定时器标志位
                    nm_canState[i].nm_lastState = nm_canState[i].nm_currentState;
                    nm_canState[i].nm_currentState = NM_PREPARE_SLEEP_MODE_SATATE;
					
                    nm_canState[i].AWB = Passive_WakeUp;// 变回默认值 ，被动唤醒
                    nm_canState[i].WakeUp_reason = WakeUp_Default;//离开工作模式时，清空唤醒原因
                    //  nm_canState[i].stayAwake_reason = StayAwake_Default;
                    printk(PRINTK_LEVEL_LOG, "STATE CHANGE!   CAN %d Be in PREPARE SLEEP  MODE STATE", i);
                    stop_timer_T_NM_TIMEROUT(i);//网络已经离开工作状态，这个定时器已经没有存在的必要。
                    restart_timer_T_WAIT_BUS_SLEEP(i, T_WAIT_BUS_SLEEP); // 开启休眠定时器
                    stop_timer_SEND_PDU(i);// 预休眠状态是停止发送 PDU的。
                    //broadcast_stopTimer(i);//增加停止报文发送定时器。
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



