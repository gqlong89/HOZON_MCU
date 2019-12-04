/**********************************************
*文件名称：can_boradCast.c
*作用：    用于CAN广播发送
        人发送逻辑的角度分类，此文件支持四种广播类型
        1：事件型广播
        2：周期性广播
        3：事件性和周期性混合广播 即 混合性广播
        4：全能性广播
*文件编时间：2019年1月11日11:36:22
*作者      ：INTEST-DSC
*适用项目：吉利MPC项目，电咖项目等。
**********************************************/


#include <app/can_mgr/can_mgr.h>
#include <app/can_mgr/can_broadCast.h>
#include <app/scom/scom_tl.h>
#include <config.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <scom.h>
#include <scom_msg_def.h>
#include <string.h>
#include <sys/_stdint.h>
#include <timer.h>

#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <app/power/power.h>
#include <pm.h>
#include <stdio.h>
#include <string.h>
#include <rtc.h>
#include <app/can_mgr/canbus_off.h>
#include <app/nm_osek/nm_osekCore.h>
#include <app/can_mgr/can_sendFifo.h>
#include <app/can_mgr/can_basic_cfg.h>

BC_CYCLE_ST  BC_CycleList[MAX_BC_CYCLE_NUMBER];   // 定义一个周期性广播列表
BC_ENENT_ST  BC_EventList[MAX_BC_EVENT_NUMBER];   // 定义一个事件性广播列表
BC_CE_ST     BC_CEList[MAX_BC_CE_NUMBER];         // 定义一个混合性广播列表
BC_ACTIVE_ST BC_ActiveList[MAX_BC_ACTIVE_NUMBER]; // 定义一个使能性广播列表


static int8_t timer_BC_Cycle[MAX_BC_CYCLE_NUMBER] = {TIMER_INVALID};
static int8_t timer_BC_Event[MAX_BC_EVENT_NUMBER] = {TIMER_INVALID};
static int8_t timer_BC_CE[MAX_BC_CE_NUMBER] = {TIMER_INVALID};
static int8_t timer_BC_Active[MAX_BC_ACTIVE_NUMBER] = {TIMER_INVALID};

static int8_t timer_BC_CE_Event[MAX_BC_CE_NUMBER] = {TIMER_INVALID}; //混合性报文中的事件性报文的周期性定时器


unsigned char BC_CYCLE_SEND_FLAG;
unsigned char BC_EVENT_SEND_FLAG;
unsigned char BC_CE_SEND_FLAG;       //混合性报文中的周期性报文发送标志
unsigned char BC_ACITVE_SEND_FLAG;

unsigned char BC_CE_EVENT_SEND_FLAG;// 混合性报文中的事件性报文发送标志


extern unsigned char canBasic_taskid;


#if(USED_NM_OSEK == FUNC_USED)
//extern NMControlBlock_ST  nm_OsekCB[2];//定义两个nm控制块变量
#endif


/*合众维护*/
extern unsigned char buf_0x440state[8];
extern unsigned char buf_0x445state[8];
extern unsigned char buf_0x526state[8];
extern unsigned char buf_0x3d2state[8];



void clear_All_BCList(void)/*清空所有列表*/
{
    unsigned char i;

    for (i = 0; i < MAX_BC_CYCLE_NUMBER; i++)
    {
        BC_CycleList[i].used = 0;
    }

    for (i = 0; i < MAX_BC_CE_NUMBER; i++)
    {
        BC_CEList[i].used = 0;
    }

    for (i = 0; i < MAX_BC_EVENT_NUMBER; i++)
    {
        BC_EventList[i].used = 0;
    }

    for (i = 0; i < MAX_BC_ACTIVE_NUMBER; i++)
    {
        BC_ActiveList[i].used = 0;
    }
}


void timer_BC_CycleISR0(int8_t handle)
{
    if (handle != timer_BC_Cycle[0])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast Cycle timer0 check Wrong!");
        return;
    }

    BC_CYCLE_SEND_FLAG |= SEND_TIMER0;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//

    if (BC_CycleList[0].num_C != BC_FOREVER1)
    {
        BC_CycleList[0].num_C = (BC_CycleList[0].num_C - 1);

        if (BC_CycleList[0].num_C <= 0)

        {
            ltimer_stop(timer_BC_Cycle[0]);
        }
    }
}

void timer_BC_CycleISR1(int8_t handle)
{
    if (handle != timer_BC_Cycle[1])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast Cycle timer1 check Wrong!");
        return;
    }

    BC_CYCLE_SEND_FLAG |= SEND_TIMER1;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);

    if (BC_CycleList[1].num_C != BC_FOREVER1)
    {
        BC_CycleList[1].num_C = (BC_CycleList[1].num_C - 1);

        if (BC_CycleList[1].num_C <= 0)

        {
            ltimer_stop(timer_BC_Cycle[1]);
        }
    }


}


void timer_BC_CycleISR2(int8_t handle)
{
    if (handle != timer_BC_Cycle[2])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast Cycle timer2 check Wrong!");
        return;
    }

    BC_CYCLE_SEND_FLAG |= SEND_TIMER2;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);

    if (BC_CycleList[2].num_C != BC_FOREVER1)
    {
        BC_CycleList[2].num_C = (BC_CycleList[2].num_C - 1);

        if (BC_CycleList[2].num_C <= 0)

        {
            ltimer_stop(timer_BC_Cycle[2]);
        }
    }
}

void timer_BC_CycleISR3(int8_t handle)
{
    if (handle != timer_BC_Cycle[3])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast Cycle timer3 check Wrong!");
        return;
    }

    BC_CYCLE_SEND_FLAG |= SEND_TIMER3;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);

    if (BC_CycleList[3].num_C != BC_FOREVER1)
    {
        BC_CycleList[3].num_C = (BC_CycleList[0].num_C - 1);

        if (BC_CycleList[3].num_C <= 0)

        {
            ltimer_stop(timer_BC_Cycle[3]);
        }
    }
}

