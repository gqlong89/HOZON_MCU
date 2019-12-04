/**********************************************
*�ļ����ƣ�can_boradCast.c
*���ã�    ����CAN�㲥����
        �˷����߼��ĽǶȷ��࣬���ļ�֧�����ֹ㲥����
        1���¼��͹㲥
        2�������Թ㲥
        3���¼��Ժ������Ի�Ϲ㲥 �� ����Թ㲥
        4��ȫ���Թ㲥
*�ļ���ʱ�䣺2019��1��11��11:36:22
*����      ��INTEST-DSC
*������Ŀ������MPC��Ŀ���翧��Ŀ�ȡ�
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

BC_CYCLE_ST  BC_CycleList[MAX_BC_CYCLE_NUMBER];   // ����һ�������Թ㲥�б�
BC_ENENT_ST  BC_EventList[MAX_BC_EVENT_NUMBER];   // ����һ���¼��Թ㲥�б�
BC_CE_ST     BC_CEList[MAX_BC_CE_NUMBER];         // ����һ������Թ㲥�б�
BC_ACTIVE_ST BC_ActiveList[MAX_BC_ACTIVE_NUMBER]; // ����һ��ʹ���Թ㲥�б�


static int8_t timer_BC_Cycle[MAX_BC_CYCLE_NUMBER] = {TIMER_INVALID};
static int8_t timer_BC_Event[MAX_BC_EVENT_NUMBER] = {TIMER_INVALID};
static int8_t timer_BC_CE[MAX_BC_CE_NUMBER] = {TIMER_INVALID};
static int8_t timer_BC_Active[MAX_BC_ACTIVE_NUMBER] = {TIMER_INVALID};

static int8_t timer_BC_CE_Event[MAX_BC_CE_NUMBER] = {TIMER_INVALID}; //����Ա����е��¼��Ա��ĵ������Զ�ʱ��


unsigned char BC_CYCLE_SEND_FLAG;
unsigned char BC_EVENT_SEND_FLAG;
unsigned char BC_CE_SEND_FLAG;       //����Ա����е������Ա��ķ��ͱ�־
unsigned char BC_ACITVE_SEND_FLAG;

unsigned char BC_CE_EVENT_SEND_FLAG;// ����Ա����е��¼��Ա��ķ��ͱ�־


extern unsigned char canBasic_taskid;


#if(USED_NM_OSEK == FUNC_USED)
//extern NMControlBlock_ST  nm_OsekCB[2];//��������nm���ƿ����
#endif


/*����ά��*/
extern unsigned char buf_0x440state[8];
extern unsigned char buf_0x445state[8];
extern unsigned char buf_0x526state[8];
extern unsigned char buf_0x3d2state[8];



void clear_All_BCList(void)/*��������б�*/
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

    if (BC_CEList[0].eventIsGoing == 0) //û���¼��Ա����ڴ���
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

    if (BC_CEList[1].eventIsGoing == 0) //û���¼��Ա����ڴ���
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

    if (BC_CEList[2].eventIsGoing == 0) //û���¼��Ա����ڴ���
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

    if (BC_CEList[3].eventIsGoing == 0) //û���¼��Ա����ڴ���
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

    if (BC_CEList[4].eventIsGoing == 0) //û���¼��Ա����ڴ���
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



/*�����㲥��Ŀ����************************
  �����źž����е������ܹ�  BC_SET_ST�ṹ��
  д������͹㲥�б�

  BC_VALUE���źž����е�ֵ
  ����ֵ ��0���㲥��Ŀע��ɹ�
           ��0 ע��ʧ��
*****************************************/
unsigned char creat_BC_Item(BC_SET_ST BC_VALUE)
{
    unsigned char ret = 0;// ���
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
                        continue;//��������ѭ��
                    }

                    break; // ����FORѭ��
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
                        continue;//��������ѭ��
                    }

                    break; // ����FORѭ��
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
                        continue;//��������ѭ��
                    }

                    break; // ����FORѭ��
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
                        continue;//��������ѭ��
                    }

                    break; // ����FORѭ��
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

/*�ֶ����ù㲥����*/
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


	tmpBC.id = 0x3d2;                // ��֤���� �����¼��Ա���
	tmpBC.port = 1;
	tmpBC.type = BC_TYPE_EVENT;
	creat_BC_Item(tmpBC);	

}

