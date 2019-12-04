

#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <stdint.h>
#include <timer.h>
#include <pm.h>
#include <call.h>
#include <app/power/power.h>
#include <app/can_mgr/can_mgr.h>
#include <adc.h>
#include <rtc.h>
#include <app/can_mgr/can_broadCast.h>
#include <app/power/power.h>
#include <config.h>
#include <app/can_mgr/canbus_off.h>
#include <app/can_mgr/can_basic.h>
#include <app/nm_osek/nm_osekCore.h>
#include <app/can_mgr/can_voltage.h>
#include <app/can_mgr/can_sendFifo.h>
#include <interrupt_manager.h>
#include <app/can_mgr/can_basic_cfg.h>
#include <app/nm_autosar/nm_autosar.h>
#include <string.h>

unsigned char canBasic_taskid = INVALID_TASK_NUMBER;//  本任务ID

LOCAL_WAKEUPCOND_ST NM_Local_WakeupCondition;  


static int8_t timer_CAN_basic_task_trigger = TIMER_INVALID;  //定时触发can_basic任务定时器

static int8_t timer_CANFD_test= TIMER_INVALID;  //CANFD测试定时器。


extern CAN_SENDFIFO_ST  APP_CANSendFifo[2];
extern CAN_BASIC_CFG_ST can_basic_cfg;


CAN_SEND_MSG  canfd_aaa;



#if USED_NM_OSEK
extern NMControlBlock_ST  nm_OsekCB[2];//定义两个nm控制块变量
#endif




/*can_basic 触发定时器中断服务函数*/
static void CAN_basic_task_trigger_isr(int8_t handle)
{
    if (handle != timer_CAN_basic_task_trigger)
    {
        return;
    }

    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);
}

static void timer_CANFD_test_isr(int8_t handle)
{
    if (handle != timer_CANFD_test)
    {
        return;
    }

    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST4);
}



/*更新本地唤醒条件
wp_type:唤醒条件类型。
        当唤醒条件类型为   :WP_TYPE_ON_HD时，state无效，
        程序是自动读取当前硬状态。

        ACC 与  CHARGE 同上。

        当唤醒条件类型为：WP_TYPE_ON_VT时 ,state有效。

 state:唤醒条件状态，只在 唤醒条件类型为 ：WP_TYPE_ON_VT时有效。

 		0：清空
 		1：置位
 		2：不关心，此参数无效

*/
void update_NM_Local_WakeupCondition(WAKEUP_TYPE_E wp_type,unsigned char state)
{
	switch(wp_type)
	{
		case WP_TYPE_ON_HD:
		{
			NM_Local_WakeupCondition.last_state = NM_Local_WakeupCondition.current_state;
			if(pm_get_on_wakeup() == true)
			{
				
				NM_Local_WakeupCondition.current_state |= Signal_ON;
			}
			else
			{
				NM_Local_WakeupCondition.current_state &=(~Signal_ON);
			}
		
			break;
		}
		case WP_TYPE_ON_VT:
		{
			NM_Local_WakeupCondition.last_state = NM_Local_WakeupCondition.current_state;

			if(state == true)
			{
				NM_Local_WakeupCondition.current_state |= Signal_ON_VT;
			}
			else
			{
				NM_Local_WakeupCondition.current_state &=(~Signal_ON_VT);
			}

			break;
		}
#if 0
		case WP_TYPE_CHARGE_HD:
		{
			NM_Local_WakeupCondition.last_state = NM_Local_WakeupCondition.current_state;

			if(pm_get_f_charge_wakeup() == true)
			{
				NM_Local_WakeupCondition.current_state |=Signal_ACC;
			}
			else
			{
				NM_Local_WakeupCondition.current_state &=(~Signal_ACC);
			}
			break;
		}
		case WP_TYPE_ACC_HD:
		{
			NM_Local_WakeupCondition.last_state = NM_Local_WakeupCondition.current_state;

			if(pm_get_acc_wakeup() == true)
			{
				NM_Local_WakeupCondition.current_state |= ACC_HARDWARE;
			}
			else
			{
				NM_Local_WakeupCondition.current_state &=(~ACC_HARDWARE);
			}

			break;
		}

		case WP_TYPE_ALL:
		{

			break;
		}
#endif
		default:
		{
			printk(PRINTK_LEVEL_LOG, "Wrong!!!NO support this wakeup type!!!");
			break;
		}
			
	}
}