void timer_BC_CycleISR4(int8_t handle)
{
    if (handle != timer_BC_Cycle[4])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast Cycle timer4 check Wrong!");
        return;
    }

    BC_CYCLE_SEND_FLAG |= SEND_TIMER4;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);

    if (BC_CycleList[4].num_C != BC_FOREVER1)
    {
        BC_CycleList[4].num_C = (BC_CycleList[4].num_C - 1);

        if (BC_CycleList[4].num_C <= 0)

        {
            ltimer_stop(timer_BC_Cycle[4]);
        }
    }
}



/////////////////////////////////////////////////////////////////////////

void timer_BC_EventISR0(int8_t handle)
{
    if (handle != timer_BC_Event[0])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast Event timer0 check Wrong!");
        return;
    }

    BC_EVENT_SEND_FLAG |= SEND_TIMER0;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//

    BC_EventList[0].num_Event = (BC_EventList[0].num_Event - 1);

    if (BC_EventList[0].num_Event <= 0)
    {
        ltimer_stop(timer_BC_Event[0]);
    }
}

void timer_BC_EventISR1(int8_t handle)
{
    if (handle != timer_BC_Event[1])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast Event timer1 check Wrong!");
        return;
    }

    BC_EVENT_SEND_FLAG |= SEND_TIMER1;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//

    BC_EventList[1].num_Event = (BC_EventList[1].num_Event - 1);

    if (BC_EventList[1].num_Event <= 0)
    {
        ltimer_stop(timer_BC_Event[1]);
    }
}

void timer_BC_EventISR2(int8_t handle)
{
    if (handle != timer_BC_Event[2])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast Event timer2 check Wrong!");
        return;
    }

    BC_EVENT_SEND_FLAG |= SEND_TIMER2;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//

    BC_EventList[2].num_Event = (BC_EventList[2].num_Event - 1);

    if (BC_EventList[2].num_Event <= 0)
    {
        ltimer_stop(timer_BC_Event[2]);
    }
}

void timer_BC_EventISR3(int8_t handle)
{
    if (handle != timer_BC_Event[3])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast Event timer3 check Wrong!");
        return;
    }

    BC_EVENT_SEND_FLAG |= SEND_TIMER3;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//

    BC_EventList[3].num_Event = (BC_EventList[3].num_Event - 1);

    if (BC_EventList[3].num_Event <= 0)
    {
        ltimer_stop(timer_BC_Event[3]);
    }
}

void timer_BC_EventISR4(int8_t handle)
{
    if (handle != timer_BC_Event[4])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast Event timer4 check Wrong!");
        return;
    }

    BC_EVENT_SEND_FLAG |= SEND_TIMER4;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//

    BC_EventList[4].num_Event = (BC_EventList[4].num_Event - 1);

    if (BC_EventList[4].num_Event <= 0)
    {
        ltimer_stop(timer_BC_Event[4]);
    }
}



void timer_BC_CEISR0(int8_t handle)
{
    if (handle != timer_BC_CE[0])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast CE timer0 check Wrong!");
        return;
    }

    if (BC_CEList[0].eventIsGoing == 0) //没有事件性报文在处理
    {
        BC_CE_SEND_FLAG |= SEND_TIMER0;
        kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);
    }

}

void timer_BC_CEISR1(int8_t handle)
{
    if (handle != timer_BC_CE[1])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast CE timer1 check Wrong!");
        return;
    }

    if (BC_CEList[1].eventIsGoing == 0) //没有事件性报文在处理
    {
        BC_CE_SEND_FLAG |= SEND_TIMER1;

        kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);
    }
}

void timer_BC_CEISR2(int8_t handle)
{
    if (handle != timer_BC_CE[2])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast CE timer2 check Wrong!");
        return;
    }

    if (BC_CEList[2].eventIsGoing == 0) //没有事件性报文在处理
    {
        BC_CE_SEND_FLAG |= SEND_TIMER2;
        kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);
    }
}


void timer_BC_CEISR3(int8_t handle)
{
    if (handle != timer_BC_CE[3])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast CE timer3 check Wrong!");
        return;
    }

    if (BC_CEList[3].eventIsGoing == 0) //没有事件性报文在处理
    {
        BC_CE_SEND_FLAG |= SEND_TIMER3;
        kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);
    }
}

void timer_BC_CEISR4(int8_t handle)
{
    if (handle != timer_BC_CE[4])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast CE timer4 check Wrong!");
        return;
    }

    if (BC_CEList[4].eventIsGoing == 0) //没有事件性报文在处理
    {
        BC_CE_SEND_FLAG |= SEND_TIMER4;
        kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);
    }
}

void timer_BC_CE_EVENTISR0(int8_t handle)
{

    if (handle != timer_BC_CE_Event[0])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast CE_Event timer0 check Wrong!");
        return;
    }

    BC_CE_EVENT_SEND_FLAG |= SEND_TIMER0;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);

    BC_CEList[0].num_Event = (BC_CEList[0].num_Event - 1);

    if (BC_CEList[0].num_Event <= 0)
    {
        ltimer_stop(timer_BC_CE_Event[0]);
    }

}

void timer_BC_CE_EVENTISR1(int8_t handle)
{

    if (handle != timer_BC_CE_Event[1])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast CE_Event timer1 check Wrong!");
        return;
    }

    BC_CE_EVENT_SEND_FLAG |= SEND_TIMER1;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);

    BC_CEList[1].num_Event = (BC_CEList[1].num_Event - 1);

    if (BC_CEList[1].num_Event <= 0)
    {
        ltimer_stop(timer_BC_CE_Event[1]);
    }
}

