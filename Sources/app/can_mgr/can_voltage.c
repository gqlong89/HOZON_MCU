
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
extern unsigned char  accelerate_PDU_Flag[2];    // PDU快速发送标志

static COMM_VOL_ST COMM_Vol_SM;//定义一个通讯电压范围状态机

/*通讯电压功能初始化
    其实就是初始化其状态机的初始值
*/
void COMM_VolInit(void)
{
    COMM_Vol_SM.currrentState = COMM_VOL_NORMAL;
}

/*获取当前通讯电压状态*/
unsigned char get_Current_COMM_VolState(void)
{
    return COMM_Vol_SM.currrentState;
}



/*通讯电压检测函数*/
void COMM_VoltageCheck(void)
{
	static unsigned char comm_vol_wrongFlag = 0;//0:没有问题，1：有问题
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
					//要提示 设置 通讯电压异常 汇报函数
				}
			}
			
			
			for(i = 0;i<2;i++)
			{
				stop_BC_timerByChl(i);//停止应用报文
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
				//first_BC(i);		   //考虑电压恢复时，有无对首 帧报文时间要求 。
				start_BC_timerByChl(i);//恢复发送应用报文
#if USED_NM_AUTOSAR
				if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_AUTOSAR)
				{
					if (accelerate_PDU_Flag[i] == accelerate_YES)  // 开启NM 报文定时器
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