/*使用本地唤醒条件，触发网络状态转换*/
void local_Condition_trigger_NM(void)
{
	unsigned char i;


#if USED_NM_OSEK

	if(NM_Local_WakeupCondition.last_state == 0)
	{
		if(NM_Local_WakeupCondition.current_state != 0)  // 满足了唤醒条件
		{
            for (i = 0; i < 2; i++)
            {
                if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_OSEK)
                {
					printk(PRINTK_LEVEL_LOG, "CAN %d NM go to Awake",i);
					GotoMode(i, NM_Awake);
                }
                else if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_NONE)
                {
					start_BC_timerByChl(i);
                }
				else
				{}
            }
		}
	}
	if(NM_Local_WakeupCondition.last_state != 0)
	{
		if(NM_Local_WakeupCondition.current_state == 0)   // 满足了休眠条件
		{
			for(i = 0;i<2;i++)
			{
				if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_OSEK)
          		{
					printk(PRINTK_LEVEL_LOG, "CAN %d NM go to Bussleep",i);
					GotoMode(i, NM_BusSleep);
           	 	}
           		else if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_NONE)
            	{
					stop_BC_timerByChl(i);
            	}
				else
				{}
			}			
		}
	}

#endif


#if USED_NM_AUTOSAR

	if(NM_Local_WakeupCondition.last_state == 0)
	{
		if(NM_Local_WakeupCondition.current_state != 0)  // 满足了唤醒条件
		{
			for (i = 0; i < 2; i++)
			{
				if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_AUTOSAR)
				{

				}
				else if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_NONE)
				{
					start_BC_timerByChl(i);
				}
				else
				{}
			}
		}
	}
	if(NM_Local_WakeupCondition.last_state != 0)
	{
		if(NM_Local_WakeupCondition.current_state == 0)	// 满足了休眠条件
		{
			for(i = 0;i<2;i++)
			{
				if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_AUTOSAR)
				{
				}
				else if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_NONE)
				{
					stop_BC_timerByChl(i);
				}
				else
				{}
			}		
		}
	}
#endif
}

/*开启网络管理*/
static void start_NM(void)
{
	unsigned char i;
	unsigned char  ret = 0;
	for(i = 0;i<2;i++)
	{
		if(can_basic_cfg.nm_cfg[i].nm_type != NM_TYPE_NONE)
		{
			if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_AUTOSAR)
			{
#if USED_NM_AUTOSAR 
				nm_AutosarInit(i);// 开启AUTOSAR 网络管理			
				ret |=(0x01<<0); 
#endif
			}
			else if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_OSEK)
			{
#if USED_NM_OSEK 
				Start_NM_Osek(i);//开启OSEK 网络管理
				ret |=(0x01<<1);
#endif
			}
			else
			{}
		}
		else if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_NONE)
		{
			if(NM_Local_WakeupCondition.current_state != 0)
			{
			  first_BC(i);// 开启广播。
		      start_BC_timerByChl(i);
			}			 
		}
		else
		{}
	}


/*当没有用到相应的NM，而NM开启的，则提示关闭。*/
	if(ret != 0)
	{
		if((ret & (0x01<<0)) ==0 )// 没有用到 autosar_task
		{
			if(USED_NM_AUTOSAR == FUNC_USED)
			{
				taskPrintf(TASK_LEVEL_ERROR, "Unused AUTOSAR NM Function,Please set * USED_NM_AUTOSAR == FUNC_UNUSED * \r\n");
			}
		}

		if((ret & (0x01<<1)) ==0 )// 没有用到 autosar_task
		{
			if(USED_NM_OSEK == FUNC_USED)
			{
				taskPrintf(TASK_LEVEL_ERROR, "Unused OSEK NM Function,Please set * USED_NM_OSEK == FUNC_UNUSED * \r\n");
			}
		}

	}

	if(ret == 0)
	{
		if((USED_NM_AUTOSAR == FUNC_USED)||(USED_NM_OSEK == FUNC_USED))
		{
			taskPrintf(TASK_LEVEL_ERROR, "No NM is need ,Please set * USED_NM_AUTOSAR == FUNC_UNUSED,USED_NM_OSEK == FUNC_UNUSED * \r\n");
		}
	}

}


void run_NM(void)
{

#if USED_NM_AUTOSAR

	nm_AutosarMainFunction();//  运行AUTOSAR 状态转换主函数

#endif

#if USED_NM_OSEK

	nm_OsekMainFunction();//   运行OSEK 状态转换主函数

#endif
}

