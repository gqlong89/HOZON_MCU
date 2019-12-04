



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
  ��շ��Ͷ���
************************************************/
void APP_ClearTxFifo(unsigned char canport)
{
    APP_CANSendFifo[canport].p_in = APP_CANSendFifo[canport].p_out = 0;
    printk(PRINTK_LEVEL_LOG, "Clear CAN %d Tx FIFO OK!!!", canport);
}



/*************************************************
    Ӧ�ñ���CAN���ͺ���

  ����һ��CAN���ͺ�����ר�����ڷ���APP���ġ�
  ��ΪAPP���ĵ������Ѿ�ָ���������ﲻ��ָ�����䡣

  PS���˺����������ж���ʹ��
  ʵ����������������Ͷ�����д�����õ�

  PS����ǰ���Ͷ��е����Ϊ10����������²��������
*************************************************/
unsigned char APP_CANSend(unsigned char canport, CAN_SEND_MSG *canMsg)
{
    if (canport >= 2)
    {
        printk(PRINTK_LEVEL_LOG, "CAN PORT %d is Wrong!!!", canport);
        return (1);
    }

    if (check_busOffState(canport) == 1) //����Ѿ���BUSOFF�����������д����
    {
        printk(PRINTK_LEVEL_LOG, "CAN %d BUS off,can not send message!!!", canport);
        return (2);
    }

    /*�����Ϣ���������������д��*/
    if ((APP_CANSendFifo[canport].p_in + 1) % APP_CANSENDFIFO_NUM == APP_CANSendFifo[canport].p_out)
    {
        //  printk(PRINTK_LEVEL_LOG, "CAN %d send fifo is overflow!!!", canport);
        return (3);
    }

    memcpy(&APP_CANSendFifo[canport].buf[APP_CANSendFifo[canport].p_in], canMsg, sizeof(CAN_SEND_MSG));

    APP_CANSendFifo[canport].p_in = (APP_CANSendFifo[canport].p_in + 1) % APP_CANSENDFIFO_NUM;

    return (0);//д��FIFO�ɹ���
}



/********************************************
    Ӧ�ñ��ķ��Ͷ��д�����

    ������Ӧͨ���ķ��Ͷ��У������ǽ����Ͷ�����
    ���ݴ�CAN�ڷ��ͳ�ȥ��

********************************************/
unsigned char APP_CANSendFifoHandle(unsigned char canport)
{
    int ret = 0;

    while (APP_CANSendFifo[canport].p_in != APP_CANSendFifo[canport].p_out)
    {
        ret = can_send(canport, MAIL_TRY, &APP_CANSendFifo[canport].buf[APP_CANSendFifo[canport].p_out]);

		if (ret != 0) // ��ʾ����ʧ�ܣ�����������д����ʧ��
        {
			kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST2);// ��������һֱ����
			return ret;
        }
        else  // ���� �ɹ�����ָ����ǰ��λ
        {
            APP_CANSendFifo[canport].p_out = (APP_CANSendFifo[canport].p_out + 1) % APP_CANSENDFIFO_NUM;
        }
    }

    return ret;
}

