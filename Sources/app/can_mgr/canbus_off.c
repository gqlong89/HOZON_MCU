/*******************
    BUS OFF处理文件
 ******************/

#include <app/can_mgr/can_mgr.h>
#include <app/can_mgr/can_broadCast.h>
#include <app/can_mgr/canbus_off.h>
#include <can_api.h>
#include <timer.h>
#include <mid/kernel/task.h>
#include <mid/kernel/kernel.h>
#include <app/can_mgr/can_sendFifo.h>
#include <flexcan_hal.h>
#include <app/nm_osek/nm_osekCore.h>
#include <app/can_mgr/can_basic_cfg.h>



extern CAN_BASIC_CFG_ST can_basic_cfg;
extern unsigned char scom_taskid;


static int8_t timer_canport_recover[NATIVE_CAN_MAX] = {TIMER_INVALID, TIMER_INVALID, TIMER_INVALID};

static int8_t timer_busoff_counter_clear_timer[2] = {TIMER_INVALID,TIMER_INVALID};

CAN_RECOVER_STRATEGY str_canport_recover[NATIVE_CAN_MAX];

static volatile signed char can_busoff_flag[2] = { -1, -1};

#define SPI_TLS_NM_STATE            TASK_SYS_EVENT_REQST0
#define SPI_TLS_BUSOFF_STATE        TASK_SYS_EVENT_REQST1
#define SPI_TLS_NM_LIMPHOME_STATE   TASK_SYS_EVENT_REQST2  // ADD BY DSC



static void can_recover_port0_ISR(int8_t handle);
static void can_recover_port1_ISR(int8_t handle);

static void busoff_counter_clear_port0_ISR(int8_t handle);
static void busoff_counter_clear_port1_ISR(int8_t handle);


void can_recover_init(void)
{
	if(timer_canport_recover[0] == TIMER_INVALID)
	{
		timer_canport_recover[0] = ltimer_create(can_recover_port0_ISR);
	}

	if(timer_canport_recover[1] == TIMER_INVALID)
	{
		timer_canport_recover[1] = ltimer_create(can_recover_port1_ISR);
	}

	if(timer_busoff_counter_clear_timer[0] == TIMER_INVALID)
	{
		timer_busoff_counter_clear_timer[0] = ltimer_create(busoff_counter_clear_port0_ISR);
	}

	if(timer_busoff_counter_clear_timer[1] == TIMER_INVALID)
	{
		timer_busoff_counter_clear_timer[1] = ltimer_create(busoff_counter_clear_port1_ISR);
	}		
}

static uint16_t count_busoff_port[2] = {0, 0};

static void busoff_counter_clear_port0_ISR(int8_t handle)
{
	if (handle != timer_busoff_counter_clear_timer[0])
	{
		return;
	}

	count_busoff_port[0] = 0;
	ltimer_stop(timer_busoff_counter_clear_timer[0]);
}

static void busoff_counter_clear_port1_ISR(int8_t handle)
{
	if (handle != timer_busoff_counter_clear_timer[1])
	{
		return;
	}

	count_busoff_port[1] = 0;
	ltimer_stop(timer_busoff_counter_clear_timer[1]);
}


static void restart_timer_busoff_counter_clear(unsigned char canport)
{
	if(canport > 1)
	{
		return;
	}

	ltimer_stop(timer_busoff_counter_clear_timer[canport]);
	ltimer_start(timer_busoff_counter_clear_timer[canport], 5000);
}


static void can_recover_port0_ISR(int8_t handle)
{
    if (handle != timer_canport_recover[0])
    {
        return;
    }
	can_basic_cfg.busoff_cfg.busoff_recover_fun(CAN_PORT0);
    str_canport_recover[0].cL1ToL2++;
   if(count_busoff_port[0] < 6)
    {
        printk(PRINTK_LEVEL_ERROR, "can%d: fast, busoff = %d", 0, count_busoff_port[0]);
    }
    else
    {
		ltimer_stop(timer_canport_recover[0]);
		ltimer_start(timer_canport_recover[0], can_basic_cfg.busoff_cfg.slow_recover_cycle);
		printk(PRINTK_LEVEL_ERROR, "can%d: slow, busoff = %d", 0,  count_busoff_port[0]);
    }
}