void timer_BC_CE_EVENTISR2(int8_t handle)
{

    if (handle != timer_BC_CE_Event[2])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast CE_Event timer2 check Wrong!");
        return;
    }

    BC_CE_EVENT_SEND_FLAG |= SEND_TIMER2;

    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);

    BC_CEList[2].num_Event = (BC_CEList[2].num_Event - 1);

    if (BC_CEList[2].num_Event <= 0)
    {
        ltimer_stop(timer_BC_CE_Event[2]);
    }
}

void timer_BC_CE_EVENTISR3(int8_t handle)
{

    if (handle != timer_BC_CE_Event[3])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast CE_Event timer3 check Wrong!");
        return;
    }

    BC_CE_EVENT_SEND_FLAG |= SEND_TIMER3;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);

    BC_CEList[3].num_Event = (BC_CEList[3].num_Event - 1);

    if (BC_CEList[3].num_Event <= 0)
    {
        ltimer_stop(timer_BC_CE_Event[3]);
    }
}

void timer_BC_CE_EVENTISR4(int8_t handle)
{

    if (handle != timer_BC_CE_Event[4])
    {
        printk(PRINTK_LEVEL_ERROR, "BroadCast CE_Event timer4 check Wrong!");
        return;
    }

    BC_CE_EVENT_SEND_FLAG |= SEND_TIMER4;
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);

    BC_CEList[4].num_Event = (BC_CEList[4].num_Event - 1);

    if (BC_CEList[4].num_Event <= 0)
    {
        ltimer_stop(timer_BC_CE_Event[4]);
    }
}


timeout_handler timer_BC_CycleISR[MAX_BC_CYCLE_NUMBER] =
{
    timer_BC_CycleISR0,
    timer_BC_CycleISR1,
    timer_BC_CycleISR2,
    timer_BC_CycleISR3,
    timer_BC_CycleISR4,
};

timeout_handler timer_BC_EventISR[MAX_BC_EVENT_NUMBER] =
{
    timer_BC_EventISR0,
    timer_BC_EventISR1,
    timer_BC_EventISR2,
    timer_BC_EventISR3,
    timer_BC_EventISR4,
};


timeout_handler timer_BC_CEISR[MAX_BC_CE_NUMBER] =
{
    timer_BC_CEISR0,
    timer_BC_CEISR1,
    timer_BC_CEISR2,
    timer_BC_CEISR3,
    timer_BC_CEISR4,
};

timeout_handler timer_BC_CE_EVENTISR[MAX_BC_CE_NUMBER] =
{
    timer_BC_CE_EVENTISR0,
    timer_BC_CE_EVENTISR1,
    timer_BC_CE_EVENTISR2,
    timer_BC_CE_EVENTISR3,
    timer_BC_CE_EVENTISR4,
};



/*创建广播条目函数************************
  即将信号矩阵中的内容能过  BC_SET_ST结构体
  写入各类型广播列表

  BC_VALUE：信号矩阵中的值
  返回值 ：0：广播条目注册成功
           ！0 注册失败
*****************************************/
unsigned char creat_BC_Item(BC_SET_ST BC_VALUE)
{
    unsigned char ret = 0;// 结果
    unsigned char i = 0;

    if (BC_VALUE.type == BC_TYPE_INVALID)
    {
        printk(PRINTK_LEVEL_ERROR, "BoartCast type need to be set!");
        ret = 1;
        return ret;
    }


    switch (BC_VALUE.type)
    {
        case BC_TYPE_CYCLE:
            {
                for (i = 0; i < MAX_BC_CYCLE_NUMBER; i++)
                {
                    if (BC_CycleList[i].used == BC_USED)
                    {
                        continue;//跳出本次循环
                    }

                    break; // 跳出FOR循环
                }

                if (i == MAX_BC_CYCLE_NUMBER)
                {
                    ret = 2;
                    printk(PRINTK_LEVEL_ERROR, "Cycle broadCast number is Over mast number");
                    return ret;
                }

                BC_CycleList[i].id = BC_VALUE.id;
                BC_CycleList[i].port = BC_VALUE.port;
                BC_CycleList[i].num_C = BC_VALUE.num_C;
                BC_CycleList[i].cycle_C = BC_VALUE.cycle_C;
                BC_CycleList[i].data_C_type = BC_VALUE.data_C_type;

                memcpy(BC_CycleList[i].data_C, BC_VALUE.data_C, 8);
                memcpy(BC_CycleList[i].mask_C, BC_VALUE.mask_C, 8);
                BC_CycleList[i].used = BC_USED;
            //    printk(PRINTK_LEVEL_ERROR, "Cycle broadcast %d register OK!", i);
                ret = 0;
                break;
            }

        case BC_TYPE_EVENT:
            {
                for (i = 0; i < MAX_BC_EVENT_NUMBER; i++)
                {
                    if (BC_EventList[i].used == BC_USED)
                    {
                        continue;//跳出本次循环
                    }

                    break; // 跳出FOR循环
                }

                if (i == MAX_BC_EVENT_NUMBER)
                {
                    ret = 3;
                    printk(PRINTK_LEVEL_ERROR, "Event broadCast number is Over mast number");
                    return ret;
                }

                BC_EventList[i].id = BC_VALUE.id;
                BC_EventList[i].port = BC_VALUE.port;
                BC_EventList[i].used = BC_USED;
            //    printk(PRINTK_LEVEL_ERROR, "Event broadcast %d register OK!", i);
                ret = 0;
                break;
            }

        case BC_TYPE_CE:
            {
                for (i = 0; i < MAX_BC_CE_NUMBER; i++)
                {
                    if (BC_CEList[i].used == BC_USED)
                    {
                        continue;//跳出本次循环
                    }

                    break; // 跳出FOR循环
                }

                if (i == MAX_BC_CE_NUMBER)
                {
                    ret = 4;
                    printk(PRINTK_LEVEL_ERROR, "CE broadCast number is Over mast number");
                    return ret;
                }

                BC_CEList[i].id = BC_VALUE.id;
                BC_CEList[i].port = BC_VALUE.port;
                BC_CEList[i].num_C = BC_VALUE.num_C;
                BC_CEList[i].cycle_C = BC_VALUE.cycle_C;
                BC_CEList[i].data_C_type = BC_VALUE.data_C_type;
                memcpy(BC_CEList[i].data_C, BC_VALUE.data_C, 8);
                memcpy(BC_CEList[i].mask_C, BC_VALUE.mask_C, 8);
                BC_CEList[i].used = BC_USED;
                ret = 0;

                break;
            }

        case BC_TYPE_ACTIVE:
            {
                for (i = 0; i < MAX_BC_ACTIVE_NUMBER; i++)
                {
                    if (BC_ActiveList[i].used == BC_USED)
                    {
                        continue;//跳出本次循环
                    }

                    break; // 跳出FOR循环
                }

                if (i == MAX_BC_ACTIVE_NUMBER)
                {
                    ret = 5;
                    printk(PRINTK_LEVEL_ERROR, "Active broadCast number is Over mast number");
                    return ret;
                }

                BC_ActiveList[i].id = BC_VALUE.id;
                BC_ActiveList[i].port = BC_VALUE.port;
                BC_ActiveList[i].used = BC_USED;
                ret = 0;
                break;
            }

        default:
            {
                printk(PRINTK_LEVEL_ERROR, "BC type worng!");
                ret = 0xfe;
                break;
            }
    }

    return ret;
}

