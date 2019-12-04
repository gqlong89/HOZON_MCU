


#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <string.h>
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
#include <app/custom/custom.h>


CAN_BASIC_CFG_ST can_basic_cfg;


#if USED_NM_OSEK
//extern NMControlBlock_ST  nm_OsekCB[2];//定义两个nm控制块变量
#endif

/*busoff 恢复时 发送函数*/
static void cfg_can_busoff_recover_send(unsigned char canport)
{
	CAN_SEND_MSG  tmpMsg;
	static RTCTime current_time1;
	if(canport == CAN_PORT0)
	{

	}
	else if (canport == CAN_PORT1)
	{
		tmpMsg.MsgID = 0x530;//使用ODO里程数据为做恢复报文。
        tmpMsg.DLC = 8;

        tmpMsg.isEID = 0;
        tmpMsg.isRTR = 0;

        driverRTCGetTime(&current_time1);      // 实时封装数据

        tmpMsg.Data[0] = (unsigned char)(current_time1.RTC_Year);
        tmpMsg.Data[1] = current_time1.RTC_Mon;
        tmpMsg.Data[2] = current_time1.RTC_Mday;
        tmpMsg.Data[3] = current_time1.RTC_Hour;
        tmpMsg.Data[4] = current_time1.RTC_Min;
        tmpMsg.Data[5] = current_time1.RTC_Sec;
        tmpMsg.Data[6] = 0;
        tmpMsg.Data[7] = 0;

        can_send_busoff(canport, MAIL_TRY, &tmpMsg);
	}
	else
	{}
}



/*向MPU汇报BUSOFF状态所用的函数*/
static void cfg_can_busoff_report(unsigned char port)
{
	// 通知SCOM任务
}



/*向MPU汇报CAN通讯电压异常所用的函数*/
static void cfg_can_comm_vol_report(void)
{
	//通知SCOM任务
}


/**/
void can_basic_cfg_Init(void)
{
	unsigned char i;
	
	memset(&can_basic_cfg,0,sizeof(CAN_BASIC_CFG_ST));
	
	can_basic_cfg.nm_cfg[0].nm_type = NM_TYPE_NONE;
	can_basic_cfg.nm_cfg[1].nm_type = NM_TYPE_AUTOSAR;
	
	/*配置网络管理参数开始*/
#if USED_NM_AUTOSAR
	can_basic_cfg.nm_cfg[1].nm_autosar_cfg.nma_base_id  = 0x600;	// 合众网络管理参数
	can_basic_cfg.nm_cfg[1].nm_autosar_cfg.nma_locol_id = 0x01;
	can_basic_cfg.nm_cfg[1].nm_autosar_cfg.nma_max_id = 0x7f;
	can_basic_cfg.nm_cfg[1].nm_autosar_cfg.nma_mix_id = 0;
#endif

#if USED_NM_OSEK
	
	can_basic_cfg.nm_cfg[1].nm_osek_cfg.nmo_base_id	= 0x400;
	can_basic_cfg.nm_cfg[1].nm_osek_cfg.nmo_locol_id = 0x02;
	can_basic_cfg.nm_cfg[1].nm_osek_cfg.nmo_max_id = 0x7f;
	can_basic_cfg.nm_cfg[1].nm_osek_cfg.nmo_mix_id = 0;
	
#endif
	/*配置网络管理参数结束*/


	/*配置BUSOFF参数开始*/
	can_basic_cfg.busoff_cfg.fast_recover_cycle = 100;
	can_basic_cfg.busoff_cfg.slow_recover_cycle = 1000;
	can_basic_cfg.busoff_cfg.fast_recover_num = 5;
	can_basic_cfg.busoff_cfg.busoff_recover_fun = cfg_can_busoff_recover_send;
	can_basic_cfg.busoff_cfg.report_to_MPU_EN = 1;
	can_basic_cfg.busoff_cfg.busoff_report_fun = cfg_can_busoff_report;

	can_recover_init(); //  BUSoff 恢复定时器初始化
	
	/*配置BUSOFF参数结束*/


	/*配置CAN 通讯电压开始*/

	can_basic_cfg.comm_vol_cfg.high_thd  = 16;
	can_basic_cfg.comm_vol_cfg.low_thd = 9;
	can_basic_cfg.comm_vol_cfg.report_to_MPU_EN = 1;
	can_basic_cfg.comm_vol_cfg.comm_vol_report_fun = cfg_can_comm_vol_report;
	
	/*配置CAN 通讯电压结束*/


	/*配置广播参数开始*/
	 BC_Init();
	/*配置广播参数结束*/


	/*IPC私有协议初始化*/
	scom_Custom_Init();

#if 1/*检测网络管理参数是否已经配置*/
	for(i= 0;i<2;i++)
	{
		if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_AUTOSAR)
		{
			if((can_basic_cfg.nm_cfg[i].nm_autosar_cfg.nma_max_id ==0)&&(can_basic_cfg.nm_cfg[i].nm_autosar_cfg.nma_mix_id ==0))
			{
				 taskPrintf(TASK_LEVEL_ERROR, "CAN %d used Autosar NM,please config param\r\n",i);
			}
		}
		else if(can_basic_cfg.nm_cfg[i].nm_type == NM_TYPE_OSEK)
		{
			if((can_basic_cfg.nm_cfg[i].nm_osek_cfg.nmo_max_id ==0)&&(can_basic_cfg.nm_cfg[i].nm_osek_cfg.nmo_mix_id ==0))
			{
				 taskPrintf(TASK_LEVEL_ERROR, "CAN %d used Osek NM,please config param\r\n",i);
			}
		}
		else
		{}	
	}
#endif
	
}