/*****************************************
�㲥��ʼ������Ҫ���ݸ��㲥�б����Ѿ�ע���
�㲥��Ŀ��������ʱ����
����ֵ ��0���ɹ�
        ��0��ʧ��
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
    //�����㲥��Ŀ

    for (i = 0; i < MAX_BC_CYCLE_NUMBER; i++)
    {
        if (BC_CycleList[i].used == BC_USED)
        {
            if (timer_BC_Cycle[i] == TIMER_INVALID) // �˶�ʱ����û�д���
            {
                timer_BC_Cycle[i] = ltimer_create(timer_BC_CycleISR[i]);//ֻ������û�п���
                
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
            if (timer_BC_Event[i] == TIMER_INVALID) // �˶�ʱ����û�д���
            {
                timer_BC_Event[i] = ltimer_create(timer_BC_EventISR[i]);//ֻ������û�п���
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
            if (timer_BC_CE[i] == TIMER_INVALID) // �˶�ʱ����û�д���,����Ա����е������Զ�ʱ��
            {
                timer_BC_CE[i] = ltimer_create(timer_BC_CEISR[i]);//ֻ������û�п���
            }
            else
            {
                printk(PRINTK_LEVEL_ERROR, "Timer for broadCast CE %d is used,Wrong!", i);
            }

            if (timer_BC_CE_Event[i] == TIMER_INVALID) // ����Ա����е��¼��Զ�ʱ����
            {
                timer_BC_CE_Event[i] = ltimer_create(timer_BC_CE_EVENTISR[i]);//ֻ������û�п���
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

/*�������Ա��ķ��ͺ���*******************
  �����Ա��� ���巢�ͺ���
  ���з��ʹ�����ά���ͼ��

seq:ָ��Ҫ�����Ա����б��е���һ������
����ֵ��0�����ͳɹ�
        ������ 0������ʧ��
****************************************/


/*�����Ա��Ĵ���������AAP�����������*/


/**************************
�ֶ������㲥���Ͷ�ʱ��
id:����ֵ ��Ϊ��ӦID�㲥
   0xfffffffe,����ID

    ID�Ա�ֻ�������Ա����б�ͻ���Ա����б��н��С�


 ����ֵ ��

        0���ɹ�
        1��idû��ע��
**************************/
unsigned char start_BC_timerByID(unsigned char canport, unsigned int id)
{
    unsigned char i;

    if (id == 0xfffffffe)
    {
        //��������ͨ�������������Ա���
        //��ʱ�ò������Ȳ�д��Ҫ��дҲ��д
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
                    break;//����ѭ��
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
�ֶ������㲥���Ͷ�ʱ��
canport:ΪCANͨͨ��
PS:��ָ���������Ա��ĺͻ���Ա����е������Ա��Ķ�ʱ��

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
�ֶ�ֹͣ�㲥���Ͷ�ʱ��
canport:CAN�˿�
id:����ֵ ��Ϊ��ӦID�㲥
   0xfffffffe,����ID

 ����ֵ ��

        0���ɹ�
        1��idû��ע��
PS��һ��Ҫ�ж�ͨ������ֹ����ͨ��������ͬ��ID��
**************************/
unsigned char stop_BC_timerByID(unsigned char canport, unsigned int id)
{
    unsigned char i;

    if (id == 0xfffffffe)
    {
        //�ر�����ͨ�������������Ա���
        //��ʱ�ò������Ȳ�д��Ҫ��дҲ��д
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
                    break;//����ѭ��
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
�ֶ�ֹͣ�㲥���Ͷ�ʱ��
canport:ΪCANͨͨ��
PS:ֹͣ��ʱ��ָֹͣ��ӦCAN���ϵ�
   ����APP��ʱ�������������Զ�ʱ�����¼��Զ�ʱ��

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
                BC_CEList[i].num_Event = 0;           //����¼��Ա���ʣ�෢�ʹ�����
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
                BC_EventList[i].num_Event = 0;      //����¼��Ա���ʣ�෢�ʹ���
            }
            else
            {
                break;
            }
        }
    }
}


/*�����¼�*/
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
                break;//����ѭ��
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


/*�ر��¼�*/
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
                break;//����ѭ��
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
  ���ݷ�װ���ܺ���

  p_des:��װ������ݴ��ָ��
  p_src:����װ���ݵ���Դָ��
  p_mask:���ݵ� λ ����� ָ��
  data_type:����װ���ݵ����͡�

  ����ֵ��ture:���ݷ�װ�ɹ�

false:���ݷ�װʧ�ܣ�û����Ӧ���������͡�

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

        case   HZ_TIME:   // ����ʱ��  
            {
                driverRTCGetTime(&current_time1);      // ʵʱ��װ����    
                
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

		case   HZ_0X440:    //����0x440����
			{
				memcpy(p_des, buf_0x440state, 8);

				break;		
			}
		case   HZ_0X445:   // ����0x445���� 
			{
				memcpy(p_des, buf_0x445state, 8);

				break;
			}
		case   HZ_0X526:      // ����0x526����
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
�ֶ����ͷ��й㲥����֡
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
    Ӧ�ñ��ķ��ʹ�����

    ����Ƿ���Ӧ�ñ�����Ҫ���ͣ���Ҫ����
    Ӧ�ñ��ķ��ͱ�־��

    ����Ҫ���͵�Ӧ�ñ��Ľ������ݷ�װ��֮��
    ���뵽 ���Ͷ���֮�С�

*********************************************/
void APP_SendHandle(void)//�����û������Ҫ���з�װ
{
    unsigned char i;
    CAN_SEND_MSG tmpMsg;


    for (i = 0; i < MAX_BC_CYCLE_NUMBER; i++)
    {
        if (BC_CYCLE_SEND_FLAG & (1 << i))
        {
            INT_SYS_DisableIRQGlobal();

            BC_CYCLE_SEND_FLAG &= (~(1 << i)); //�����־λ

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

            BC_EVENT_SEND_FLAG &= (~(1 << i)); //�����־λ

            INT_SYS_EnableIRQGlobal();

            tmpMsg.MsgID = BC_EventList[i].id;
            tmpMsg.isEID = (BC_EventList[i].id > 0x7ff) ? 1 : 0;
            tmpMsg.isRTR = 0;
            tmpMsg.DLC = 8;

            memset(tmpMsg.Data, 0, 8);
            memcpy(tmpMsg.Data, BC_EventList[i].data_Event, BC_EventList[i].len_Event);

            APP_CANSend(BC_CycleList[i].port, &tmpMsg);

            /*һ��Ҫ���¼��Ա���ȫ�����뷢�Ͷ��к����ȷ���¼�����*/
            if (BC_EventList[i].num_Event <= 0)
            {
                BC_EventList[i].eventIsGoing = 0;    //�¼��Ա����Ѿ�����
            }
        }
    }



    for (i = 0; i < MAX_BC_CE_NUMBER; i++)
    {
        if ((BC_CE_SEND_FLAG & (1 << i))
            && (BC_CEList[i].eventIsGoing == 0)) //����Ա����е����ڱ���,û�з����¼��Ա���
        {
            INT_SYS_DisableIRQGlobal();

            BC_CE_SEND_FLAG &= (~(1 << i)); //�����־λ

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

        if (BC_CE_EVENT_SEND_FLAG & (1 << i))  //����Ա����е��¼��Ա���
        {
            INT_SYS_DisableIRQGlobal();

            BC_CE_EVENT_SEND_FLAG &= (~(1 << i)); //�����־λ

            INT_SYS_EnableIRQGlobal();

            tmpMsg.MsgID = BC_CEList[i].id;
            tmpMsg.isEID = (BC_CEList[i].id > 0x7ff) ? 1 : 0;
            tmpMsg.isRTR = 0;
            tmpMsg.DLC = 8;

            memset(tmpMsg.Data, 0, 8);
            memcpy(tmpMsg.Data, BC_CEList[i].data_Event, BC_CEList[i].len_Event);
            APP_CANSend(BC_CEList[i].port, &tmpMsg);

            /*һ��Ҫ���¼��Ա���ȫ�����뷢�Ͷ��к����ȷ���¼�����*/
            if (BC_CEList[i].num_Event <= 0)
            {
                BC_CEList[i].eventIsGoing = 0;    //�¼��Ա����Ѿ�����
            }
        }
    }

    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST2);// ��������һֱ����
}





/**************************************
�¼��Թ㲥 ������
id:Ҫ�����Ĺ㲥 id
pdata:Ҫ���͵Ĺ㲥 ���ݵ�ָ��
len:Ҫ���͵Ĺ㲥���ݵ� ָ��ĳ���
cycle_Event���¼��Ա�������
num_Event:�¼��Ա��ĵķ��ʹ���

����ֵ ��0������ɹ�
         1��idû��ע��
         2��id���ʹ���
         3������ʧ�ܣ�����ԭ��
**************************************/
unsigned char BC_Event_handle(unsigned int id ,               //  ID
                              unsigned char port,
                              unsigned char len,          //�¼��Ա������ݳ���
                              unsigned char *pdata,       // �¼��Ա�������
                              unsigned char num_Event,    // �����Ա��ķ��ʹ���
                              unsigned int cycle_Event)   //
{
    unsigned char i;

    for (i = 0; i < MAX_BC_EVENT_NUMBER; i++)
    {
        if ((id == BC_EventList[i].id) && (port == BC_EventList[i].port)) //�ж�ID��Port�Ƿ���ȷ
        {
            if ((len >= 2) && (num_Event > 0) && (cycle_Event != 0)) // ��⴫�����������Ч��
            {

                if (BC_EventList[i].eventIsGoing == 1)
                {
                    INT_SYS_DisableIRQGlobal();

                    stop_Event_timerByID(port, id); // ֹͣ��Ӧ��ʱ��
                    BC_EVENT_SEND_FLAG &= (~(1 << i));   //��������¼����ķ��ͱ�־

                    INT_SYS_EnableIRQGlobal();
                }
                else
                {
                    BC_EventList[i].eventIsGoing = 1;// ���ڽ����¼��Ա��ĵĴ��乤��
                }


                BC_EventList[i].len_Event = len;
                BC_EventList[i].cycle_Event = cycle_Event;

                memcpy(BC_EventList[i].data_Event, pdata, BC_EventList[i].len_Event);

                BC_EVENT_SEND_FLAG |= (1 << i); // ��������һ������,�൱���¼��Ա��ĵ���֡
                kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);

                if ((num_Event - 1) > 0)
                {
                    BC_EventList[i].num_Event = (num_Event - 1);
                    start_Event_timerByID(port, id);
                }
                else//����¼��Ա���ֻ����һ�����Ͳ��ÿ�����ʱ���ˡ�
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
����Թ㲥 ������
id:Ҫ�����Ĺ㲥 id
pdata:Ҫ���͵Ĺ㲥 ���ݵ�ָ��
len:Ҫ���͵Ĺ㲥���ݵ� ָ��ĳ���
cycle_Event���¼��Ա�������
num_Event:�¼��Ա��ĵķ��ʹ���

����ֵ ��0������ɹ�
         ��0 ����ʧ��

PS:������֧�ֻ���Ա��ĵķ�ͬ���Ĵ�����ơ�
   �����ڴ����¼��Ա��ĵ�ʱ�򣬲�ֹͣ������
   ���ĵĶ�ʱ����ֻ�Ǵ�ʱ�����Ա��Ĳ�����
   ������д�룬�¼��Ա��Ĵ������֮��ָ���
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
        if ((id == BC_CEList[i].id) && (port == BC_CEList[i].port)) //�ж�ID��Port�Ƿ���ȷ
        {
            if ((len >= 2) && (num_Event > 0) && (cycle_Event != 0)) // ��⴫�����������Ч��
            {

                if (BC_CEList[i].eventIsGoing == 1)
                {
                    INT_SYS_DisableIRQGlobal();

                    stop_Event_timerByID(port, id); // ֹͣ��Ӧ��ʱ��
                    BC_CE_EVENT_SEND_FLAG = 0;    //  ���ͱ�־

                    INT_SYS_EnableIRQGlobal();
                }
                else
                {
                    BC_CEList[i].eventIsGoing = 1;// ���ڽ����¼��Ա��ĵĴ��乤��
                }


                BC_CEList[i].len_Event = len;
                BC_CEList[i].cycle_Event = cycle_Event;

                memcpy(BC_CEList[i].data_Event, pdata, BC_CEList[i].len_Event);

                BC_CE_EVENT_SEND_FLAG |= (1 << i); // ��������һ������,�൱���¼��Ա��ĵ���֡
                kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);

                if ((num_Event - 1) > 0)
                {
                    BC_CEList[i].num_Event = (num_Event - 1);
                    start_Event_timerByID(port, id);
                }
                else//����¼��Ա���ֻ����һ�����Ͳ��ÿ�����ʱ���ˡ�
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