/*手动设置广播参数*/
void set_BCParam_Manual(void)
{
    BC_SET_ST tmpBC;
    clear_All_BCList();
	

	tmpBC.id = 0x440;
	memset(tmpBC.data_C, 0, 8); 
	tmpBC.data_C_type = HZ_0X440;	 //????
	memset(tmpBC.mask_C, 0, 8);
	tmpBC.num_C = BC_FOREVER1;
	tmpBC.cycle_C = 100;
	tmpBC.port = 1;
	tmpBC.type = BC_TYPE_CYCLE;
	creat_BC_Item(tmpBC);

	tmpBC.id = 0x445;
	memset(tmpBC.data_C, 0, 8); 
	tmpBC.data_C_type = HZ_0X445;	 //????
	memset(tmpBC.mask_C, 0, 8);
	tmpBC.num_C = BC_FOREVER1;
	tmpBC.cycle_C = 100;
	tmpBC.port = 1;
	tmpBC.type = BC_TYPE_CYCLE;
	creat_BC_Item(tmpBC);

	tmpBC.id = 0x526;
	memset(tmpBC.data_C, 0, 8); 
	tmpBC.data_C_type = HZ_0X526;	 //????
	memset(tmpBC.mask_C, 0, 8);
	tmpBC.num_C = BC_FOREVER1;
	tmpBC.cycle_C = 500;
	tmpBC.port = 1;
	tmpBC.type = BC_TYPE_CYCLE;
	creat_BC_Item(tmpBC);	

    tmpBC.id = 0x530;
    memset(tmpBC.data_C, 0, 8);
    tmpBC.data_C_type = HZ_TIME;   //????
    memset(tmpBC.mask_C, 0, 8);
    tmpBC.num_C = BC_FOREVER1;
    tmpBC.cycle_C = 1000;
    tmpBC.port = 1;
    tmpBC.type = BC_TYPE_CYCLE;
    creat_BC_Item(tmpBC);


	tmpBC.id = 0x3d2;                // 认证报文 ，是事件性报文
	tmpBC.port = 1;
	tmpBC.type = BC_TYPE_EVENT;
	creat_BC_Item(tmpBC);	

}

/*****************************************
广播初始化，主要根据各广播列表中已经注册的
广播条目，创建定时器。
返回值 ：0：成功
        ！0，失败
*****************************************/
unsigned char BC_Init(void)
{
    unsigned char ret = 0;
    unsigned char i = 0;

    set_BCParam_Manual();


    memset(timer_BC_Cycle, TIMER_INVALID, MAX_BC_CYCLE_NUMBER);
    memset(timer_BC_Event, TIMER_INVALID, MAX_BC_EVENT_NUMBER);
    memset(timer_BC_CE, TIMER_INVALID, MAX_BC_CE_NUMBER);
    memset(timer_BC_Active, TIMER_INVALID, MAX_BC_ACTIVE_NUMBER);

    memset(timer_BC_CE_Event, TIMER_INVALID, MAX_BC_CE_NUMBER);
    //创建广播条目

    for (i = 0; i < MAX_BC_CYCLE_NUMBER; i++)
    {
        if (BC_CycleList[i].used == BC_USED)
        {
            if (timer_BC_Cycle[i] == TIMER_INVALID) // 此定时器还没有创建
            {
                timer_BC_Cycle[i] = ltimer_create(timer_BC_CycleISR[i]);//只创建，没有开启
                
            }
            else
            {
                printk(PRINTK_LEVEL_ERROR, "Timer for broadCast Cycle %d is used,Wrong!", i);
            }
        }
        else
        {
            break;
        }
    }
//	printk(PRINTK_LEVEL_ERROR, "SYS has %d cycle broadCast to send!!!", i);

    for (i = 0; i < MAX_BC_EVENT_NUMBER; i++)
    {
        if (BC_EventList[i].used == BC_USED)
        {
            if (timer_BC_Event[i] == TIMER_INVALID) // 此定时器还没有创建
            {
                timer_BC_Event[i] = ltimer_create(timer_BC_EventISR[i]);//只创建，没有开启
            }
            else
            {
                printk(PRINTK_LEVEL_ERROR, "Timer for broadCast Event %d is used,Wrong!", i);
            }
        }
        else
        {
            break;
        }

    }
//	printk(PRINTK_LEVEL_ERROR, "SYS has %d Event broadCast to send!!!", i);


    for (i = 0; i < MAX_BC_CE_NUMBER; i++)
    {
        if (BC_CEList[i].used == BC_USED)
        {
            if (timer_BC_CE[i] == TIMER_INVALID) // 此定时器还没有创建,混合性报文中的周期性定时器
            {
                timer_BC_CE[i] = ltimer_create(timer_BC_CEISR[i]);//只创建，没有开启
            }
            else
            {
                printk(PRINTK_LEVEL_ERROR, "Timer for broadCast CE %d is used,Wrong!", i);
            }

            if (timer_BC_CE_Event[i] == TIMER_INVALID) // 混合性报文中的事件性定时器。
            {
                timer_BC_CE_Event[i] = ltimer_create(timer_BC_CE_EVENTISR[i]);//只创建，没有开启
            }
            else
            {
                printk(PRINTK_LEVEL_ERROR, "Timer for broadCast CE_EVENT %d is used,Wrong!", i);
            }
        }
        else
        {         
            break;
        }

    }
//	printk(PRINTK_LEVEL_ERROR, "SYS has %d CE broadCast to send!!!", i);
    return ret;
}