unsigned int canBasic_task(unsigned int mask)
{
    if (mask & TASK_SYS_EVENT_INIT)
    {
        canBasic_taskid = kernelGetTaskID();

		memset(&NM_Local_WakeupCondition,0,sizeof(NM_Local_WakeupCondition));
		
        can_device_init();   // can 设备初始化。此部分要进行优化。
        
		can_basic_cfg_Init();// can basi 配置初始化。
		
		update_NM_Local_WakeupCondition(WP_TYPE_ON_HD,DONOT_CARE);// 更新本地唤醒条件。
		 
		start_NM();// 开启网络管理。
		
		local_Condition_trigger_NM();
       
        COMM_VolInit();
		COMM_VoltageCheck();// 检测通讯电压范围

		run_NM();

        /*创建定时触发任务定时器*/
        if (TIMER_INVALID == timer_CAN_basic_task_trigger)
        {
            timer_CAN_basic_task_trigger = ltimer_create(CAN_basic_task_trigger_isr);//只创建，没有开启
        }

		if (TIMER_INVALID == timer_CANFD_test)
        {
            timer_CANFD_test = ltimer_create(timer_CANFD_test_isr);//只创建，没有开启
        }

		ltimer_start(timer_CANFD_test, 100);

        /*开启此定时器，10MS触发一次本任务*/
        ltimer_start(timer_CAN_basic_task_trigger, 5);

        taskPrintf(TASK_LEVEL_LOG, "CAN basic task init OK!!!");
        kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);// 此任务在一直存在
        return 0;
    }

/*只有在OSEK的时候，才会使用。*/
#if USED_NM_OSEK

    if (mask & TASK_SYS_EVENT_REQST0) //发送首帧时使用
    {

        mask &= (~TASK_SYS_EVENT_REQST0);// 清除功能码

        if (nm_OsekCB[0].appActiveFlag == 1)
        {
            nm_OsekCB[0].appActiveFlag = 0;
            first_BC(0);           // 向发送队列中写入数据，并触发发送线程
            start_BC_timerByChl(0);// 增加发送开启应用报文 定时器.
        }

        if (nm_OsekCB[1].appActiveFlag == 1)
        {
            nm_OsekCB[1].appActiveFlag = 0;
            first_BC(1);           // 向发送队列中写入数据，并触发发送线程
           
            start_BC_timerByChl(1);// 增加发送开启应用报文 定时器.
        }
    }
#endif


    if (mask & TASK_SYS_EVENT_REQST1) //10MS 运行一次
    {
        mask &= (~TASK_SYS_EVENT_REQST1);// 清除功能码        
		update_NM_Local_WakeupCondition(WP_TYPE_ON_HD,DONOT_CARE);// 更新本地唤醒条件
		local_Condition_trigger_NM();
        COMM_VoltageCheck();// 检测通讯电压范围
        run_NM();
        APP_SendHandle();//应用报文发送处理，即将要发送的APP报文加入到发送队列中。
    }

    if (mask & TASK_SYS_EVENT_REQST2)   // 发送线程,此功能码在发送完成中置位
    {
        mask &= (~TASK_SYS_EVENT_REQST2);// 清除功能码

        if (APP_CANSendFifo[0].p_in != APP_CANSendFifo[0].p_out)
        {
            APP_CANSendFifoHandle(0);
        }

        if (APP_CANSendFifo[1].p_in != APP_CANSendFifo[1].p_out)
        {
            APP_CANSendFifoHandle(1);
        }
    }


	if (mask & TASK_SYS_EVENT_REQST4)	// 
		{
			mask &= (~TASK_SYS_EVENT_REQST4);// CANFD测试

		memset(&canfd_aaa,0,sizeof(CAN_SEND_MSG));

		canfd_aaa.MsgID =0x100;
		canfd_aaa.BRS =1;
		canfd_aaa.canFD = 1;
		canfd_aaa.DLC = 64;
		canfd_aaa.isEID = 0;
		canfd_aaa.isRTR = 0;

		memset(canfd_aaa.Data,'a',64);
		
		//if(can_send(0, MAIL_TRY, &canfd_aaa) == 0)
		//{
		//	taskPrintf(TASK_LEVEL_LOG, " CAN FD SEND TEST OK !!!");
		//}
		//else
		//{
		//	taskPrintf(TASK_LEVEL_LOG, " CAN FD SEND TEST Faile!!!");
	//	}
		}

    return mask;
}






