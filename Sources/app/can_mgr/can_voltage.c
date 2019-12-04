
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
#include <app/can_mgr/can_basic_cfg.h>
#include <app/nm_autosar/nm_autosar.h>

extern CAN_BASIC_CFG_ST can_basic_cfg;
extern unsigned char  accelerate_PDU_Flag[2];    // PDU���ٷ��ͱ�־

static COMM_VOL_ST COMM_Vol_SM;//����һ��ͨѶ��ѹ��Χ״̬��

/*ͨѶ��ѹ���ܳ�ʼ��
    ��ʵ���ǳ�ʼ����״̬���ĳ�ʼֵ
*/
void COMM_VolInit(void)
{
    COMM_Vol_SM.currrentState = COMM_VOL_NORMAL;
}

/*��ȡ��ǰͨѶ��ѹ״̬*/
unsigned char get_Current_COMM_VolState(void)
{
    return COMM_Vol_SM.currrentState;
}



/*ͨѶ��ѹ��⺯��*/
void COMM_VoltageCheck(void)
{
	static unsigned char comm_vol_wrongFlag = 0;//0:û�����⣬1��������
	unsigned char i;

	if (get_powervoltage() < COMM_VOL_LOW_THRESHOLD)
    {
        COMM_Vol_SM.currrentState = COMM_VOL_LOW;

    }
    else if (get_powervoltage() > COMM_VOL_HIGH_THRESHODL)
    {
        COMM_Vol_SM.currrentState = COMM_VOL_HIGH;

    }
    else
    {
        COMM_Vol_SM.currrentState = COMM_VOL_NORMAL;

    }

	if (COMM_Vol_SM.currrentState != COMM_VOL_NORMAL)
	{
		if (comm_vol_wrongFlag == 0)
	    {
			comm_vol_wrongFlag = 1;

			if(can_basic_cfg.comm_vol_cfg.report_to_MPU_EN)
			{
				if(can_basic_cfg.comm_vol_cfg.comm_vol_report_fun != NULL)
				{
					can_basic_cfg.comm_vol_cfg.comm_vol_report_fun();
				}	
				else
				{
					//Ҫ��ʾ ���� ͨѶ��ѹ�쳣 �㱨����
				}
			}
			
			
			for(i = 0;i<2;i++)
			{
				stop_BC_timerByChl(i);//ֹͣӦ�ñ���
#if USED_NM_AUTOSAR
				if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_AUTOSAR)
				{
					stop_timer_SEND_PDU(i);
				}

#endif
			}
		}
	}
	
    if (COMM_Vol_SM.currrentState == COMM_VOL_NORMAL)
	{
		if (comm_vol_wrongFlag != 0)
		{
			comm_vol_wrongFlag = 0;

			if(can_basic_cfg.comm_vol_cfg.report_to_MPU_EN)
			{
				if(can_basic_cfg.comm_vol_cfg.comm_vol_report_fun != NULL)
				{
					can_basic_cfg.comm_vol_cfg.comm_vol_report_fun();
				}			
			}
			
			for(i = 0;i<2;i++)
			{
				//first_BC(i);		   //���ǵ�ѹ�ָ�ʱ�����޶��� ֡����ʱ��Ҫ�� ��
				start_BC_timerByChl(i);//�ָ�����Ӧ�ñ���
#if USED_NM_AUTOSAR
				if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_AUTOSAR)
				{
					if (accelerate_PDU_Flag[i] == accelerate_YES)  // ����NM ���Ķ�ʱ��
           			{
                		start_timer_SEND_PDU(i, T_NM_ImmediateCycleTime);
            		}
           			else
            		{
                		start_timer_SEND_PDU(i, T_NM_MessageCycle);
           			}
				}
#endif
			}
	
		}
	}
	
}