/*纯周期性报文发送函数*******************
  周期性报文 具体发送函数
  带有发送次数的维护和检测

seq:指定要周期性报文列表中的哪一个数据
返回值：0：发送成功
        ！：非 0，发送失败
****************************************/


/*周期性报文处理函数，供AAP在任务里调用*/


/**************************
手动开启广播发送定时器
id:具体值 ，为相应ID广播
   0xfffffffe,所有ID

    ID对比只在周期性报文列表和混合性报文列表中进行。


 返回值 ：

        0：成功
        1：id没有注册
**************************/
unsigned char start_BC_timerByID(unsigned char canport, unsigned int id)
{
    unsigned char i;

    if (id == 0xfffffffe)
    {
        //开启所有通道的所有周期性报文
        //暂时用不到，先不写。要是写也好写
    }
    else
    {
        for (i = 0; i < MAX_BC_CYCLE_NUMBER; i++)
        {
            if ((BC_CycleList[i].id == id) && (BC_CycleList[i].port == canport))
            {
                if (timer_BC_Cycle[i] != TIMER_INVALID)
                {
                    ltimer_start(timer_BC_Cycle[i], BC_CycleList[i].cycle_C);
                }
                else
                {
                    break;//跳出循环
                }
            }

        }

        for (i = 0; i < MAX_BC_CE_NUMBER; i++)
        {
            if ((BC_CEList[i].id == id) && (BC_CEList[i].port == canport))
            {
                if (timer_BC_CE[i] != TIMER_INVALID)
                {
                    ltimer_start(timer_BC_CE[i], BC_CEList[i].cycle_C);
                }
                else
                {
                    break;
                }
            }
        }
    }

    return 0;
}




/**************************
手动开启广播发送定时器
canport:为CAN通通道
PS:是指开启周期性报文和混合性报文中的周期性报文定时器

**************************/
void start_BC_timerByChl(unsigned char canport)
{
    unsigned char i;

    if (canport > 1)
    {
        printk(PRINTK_LEVEL_ERROR, "CAN port %d is wrong", canport);
        return;
    }

    for (i = 0; i < MAX_BC_CYCLE_NUMBER; i++)
    {
        if (BC_CycleList[i].port == canport)
        {
            if (timer_BC_Cycle[i] != TIMER_INVALID)
            {
                ltimer_start(timer_BC_Cycle[i], BC_CycleList[i].cycle_C);
            }
            else
            {
                break;
            }
        }
    }

    for (i = 0; i < MAX_BC_CE_NUMBER; i++)
    {
        if (BC_CEList[i].port == canport)
        {
            if (timer_BC_CE[i] != TIMER_INVALID)
            {

                ltimer_start(timer_BC_CE[i], BC_CEList[i].cycle_C);
            }
            else
            {
                break;
            }
        }
    }
}

/**************************
手动停止广播发送定时器
canport:CAN端口
id:具体值 ，为相应ID广播
   0xfffffffe,所有ID

 返回值 ：

        0：成功
        1：id没有注册
PS：一定要判断通道，防止两个通道中有相同的ID。
**************************/
unsigned char stop_BC_timerByID(unsigned char canport, unsigned int id)
{
    unsigned char i;

    if (id == 0xfffffffe)
    {
        //关闭所有通道的所有周期性报文
        //暂时用不到，先不写。要是写也好写
    }
    else
    {
        for (i = 0; i < MAX_BC_CYCLE_NUMBER; i++)
        {
            if ((BC_CycleList[i].id == id) && (BC_CycleList[i].port == canport))
            {
                if (timer_BC_Cycle[i] != TIMER_INVALID)
                {
                    ltimer_stop(timer_BC_Cycle[i]);
                }
                else
                {
                    break;//跳出循环
                }
            }

        }

        for (i = 0; i < MAX_BC_CE_NUMBER; i++)
        {
            if ((BC_CEList[i].id == id) && (BC_CEList[i].port == canport))
            {
                if (timer_BC_CE[i] != TIMER_INVALID)
                {
                    ltimer_stop(timer_BC_CE[i]);
                }
                else
                {
                    break;
                }
            }
        }
    }

    return 1;
}

