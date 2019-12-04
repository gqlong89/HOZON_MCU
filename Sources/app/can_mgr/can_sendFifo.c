



#include <app/can_mgr/can_hardware.h>
#include <app/can_mgr/can_mgr.h>
#include <app/can_mgr/can_broadCast.h>
#include <app/can_mgr/canbus_off.h>
#include <app/scom/scom_tl_ex.h>
#include <app/tp/uds/UDS_Config.h>
#include <config.h>
#include <mid/can_flexcan/can_flexcan.h>
#include <osif.h>
#include <string.h>
#include <timer.h>
#include <mid/kernel/task.h>
#include <flexcan_hal.h>
#include <mid/kernel/kernel.h>
#include <app/shell/shell_list.h>
#include <rtc.h>
#include <app/can_mgr/can_voltage.h>
#include <app/nm_osek/nm_osekCore.h>
#include <app/can_mgr/can_sendFifo.h>
#include <interrupt_manager.h>



CAN_SENDFIFO_ST  APP_CANSendFifo[2];

extern unsigned char canBasic_taskid;

/***********************************************
  清空发送队列
************************************************/
void APP_ClearTxFifo(unsigned char canport)
{
    APP_CANSendFifo[canport].p_in = APP_CANSendFifo[canport].p_out = 0;
    printk(PRINTK_LEVEL_LOG, "Clear CAN %d Tx FIFO OK!!!", canport);
}



/*************************************************
    应用报文CAN发送函数

  定义一个CAN发送函数，专门用于发送APP报文。
  因为APP报文的邮箱已经指定，故这里不用指定邮箱。

  PS：此函数不能在中断中使用
  实际上这个函数是向发送队列里写数据用的

  PS：当前发送队列的深度为10，正常情况下不会溢出。
*************************************************/
unsigned char APP_CANSend(unsigned char canport, CAN_SEND_MSG *canMsg)
{
    if (canport >= 2)
    {
        printk(PRINTK_LEVEL_LOG, "CAN PORT %d is Wrong!!!", canport);
        return (1);
    }

    if (check_busOffState(canport) == 1) //如果已经是BUSOFF，不向队列中写数据
    {
        printk(PRINTK_LEVEL_LOG, "CAN %d BUS off,can not send message!!!", canport);
        return (2);
    }

    /*如果消息队列溢出，则不向其写入*/
    if ((APP_CANSendFifo[canport].p_in + 1) % APP_CANSENDFIFO_NUM == APP_CANSendFifo[canport].p_out)
    {
        //  printk(PRINTK_LEVEL_LOG, "CAN %d send fifo is overflow!!!", canport);
        return (3);
    }

    memcpy(&APP_CANSendFifo[canport].buf[APP_CANSendFifo[canport].p_in], canMsg, sizeof(CAN_SEND_MSG));

    APP_CANSendFifo[canport].p_in = (APP_CANSendFifo[canport].p_in + 1) % APP_CANSENDFIFO_NUM;

    return (0);//写入FIFO成功。
}



/********************************************
    应用报文发送队列处理函数

    处理相应通道的发送队列，具体是将发送队列内
    数据从CAN口发送出去。

********************************************/
unsigned char APP_CANSendFifoHandle(unsigned char canport)
{
    int ret = 0;

    while (APP_CANSendFifo[canport].p_in != APP_CANSendFifo[canport].p_out)
    {
        ret = can_send(canport, MAIL_TRY, &APP_CANSendFifo[canport].buf[APP_CANSendFifo[canport].p_out]);

		if (ret != 0) // 表示发送失败，即向邮箱中写数据失败
        {
			kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST2);// 此任务在一直存在
			return ret;
        }
        else  // 发送 成功，读指针向前移位
        {
            APP_CANSendFifo[canport].p_out = (APP_CANSendFifo[canport].p_out + 1) % APP_CANSENDFIFO_NUM;
        }
    }

    return ret;
}