static void can_recover_port1_ISR(int8_t handle)
{
    if (handle != timer_canport_recover[1])
    {
        return;
    }

    can_basic_cfg.busoff_cfg.busoff_recover_fun(CAN_PORT1);

    str_canport_recover[1].cL1ToL2++;
    if(count_busoff_port[1] < 6)
    {
        printk(PRINTK_LEVEL_ERROR, "can%d: fast busoff = %d", 1, count_busoff_port[1]);
    }
    else
    {
		ltimer_stop(timer_canport_recover[1]);
		ltimer_start(timer_canport_recover[1], can_basic_cfg.busoff_cfg.slow_recover_cycle);
		printk(PRINTK_LEVEL_ERROR, "can%d: slow busoff = %d", 1, count_busoff_port[1]);
    }
}

unsigned char check_busOffState(unsigned char canport)
{
    return (can_busoff_flag[canport]);
}

void can_busOffMgr(unsigned char canport, unsigned char state)
{
    if (state == STATE_BUSOFF)
    {
		restart_timer_busoff_counter_clear(canport);

		if (can_busoff_flag[canport] != 1)                           //首次发生BUSOFF
        {
            can_busoff_flag[canport] = 1;                             //置位BUSOFF标志
            stop_BC_timerByChl(canport);                              // 停止发送 广播

			if( count_busoff_port[canport] < 6 )
			{				
				/*BUSOFF计数器小于6时，执行快恢复*/
				ltimer_start(timer_canport_recover[canport], can_basic_cfg.busoff_cfg.fast_recover_cycle);
			}
			else
			{				
				/*BUSOFF计数器大于6时，执行慢恢复*/
				ltimer_start(timer_canport_recover[canport], can_basic_cfg.busoff_cfg.slow_recover_cycle);	// 开启恢复标志定时器
			}
			
	//		if(can_basic_cfg.busoff_cfg.report_to_MPU_EN)
	//		{
	//			if(can_basic_cfg.busoff_cfg.busoff_report_fun != NULL)
	//			{
	//				can_basic_cfg.busoff_cfg.busoff_report_fun(canport);
	//			}
	//		}			

#if USED_NM_OSEK	
            gotoLimpHome_ByBusOff(canport);
#endif
        }

        can_init(canport,can_default_baud.baud[canport]); /*重新初始化CAN控制器*/
		
        count_busoff_port[canport]++;
		
		if(count_busoff_port[canport] > 6)
		{
			count_busoff_port[canport] = 6;
		}
		
	//	if( str_canport_recover[0].cL1ToL2 == (can_basic_cfg.busoff_cfg.fast_recover_num+1))
	//	{
	//		kernelSetTaskEvent(scom_taskid, SPI_TLS_BUSOFF_STATE);//向MPU同步BUSSLEEP状态
	//	}
    }
    else if (state == STATE_SEND)
    {
        if (can_busoff_flag[canport] == 1)
        {
            can_busoff_flag[canport] = 0;
            ltimer_stop(timer_canport_recover[canport]);     // 停止BUSoff恢复定时器
            start_BC_timerByChl(canport);                    // 开启广播定时器
			str_canport_recover[canport].cL1ToL2 = 0;
            APP_ClearTxFifo(canport);//BUSOFF恢复了之后，要清空发送队列
            
       //     if(can_basic_cfg.busoff_cfg.report_to_MPU_EN)
        //    {
		//		if(can_basic_cfg.busoff_cfg.busoff_report_fun != NULL)
		//		{
		//			can_basic_cfg.busoff_cfg.busoff_report_fun(canport);
		//		}
		//	}		
        }
    }
    else
    {}
}