/**************************
手动停止广播发送定时器
canport:为CAN通通道
PS:停止定时器指停止相应CAN口上的
   所有APP定时器，包括周期性定时器和事件性定时器

**************************/
void stop_BC_timerByChl(unsigned char canport)
{
    unsigned char i;

    if (canport > 1)
    {
        printk(PRINTK_LEVEL_ERROR, "CAN port %d is wrong", canport);
        return;
    }

	printk(PRINTK_LEVEL_ERROR, "stop can %d bc", canport);

    for (i = 0; i < MAX_BC_CYCLE_NUMBER; i++)
    {
        if (BC_CycleList[i].port == canport)
        {
            if (timer_BC_Cycle[i] != TIMER_INVALID)
            {
                ltimer_stop(timer_BC_Cycle[i]);
            }
            else
            {
                break;
            }
        }
    }

    for (i = 0; i < MAX_BC_CE_NUMBER; i++)
    {
        if (BC_CEList[i].port == canport)
        {
            if (timer_BC_CE[i] != TIMER_INVALID)
            {
                ltimer_stop(timer_BC_CE[i]);
            }
            else
            {
                break;
            }

            if (timer_BC_CE_Event[i] != TIMER_INVALID)
            {
                ltimer_stop(timer_BC_CE_Event[i]);
                BC_CEList[i].num_Event = 0;           //清空事件性报文剩余发送次数。
            }
            else
            {
                break;
            }
        }
    }

    for (i = 0; i < MAX_BC_EVENT_NUMBER; i++)
    {
        if (BC_EventList[i].port == canport)
        {
            if (timer_BC_Event[i] != TIMER_INVALID)
            {
                ltimer_stop(timer_BC_Event[i]);
                BC_EventList[i].num_Event = 0;      //清空事件性报文剩余发送次数
            }
            else
            {
                break;
            }
        }
    }
}


/*开启事件*/
void start_Event_timerByID(unsigned char canport, unsigned int id)
{
    unsigned char i;

    for (i = 0; i < MAX_BC_EVENT_NUMBER; i++)
    {
        if ((BC_EventList[i].id == id) && (BC_EventList[i].port == canport))
        {
            if (timer_BC_Event[i] != TIMER_INVALID)
            {
                ltimer_start(timer_BC_Event[i], BC_EventList[i].cycle_Event);
            }
            else
            {
                break;//跳出循环
            }
        }

    }

    for (i = 0; i < MAX_BC_CE_NUMBER; i++)
    {
        if ((BC_CEList[i].id == id) && (BC_CEList[i].port == canport))
        {
            if (timer_BC_CE[i] != TIMER_INVALID)
            {
                ltimer_start(timer_BC_CE_Event[i], BC_CEList[i].cycle_Event);
            }
            else
            {
                break;
            }
        }
    }
}


/*关闭事件*/
void stop_Event_timerByID(unsigned char canport, unsigned int id)
{
    unsigned char i;

    for (i = 0; i < MAX_BC_EVENT_NUMBER; i++)
    {
        if ((BC_EventList[i].id == id) && (BC_EventList[i].port == canport))
        {
            if (timer_BC_Event[i] != TIMER_INVALID)
            {
                ltimer_stop(timer_BC_Event[i]);
            }
            else
            {
                break;//跳出循环
            }
        }

    }

    for (i = 0; i < MAX_BC_CE_NUMBER; i++)
    {
        if ((BC_CEList[i].id == id) && (BC_CEList[i].port == canport))
        {
            if (timer_BC_CE[i] != TIMER_INVALID)
            {
                ltimer_stop(timer_BC_CE_Event[i]);
            }
            else
            {
                break;
            }
        }
    }
}

/***************************************************
  数据封装功能函数

  p_des:封装后的数据存放指针
  p_src:被封装数据的来源指针
  p_mask:数据的 位 填充码 指针
  data_type:被封装数据的类型。

  返回值：ture:数据封装成功

false:数据封装失败，没有相应的数据类型。

****************************************************/
bool data_packageFunction(unsigned char *p_des, unsigned char *p_src, unsigned char *p_mask,
                          unsigned char data_type)
{
    static RTCTime current_time1;
    unsigned char i = 0;
    bool ret = true;

    switch (data_type)
    {
        case    DATA_TYPE_CMX_FIX:
            {
                memcpy(p_des, p_src, 8);
                break;
            }

        case   HZ_TIME:   // 合众时间  
            {
                driverRTCGetTime(&current_time1);      // 实时封装数据    
                
                p_des[0] = (unsigned char)(current_time1.RTC_Year);
                p_des[1] = current_time1.RTC_Mon;
                p_des[2] = current_time1.RTC_Mday;
                p_des[3] = current_time1.RTC_Hour;
                p_des[4] = current_time1.RTC_Min;
                p_des[5] = current_time1.RTC_Sec;
				p_des[6] = 0;
				p_des[7] = 0;
				break;
            }

		case   HZ_0X440:    //合众0x440报文
			{
				memcpy(p_des, buf_0x440state, 8);

				break;		
			}
		case   HZ_0X445:   // 合众0x445报文 
			{
				memcpy(p_des, buf_0x445state, 8);

				break;
			}
		case   HZ_0X526:      // 合众0x526报文
			{
				memcpy(p_des, buf_0x526state, 8);

				break;
			}
        default:
            {
                ret = false;
                break;
            }
    }

    if (ret)
    {
        for (i = 0; i < 8; i++)
        {
            *(p_des + i) |= *(p_mask + i);
        }
    }

    return ret;
}


/*****************************************
手动发送发有广播的首帧
 ****************************************/
