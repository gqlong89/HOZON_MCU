

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

unsigned char canBasic_taskid = INVALID_TASK_NUMBER;//  ������ID

LOCAL_WAKEUPCOND_ST NM_Local_WakeupCondition;  


static int8_t timer_CAN_basic_task_trigger = TIMER_INVALID;  //��ʱ����can_basic����ʱ��

static int8_t timer_CANFD_test= TIMER_INVALID;  //CANFD���Զ�ʱ����


extern CAN_SENDFIFO_ST  APP_CANSendFifo[2];
extern CAN_BASIC_CFG_ST can_basic_cfg;


CAN_SEND_MSG  canfd_aaa;



#if USED_NM_OSEK
extern NMControlBlock_ST  nm_OsekCB[2];//��������nm���ƿ����
#endif




/*can_basic ������ʱ���жϷ�����*/
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



/*���±��ػ�������
wp_type:�����������͡�
        ��������������Ϊ   :WP_TYPE_ON_HDʱ��state��Ч��
        �������Զ���ȡ��ǰӲ״̬��

        ACC ��  CHARGE ͬ�ϡ�

        ��������������Ϊ��WP_TYPE_ON_VTʱ ,state��Ч��

 state:��������״̬��ֻ�� ������������Ϊ ��WP_TYPE_ON_VTʱ��Ч��

 		0�����
 		1����λ
 		2�������ģ��˲�����Ч

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


/*ʹ�ñ��ػ�����������������״̬ת��*/
void local_Condition_trigger_NM(void)
{
	unsigned char i;


#if USED_NM_OSEK

	if(NM_Local_WakeupCondition.last_state == 0)
	{
		if(NM_Local_WakeupCondition.current_state != 0)  // �����˻�������
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
		if(NM_Local_WakeupCondition.current_state == 0)   // ��������������
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
		if(NM_Local_WakeupCondition.current_state != 0)  // �����˻�������
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
		if(NM_Local_WakeupCondition.current_state == 0)	// ��������������
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

/*�����������*/
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
				nm_AutosarInit(i);// ����AUTOSAR �������			
				ret |=(0x01<<0); 
#endif
			}
			else if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_OSEK)
			{
#if USED_NM_OSEK 
				Start_NM_Osek(i);//����OSEK �������
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
			  first_BC(i);// �����㲥��
		      start_BC_timerByChl(i);
			}			 
		}
		else
		{}
	}


/*��û���õ���Ӧ��NM����NM�����ģ�����ʾ�رա�*/
	if(ret != 0)
	{
		if((ret & (0x01<<0)) ==0 )// û���õ� autosar_task
		{
			if(USED_NM_AUTOSAR == FUNC_USED)
			{
				taskPrintf(TASK_LEVEL_ERROR, "Unused AUTOSAR NM Function,Please set * USED_NM_AUTOSAR == FUNC_UNUSED * \r\n");
			}
		}

		if((ret & (0x01<<1)) ==0 )// û���õ� autosar_task
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

	nm_AutosarMainFunction();//  ����AUTOSAR ״̬ת��������

#endif

#if USED_NM_OSEK

	nm_OsekMainFunction();//   ����OSEK ״̬ת��������

#endif
}

unsigned int canBasic_task(unsigned int mask)
{
    if (mask & TASK_SYS_EVENT_INIT)
    {
        canBasic_taskid = kernelGetTaskID();

		memset(&NM_Local_WakeupCondition,0,sizeof(NM_Local_WakeupCondition));
		
        can_device_init();   // can �豸��ʼ�����˲���Ҫ�����Ż���
        
		can_basic_cfg_Init();// can basi ���ó�ʼ����
		
		update_NM_Local_WakeupCondition(WP_TYPE_ON_HD,DONOT_CARE);// ���±��ػ���������
		 
		start_NM();// �����������
		
		local_Condition_trigger_NM();
       
        COMM_VolInit();
		COMM_VoltageCheck();// ���ͨѶ��ѹ��Χ

		run_NM();

        /*������ʱ��������ʱ��*/
        if (TIMER_INVALID == timer_CAN_basic_task_trigger)
        {
            timer_CAN_basic_task_trigger = ltimer_create(CAN_basic_task_trigger_isr);//ֻ������û�п���
        }

		if (TIMER_INVALID == timer_CANFD_test)
        {
            timer_CANFD_test = ltimer_create(timer_CANFD_test_isr);//ֻ������û�п���
        }

		ltimer_start(timer_CANFD_test, 100);

        /*�����˶�ʱ����10MS����һ�α�����*/
        ltimer_start(timer_CAN_basic_task_trigger, 5);

        taskPrintf(TASK_LEVEL_LOG, "CAN basic task init OK!!!");
        kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);// ��������һֱ����
        return 0;
    }

/*ֻ����OSEK��ʱ�򣬲Ż�ʹ�á�*/
#if USED_NM_OSEK

    if (mask & TASK_SYS_EVENT_REQST0) //������֡ʱʹ��
    {

        mask &= (~TASK_SYS_EVENT_REQST0);// ���������

        if (nm_OsekCB[0].appActiveFlag == 1)
        {
            nm_OsekCB[0].appActiveFlag = 0;
            first_BC(0);           // ���Ͷ�����д�����ݣ������������߳�
            start_BC_timerByChl(0);// ���ӷ��Ϳ���Ӧ�ñ��� ��ʱ��.
        }

        if (nm_OsekCB[1].appActiveFlag == 1)
        {
            nm_OsekCB[1].appActiveFlag = 0;
            first_BC(1);           // ���Ͷ�����д�����ݣ������������߳�
           
            start_BC_timerByChl(1);// ���ӷ��Ϳ���Ӧ�ñ��� ��ʱ��.
        }
    }
#endif


    if (mask & TASK_SYS_EVENT_REQST1) //10MS ����һ��
    {
        mask &= (~TASK_SYS_EVENT_REQST1);// ���������        
		update_NM_Local_WakeupCondition(WP_TYPE_ON_HD,DONOT_CARE);// ���±��ػ�������
		local_Condition_trigger_NM();
        COMM_VoltageCheck();// ���ͨѶ��ѹ��Χ
        run_NM();
        APP_SendHandle();//Ӧ�ñ��ķ��ʹ�������Ҫ���͵�APP���ļ��뵽���Ͷ����С�
    }

    if (mask & TASK_SYS_EVENT_REQST2)   // �����߳�,�˹������ڷ����������λ
    {
        mask &= (~TASK_SYS_EVENT_REQST2);// ���������

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
			mask &= (~TASK_SYS_EVENT_REQST4);// CANFD����

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