void first_BC(unsigned char canport)
{
    unsigned char i ;
    CAN_SEND_MSG tmpMsg;

    for (i = 0; i < MAX_BC_CYCLE_NUMBER; i++)
    {
        if ((BC_CycleList[i].used) && (BC_CycleList[i].port == canport))
        {
            tmpMsg.MsgID = BC_CycleList[i].id;
            tmpMsg.isEID = (BC_CycleList[i].id > 0x7ff) ? 1 : 0;
            tmpMsg.isRTR = 0;
            tmpMsg.DLC = 8;
			memset(tmpMsg.Data,0,8);

            if (data_packageFunction(tmpMsg.Data, BC_CycleList[i].data_C, BC_CycleList[i].mask_C,
                                     BC_CycleList[i].data_C_type))
            {

            }
            else
            {
                printk(PRINTK_LEVEL_ERROR, "Data package worng!!!");
                memcpy(tmpMsg.Data, 0, 8);
            }

            APP_CANSend(BC_CycleList[i].port, &tmpMsg);

            if (BC_CycleList[i].num_C != BC_FOREVER1)
            {
                BC_CycleList[i].num_C = (BC_CycleList[i].num_C - 1);

                if (BC_CycleList[i].num_C <= 0)
                {
                    ltimer_stop(timer_BC_Cycle[i]);
                }
            }
        }
        else
        {
            
        }
    }

    for (i = 0; i < MAX_BC_CE_NUMBER; i++)
    {
        if ((BC_CEList[i].used) && (BC_CEList[i].port == canport))
        {
            tmpMsg.MsgID = BC_CEList[i].id;
            tmpMsg.isEID = (BC_CEList[i].id > 0x7ff) ? 1 : 0;
            tmpMsg.isRTR = 0;
            tmpMsg.DLC = 8;
			memset(tmpMsg.Data,0,8);
            if (data_packageFunction(tmpMsg.Data, BC_CEList[i].data_C, BC_CEList[i].mask_C,
                                     BC_CEList[i].data_C_type))
            {

            }
            else
            {
                printk(PRINTK_LEVEL_ERROR, "Data package worng!!!");
                memcpy(tmpMsg.Data, 0, 8);
            }

            APP_CANSend(BC_CEList[i].port, &tmpMsg);
        }
        else
        {
         //   break;
        }
    }

	APP_CANSendFifoHandle(canport);

	// kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST2);
}



/********************************************
    应用报文发送处理函数

    检测是否有应用报文需要发送，主要检测各
    应用报文发送标志。

    将需要发送的应用报文进行数据封装，之后
    加入到 发送队列之中。

*********************************************/
void APP_SendHandle(void)//检测有没有数据要进行封装
{
    unsigned char i;
    CAN_SEND_MSG tmpMsg;


    for (i = 0; i < MAX_BC_CYCLE_NUMBER; i++)
    {
        if (BC_CYCLE_SEND_FLAG & (1 << i))
        {
            INT_SYS_DisableIRQGlobal();

            BC_CYCLE_SEND_FLAG &= (~(1 << i)); //清除标志位

            INT_SYS_EnableIRQGlobal();

            tmpMsg.MsgID = BC_CycleList[i].id;
            tmpMsg.isEID = (BC_CycleList[i].id > 0x7ff) ? 1 : 0;
            tmpMsg.isRTR = 0;
            tmpMsg.DLC = 8;
            memset(tmpMsg.Data,0,8);
            if (data_packageFunction(tmpMsg.Data, BC_CycleList[i].data_C, BC_CycleList[i].mask_C,
                                     BC_CycleList[i].data_C_type))
            {

            }
            else
            {
                printk(PRINTK_LEVEL_ERROR, "Data package worng!!!");
                memset(tmpMsg.Data, 0, 8);
            }

            APP_CANSend(BC_CycleList[i].port, &tmpMsg);
        }
    }

    for (i = 0; i < MAX_BC_EVENT_NUMBER; i++)
    {
        if (BC_EVENT_SEND_FLAG & (1 << i))
        {
            INT_SYS_DisableIRQGlobal();

            BC_EVENT_SEND_FLAG &= (~(1 << i)); //清除标志位

            INT_SYS_EnableIRQGlobal();

            tmpMsg.MsgID = BC_EventList[i].id;
            tmpMsg.isEID = (BC_EventList[i].id > 0x7ff) ? 1 : 0;
            tmpMsg.isRTR = 0;
            tmpMsg.DLC = 8;

            memset(tmpMsg.Data, 0, 8);
            memcpy(tmpMsg.Data, BC_EventList[i].data_Event, BC_EventList[i].len_Event);

            APP_CANSend(BC_CycleList[i].port, &tmpMsg);

            /*一定要在事件性报文全都加入发送队列后才能确认事件结束*/
            if (BC_EventList[i].num_Event <= 0)
            {
                BC_EventList[i].eventIsGoing = 0;    //事件性报文已经结束
            }
        }
    }



    for (i = 0; i < MAX_BC_CE_NUMBER; i++)
    {
        if ((BC_CE_SEND_FLAG & (1 << i))
            && (BC_CEList[i].eventIsGoing == 0)) //混合性报文中的周期报文,没有发生事件性报文
        {
            INT_SYS_DisableIRQGlobal();

            BC_CE_SEND_FLAG &= (~(1 << i)); //清除标志位

            INT_SYS_EnableIRQGlobal();

            tmpMsg.MsgID = BC_CEList[i].id;
            tmpMsg.isEID = (BC_CEList[i].id > 0x7ff) ? 1 : 0;
            tmpMsg.isRTR = 0;
            tmpMsg.DLC = 8;
            memset(tmpMsg.Data,0,8);
            if (data_packageFunction(tmpMsg.Data, BC_CEList[i].data_C, BC_CEList[i].mask_C,
                                     BC_CEList[i].data_C_type))
            {

            }
            else
            {
                printk(PRINTK_LEVEL_ERROR, "Data package worng!!!");
                memcpy(tmpMsg.Data, 0, 8);
            }

            APP_CANSend(BC_CEList[i].port, &tmpMsg);
        }

        if (BC_CE_EVENT_SEND_FLAG & (1 << i))  //混合性报文中的事件性报文
        {
            INT_SYS_DisableIRQGlobal();

            BC_CE_EVENT_SEND_FLAG &= (~(1 << i)); //清除标志位

            INT_SYS_EnableIRQGlobal();

            tmpMsg.MsgID = BC_CEList[i].id;
            tmpMsg.isEID = (BC_CEList[i].id > 0x7ff) ? 1 : 0;
            tmpMsg.isRTR = 0;
            tmpMsg.DLC = 8;

            memset(tmpMsg.Data, 0, 8);
            memcpy(tmpMsg.Data, BC_CEList[i].data_Event, BC_CEList[i].len_Event);
            APP_CANSend(BC_CEList[i].port, &tmpMsg);

            /*一定要在事件性报文全都加入发送队列后才能确认事件结束*/
            if (BC_CEList[i].num_Event <= 0)
            {
                BC_CEList[i].eventIsGoing = 0;    //事件性报文已经结束
            }
        }
    }

    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST2);// 此任务在一直存在
}





/**************************************
事件性广播 处理函数
id:要触发的广播 id
pdata:要发送的广播 内容的指针
len:要发送的广播内容的 指针的长度
cycle_Event：事件性报文周期
num_Event:事件性报文的发送次数

返回值 ：0：处理成功
         1：id没有注册
         2：id类型错误
         3：处理失败（其它原因）
**************************************/
unsigned char BC_Event_handle(unsigned int id ,               //  ID
                              unsigned char port,
                              unsigned char len,          //事件性报文内容长度
                              unsigned char *pdata,       // 事件性报文内容
                              unsigned char num_Event,    // 事情性报文发送次数
                              unsigned int cycle_Event)   //
{
    unsigned char i;

    for (i = 0; i < MAX_BC_EVENT_NUMBER; i++)
    {
        if ((id == BC_EventList[i].id) && (port == BC_EventList[i].port)) //判断ID及Port是否正确
        {
            if ((len >= 2) && (num_Event > 0) && (cycle_Event != 0)) // 检测传输的数据是有效的
            {

                if (BC_EventList[i].eventIsGoing == 1)
                {
                    INT_SYS_DisableIRQGlobal();

                    stop_Event_timerByID(port, id); // 停止相应定时器
                    BC_EVENT_SEND_FLAG &= (~(1 << i));   //清除此条事件报文发送标志

                    INT_SYS_EnableIRQGlobal();
                }
                else
                {
                    BC_EventList[i].eventIsGoing = 1;// 正在进行事件性报文的传输工作
                }


                BC_EventList[i].len_Event = len;
                BC_EventList[i].cycle_Event = cycle_Event;

                memcpy(BC_EventList[i].data_Event, pdata, BC_EventList[i].len_Event);

                BC_EVENT_SEND_FLAG |= (1 << i); // 立即发送一条报文,相当于事件性报文的首帧
                kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);

                if ((num_Event - 1) > 0)
                {
                    BC_EventList[i].num_Event = (num_Event - 1);
                    start_Event_timerByID(port, id);
                }
                else//如果事件性报文只发送一条，就不用开启定时器了。
                {
                    BC_EventList[i].eventIsGoing = 0;
                }

            }
            else
            {
                printk(PRINTK_LEVEL_ERROR, "the detail in message is wroang!!!");
                return (2);
            }

            break;
        }
    }

    if (i >= MAX_BC_EVENT_NUMBER)
    {
        printk(PRINTK_LEVEL_ERROR, "ID 0x %x on CAN %d is not register", id, port);
        return (1);
    }

    return 0 ;
}



/**************************************
混合性广播 处理函数
id:要触发的广播 id
pdata:要发送的广播 内容的指针
len:要发送的广播内容的 指针的长度
cycle_Event：事件性报文周期
num_Event:事件性报文的发送次数

返回值 ：0：处理成功
         ！0 处理失败

PS:本函数支持混合性报文的非同步的处理机制。
   即，在处理事件性报文的时候，不停止周期性
   报文的定时器，只是此时周期性报文不向发送
   队列中写入，事件性报文处理完成之后恢复。
**************************************/

unsigned char BC_CE_handle(unsigned int id,
                           unsigned char port,
                           unsigned char len,
                           unsigned char *pdata,
                           unsigned char num_Event,
                           unsigned int  cycle_Event)
{
    unsigned char i;

    for (i = 0; i < MAX_BC_CE_NUMBER; i++)
    {
        if ((id == BC_CEList[i].id) && (port == BC_CEList[i].port)) //判断ID及Port是否正确
        {
            if ((len >= 2) && (num_Event > 0) && (cycle_Event != 0)) // 检测传输的数据是有效的
            {

                if (BC_CEList[i].eventIsGoing == 1)
                {
                    INT_SYS_DisableIRQGlobal();

                    stop_Event_timerByID(port, id); // 停止相应定时器
                    BC_CE_EVENT_SEND_FLAG = 0;    //  发送标志

                    INT_SYS_EnableIRQGlobal();
                }
                else
                {
                    BC_CEList[i].eventIsGoing = 1;// 正在进行事件性报文的传输工作
                }


                BC_CEList[i].len_Event = len;
                BC_CEList[i].cycle_Event = cycle_Event;

                memcpy(BC_CEList[i].data_Event, pdata, BC_CEList[i].len_Event);

                BC_CE_EVENT_SEND_FLAG |= (1 << i); // 立即发送一条报文,相当于事件性报文的首帧
                kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);

                if ((num_Event - 1) > 0)
                {
                    BC_CEList[i].num_Event = (num_Event - 1);
                    start_Event_timerByID(port, id);
                }
                else//如果事件性报文只发送一条，就不用开启定时器了。
                {
                    BC_CEList[i].eventIsGoing = 0;
                }

            }
            else
            {
                printk(PRINTK_LEVEL_ERROR, "the detail in message is wroang!!!");
                return (2);
            }

            break;
        }
    }

    if (i >= MAX_BC_CE_NUMBER)
    {
        printk(PRINTK_LEVEL_ERROR, "ID 0x %x on CAN %d is not register", id, port);
        return (1);
    }

    return 0;
}


