

/*
*NM osek core �ļ�
*����  DSC


****/

/*������������ʼ��
  ��Ҫ��ʼ������Ϊ��
  1:NM ID����ַ
  2:���ڵ��ַ
  3:����ʱ��ʱ��(��λMS)
    Ttyp
    Tmax
    Terror
    Tty
    TwaitBusSleep
  4:
*/
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
#include <string.h>


#include <app/nm_osek/nm_osekCore.h>
#include <app/nm_osek/nm_osekTimer.h>
#include <flexcan_hal.h>
#include "S32K144/include/S32K144.h"
#include <mid/can_flexcan/can_flexcan.h>
#include <app/custom/custom.h>
#include <app/scom/scom_can_msg.h>
#include <scom_msg_def.h>
#include <app/can_mgr/can_basic_cfg.h>




#if USED_NM_OSEK


#define SPI_TLS_NM_LIMPHOME_STATE   TASK_SYS_EVENT_REQST2  // ADD BY DSC

NMControlBlock_ST  nm_OsekCB[2];//��������nm���ƿ����

extern CAN_BASIC_CFG_ST can_basic_cfg;  // �ⲿ�����CAN���� ���� ���� ��

extern unsigned char canBasic_taskid;    

extern unsigned char scom_taskid;

void InitDirectNMparms(unsigned char canport) // ��ʼ��ֱ������������
{
    if (canport > 2)
    {
        printk(PRINTK_LEVEL_ERROR, "CAN port %d >=2", canport);
        return;
    }

    nm_OsekCB[canport].TTyp = TTyp_Value;
    nm_OsekCB[canport].TMax = TMax_Value;
    nm_OsekCB[canport].TTx = TTx_Value;
    nm_OsekCB[canport].TError = TError_Value;
    nm_OsekCB[canport].TWbs = TWBS_Value;
    nm_OsekCB[canport].TSleepReqMin_LimpHome = TSleepReqMin_LimpHome_Value;
    nm_OsekCB[canport].TSleepReqMin_Normal = TSleepReqMin_Normal_Value;
    nm_OsekCB[canport].TAppActive = TAppActive_Value;// Ӧ�ñ��� ��֡��ʱ��

    nm_OsekCB[canport].nmBaseADDR = can_basic_cfg.nm_cfg[canport].nm_osek_cfg.nmo_base_id;
    nm_OsekCB[canport].nmMaxID = can_basic_cfg.nm_cfg[canport].nm_osek_cfg.nmo_max_id;
    nm_OsekCB[canport].nmMinID = can_basic_cfg.nm_cfg[canport].nm_osek_cfg.nmo_mix_id;
    nm_OsekCB[canport].nmLocalID = can_basic_cfg.nm_cfg[canport].nm_osek_cfg.nmo_locol_id;
}

void TalkNM(unsigned char canport) // ����NMͨѶ
{
    if (canport >= 2)
    {
        printk(PRINTK_LEVEL_ERROR, "CAN port %d >=2", canport);
        return;
    }

    nm_OsekCB[canport].NetworkStatus.bit.NMactive = 1;//����NMͨѶ
}

void SilentNM(unsigned char canport)  // ��ֹNMͨѶ
{
    if (canport >= 2)
    {
        printk(PRINTK_LEVEL_ERROR, "CAN port %d >=2", canport);
        return;
    }

    nm_OsekCB[canport].NetworkStatus.bit.NMactive = 0;//��ֹNMͨѶ
}

void D_Online(unsigned char canport) // ����ӦͨѶ
{
    if (canport >= 2)
    {
        printk(PRINTK_LEVEL_ERROR, "CAN port %d >=2", canport);
        return;
    }

    nm_OsekCB[canport].NetworkStatus.bit.NMAPPactive = 1;   // ����Ӧ��ͨѶ
}

void D_Offline(unsigned char
               canport)  // ��ֹӦ��ͨѶ// ���ô˺����󣬾ͻ����TWBS״̬���Ͳ����Եķ���Ӧ�ñ��� ��
{
    if (canport >= 2)
    {
        printk(PRINTK_LEVEL_ERROR, "CAN port %d >=2", canport);
        return;
    }

    nm_OsekCB[canport].NetworkStatus.bit.NMAPPactive = 0;   // ��ֹӦ��ͨѶ
    stop_BC_timerByChl(canport);// ���� ֹͣӦ�ñ��� ���Ͷ�ʱ��
}

void NM_Osek_Init(unsigned char canport)
{
    InitDirectNMparms(canport);

}
extern CAN_Type *const g_flexcanBase[];
void D_Start(unsigned char canport)
{
    CAN_Type *base = g_flexcanBase[canport];
    nm_OsekCB[canport].NetworkStatus.bit.canModeNormal = 1;
    FLEXCAN_HAL_SetOperationMode(base, FLEXCAN_NORMAL_MODE); //ʹCAN�ڴ�����������״̬
}

void D_Stop(unsigned char canport)
{
    CAN_Type *base = g_flexcanBase[canport];
    nm_OsekCB[canport].NetworkStatus.bit.canModeNormal = 0;
    FLEXCAN_HAL_SetOperationMode(base, FLEXCAN_LISTEN_ONLY_MODE); //ʹCAN�ڴ�����״̬
    flexcan_clear_errstatus(canport);
    flexcan_set_errint(canport, true);                           //��������CAN�����ж�
}


void D_Init(unsigned char canport)
{
    nm_OsekCB[canport].NetworkStatus.bit.canModeNormal = 0;
    //can_init(canport, can_default_baud[canport]);//��ʼ��CANӲ���ӿ�
   // D_Stop(canport);// ��ʹ֮���ڼ���״̬
   // scom_Custom_Init();
}



/*���ݴ�������ͺ���
    ����   ture  ,���ͳɹ���
    ���� false, ����ʧ��

*/
bool D_WindowDataReq(unsigned char canport, NMPDU_ST *nmpdu, unsigned char len)
{
    CAN_SEND_MSG msg;

    if (canport >= 2)
    {
        printk(PRINTK_LEVEL_ERROR, "CAN port %d >=2", canport);
        return false;
    }


    memset((unsigned char *)&msg, 0, sizeof(msg));

    msg.DLC = len;
    msg.MsgID = (nm_OsekCB[canport].nmLocalID | nm_OsekCB[canport].nmBaseADDR);
    msg.Data[0] = nm_OsekCB[canport].logical_successor;
    msg.Data[1] = nmpdu->opCode.byte;
    memcpy(msg.Data + 2, nmpdu->ringData, sizeof(nmpdu->ringData));

    if (can_send(canport, TX_CAN_NM_OSEK, &msg) == 0) // ע�� �����е���������� ʹ���ض���������з��͡�
    {
        return true;   // ���ͳɹ�
    }

    return false;  //����ʧ��
}



/**/
void  nmInitReset6(unsigned char    canport, bool sleepInd) //reset from normal
{
    D_Online(canport);//����RESET�󣬶�������Ӧ�ú���

    nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
    nm_OsekCB[canport].currentState = NM_stInitReset;
    printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stInitReset !!!", canport);
	

    memset(nm_OsekCB[canport].normalList, 0, 32); //�������״̬�ڵ��б�
    nmAddtoPresent(canport, nm_OsekCB[canport].nmLocalID);//�����ڵ���뵽�����ڵ��б��У�Э��ջҪ��

    nm_OsekCancleAlarm(canport, TTyp_MASK);
    nm_OsekCancleAlarm(canport, TMax_MASK);
    nm_OsekCancleAlarm(canport, TError_MASK);
    nm_OsekCancleAlarm(canport, TTx_MASK);
    nm_OsekCancleAlarm(canport, TWBS_MASK);
    nm_OsekCancleAlarm(canport, TSleepReqMin_LimpHome_MASK);  // ����ط�Ҫ��һ��,���Ը�ɺ���һ��ֹͣ
    nm_OsekCancleAlarm(canport, TSleepReqMin_Normal_MASK);  //��ͣ���ж�ʱ������

    nm_OsekCB[canport].nmRxCount++;   // ���ʹ����������1.

    /*����Ҫ���������Ƿ���Alive����*/

	if ((nm_OsekCB[canport].lastState == NM_stBusSleep)
            || (nm_OsekCB[canport].lastState == NM_stTwbsLimphome)
            || (nm_OsekCB[canport].lastState == NM_stTwbsNormal))
    {
         nm_OsekSetAlarm(canport, TappActive_MASK); //������֡Ӧ�ñ��Ķ�ʱ��

    }
    /*��Ϊ����һ���ᷢALIVE���ʣ��Ȱѷ��ʹ������ˣ�Ϊ�˿�������Ttyp��ʱ��*/
	if (nm_OsekCB[canport].NetworkStatus.bit.NMactive != 0) //��ʾ�Ѿ����ù�TaklNM
	{
		nm_OsekCB[canport].nmTxCount++;
	}
    /*Ҫ�����ͺ���֮ǰ������Ӧ��ʱ���� ����ᵼ��Ttyp��׼ */
	if ((nm_OsekCB[canport].nmTxCount <= TX_ERROR_LIMTT)
        && (nm_OsekCB[canport].nmRxCount <= RX_ERROR_LIMIT))
    {
        nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
        nm_OsekCB[canport].currentState = NM_stNormal;
        printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stNormal !!!", canport);
        nm_OsekSetAlarm(canport, TTyp_MASK);

        nm_OsekSetAlarm(canport, TSleepReqMin_Normal_MASK);
        nm_OsekCB[canport].NetworkStatus.bit.presleepenable = 0;//  û��������
    }
    else  //  ��ʾ���ͻ���մ��󳬹���ֵ��Ҫ����LIMPHOME״̬
    {
        nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
        nm_OsekCB[canport].currentState = NM_stLimphome;
        printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stLimphome !!!", canport);

        nm_OsekSetAlarm(canport, TError_MASK);
        //����LIMPHOME  ����ʱ��ʱ��
        nm_OsekSetAlarm(canport, TSleepReqMin_LimpHome_MASK);
        nm_OsekCB[canport].NetworkStatus.bit.presleepenable = 0;
	//	kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//��MPUͬ�� NM limphome״̬
    }
	
	/*����Alive����*/

    if (nm_OsekCB[canport].NetworkStatus.bit.NMactive != 0) //��ʾ�Ѿ����ù�TaklNM
    {
        nm_OsekCB[canport].logical_successor =
        nm_OsekCB[canport].nmLocalID;// ��û���뻷��ʱ�� ���߼��̳ж����ǽڵ㱾��

        nm_OsekCB[canport].NetworkStatus.bit.sleepAck = 0;//�������һ��RING����  �ͷ���SLEEPACK

        nm_OsekCB[canport].TxPDU.opCode.byte = 0;//���������
        nm_OsekCB[canport].TxPDU.opCode.bit.alive = 1;

        if (nm_OsekCB[canport].NetworkStatus.bit.bussleep && sleepInd)
        {
            nm_OsekCB[canport].TxPDU.opCode.bit.sleepInd = 1;
        }
        else
        {
            nm_OsekCB[canport].TxPDU.opCode.bit.sleepInd = 0;
        }

        if (D_WindowDataReq(canport, &(nm_OsekCB[canport].TxPDU), 8) == false) //���ͺ���
        {
            nm_OsekSetAlarm(canport, TTx_MASK);// �����ط���ʱ��
        }
        else
        {
            printk(PRINTK_LEVEL_LOG, "Send Alive message OK!!!");
        }

        if ((nm_OsekCB[canport].lastState == NM_stBusSleep)
            || (nm_OsekCB[canport].lastState == NM_stTwbsLimphome)
            || (nm_OsekCB[canport].lastState == NM_stTwbsNormal))
        {
            nm_OsekSetAlarm(canport, TappActive_MASK); //������֡Ӧ�ñ��Ķ�ʱ��

        }
    }
}


/*���ڴ�limpHome״̬  ת��RESET״̬ʱ�������õ�RESET����
  ���������£�û����� LIMPHOME��
*/
void nmInitReset5(unsigned char canport)// reset from limpHome
{

    nm_OsekCancleAlarm(canport, TError_MASK);//ȡ��ERROR��ʱ��
    nm_OsekCB[canport].nmMerker.bit.limphome = 0;
    nm_OsekCB[canport].nmTxCount = 0;
    nm_OsekCB[canport].nmRxCount = 0;
    nmInitReset6(canport, true);

}


void nmInit3(unsigned char canport)//reset form bussleep
{
    memset(nm_OsekCB[canport].limphomeList, 0, 32); //���LIMPHOME �ڵ��б�
    nm_OsekCB[canport].nmTxCount = 0;
    nm_OsekCB[canport].nmRxCount = 0;
    nmInitReset6(canport, false);//�ϵ�׶Σ�ALIVE���Ĳ��ܷ�SLEED��IND
}

extern unsigned char scom_taskid;
#define SPI_TLS_NM_STATE            TASK_SYS_EVENT_REQST0
#define SPI_TLS_BUSOFF_STATE        TASK_SYS_EVENT_REQST1


void nmBusSleep(unsigned char canport)
{
	D_Stop(canport);

	nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
    nm_OsekCB[canport].currentState = NM_stBusSleep;
    printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stBusSleep !!!", canport);
   // D_Stop(canport);

    nm_OsekCB[canport].nmTimerOverFlow = 0;//��ն�ʱ����־
    nm_OsekCB[canport].receive_NM_Flag = 0;// ��ս��ձ�־

    nm_OsekCancleAlarm(canport, TTx_MASK | TTyp_MASK | TMax_MASK | TError_MASK |
                       TSleepReqMin_LimpHome_MASK | TSleepReqMin_Normal_MASK | TWBS_MASK); //ֹͣ���еĶ�ʱ��
    //��MPU֪ͨת��BUSSLEEP
	kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_STATE);//
}


/*�ж��߼��̳���
�ڽ��յ�NM����ʱ������
S�����յı��ĵ�Դ��ַ
R�����ص�ַ
L���߼��̳���
����ֵ ��Ϊ���µ��߼��̳���*/
static unsigned char nmDetermineLS(unsigned char S, unsigned char R, unsigned char L)
{
    unsigned char  newL = L;

    if (L == R)
    {
        newL = S;
    }
    else
    {
        if (L < R)
        {
            if (S < L)
            {
                newL = S; /* SLR */
            }
            else
            {
                if (S < R)
                {
                    /* LSR */
                }
                else
                {
                    newL = S; /* LRS */
                }
            }
        }
        else
        {
            if (S < L)
            {
                if (S < R)
                {
                    /* SRL */
                }
                else
                {
                    newL = S; /* RSL */
                }
            }
            else
            {
                /* RLS */
            }
        }
    }

    return newL;
}


/*�ж��Լ��Ƿ񱻻���
�ڽ��յ�NM����ʱ������
S���յ���NM���ĵ�Դ��ַ
R�����ص�ַ
D���յ���NM���ĵ�Ŀ�ĵ�ַ
����ֵ ��
     false:û�б�����
     true: ������*/
static bool nmIsMeSkipped(unsigned char S, unsigned char R, unsigned char D)
{
    bool isSkipped = false;

    if (D < R)
    {
        if (S < D)
        {
            /* not skipped SDR */
        }
        else
        {
            if (S < R)
            {
                isSkipped = true; /* DRS */
            }
            else
            {
                /* not skipped DSR */
            }
        }
    }
    else
    {
        if (S < D)
        {
            if (S < R)
            {
                isSkipped = true; /* SRD */
            }
            else
            {
                /* RSD */
            }
        }
        else
        {
            isSkipped = true; /* RDS */
        }
    }

    return isSkipped;
}


/*��ĳ���ڵ��ĳ��״̬�б����Ƴ�*/
static void nmRemoveFromConfig(unsigned char canport, ConfigKindName_E configKind,
                               unsigned char nodeId)
{
    unsigned char *config = NULL;

    switch (configKind)
    {
        case NM_ckNormal:
            config = nm_OsekCB[canport].normalList;
            break;

        case NM_ckLimphome:
            config = nm_OsekCB[canport].limphomeList;
            break;

        default:
            break;
    }

    if (NULL != config)
    {
        unsigned char byte    = nodeId / 8;
        unsigned char bit     = nodeId % 8;
        config[byte] &= (~(1u << bit));
    }                                          //��һλ��ʾһ���ڵ㣬������뷨
}


/*���ڵ���뵽ĳ��״̬�б���*/
void nmAddToConfig(unsigned char canport, ConfigKindName_E configKind, unsigned char nodeId)
{
    unsigned char *config = NULL;

    switch (configKind)
    {
        case NM_ckNormal:
            config = nm_OsekCB[canport].normalList;//���ڵ���� ��NORMALL�б���
            break;

        case NM_ckLimphome:
            config = nm_OsekCB[canport].limphomeList;//���ڵ���뵽LIMPHOME�б���
            break;

        default:
            break;
    }

    if (NULL != config)
    {
        unsigned char byte    = nodeId / 8;
        unsigned char bit     = nodeId % 8;
        config[byte] |= (1u << bit);
    }
}


/*���ڵ���뵽����״̬�б���*/
void nmAddtoPresent(unsigned char canport, unsigned char nodeId)
{
    nmAddToConfig(canport, NM_ckNormal , nodeId);
    nmRemoveFromConfig(canport, NM_ckLimphome, nodeId);
}

/*���ڵ���뵽LIMPHOME�б���*/
void nmAddtoLimphome(unsigned char canport, unsigned char nodeId)
{
    nmAddToConfig(canport, NM_ckLimphome, nodeId);
    //�ǲ���Ҫ�������б����Ƴ���Ҫ����һ�£�����Ŀǰû�õ���һ�顣
}

/*�˺�������NORMAL״̬�£�������յ���NM���ı�׼������Э��ջ�еĺ���*/
static void nmNormalStandard(unsigned char canport, NMPDU_ST nmpdu)
{
    nm_OsekCB[canport].nmRxCount = 0;//���յ�NM���ģ���Ҫ��ս��մ��������

    if (nmpdu.opCode.bit.limphome)
    {
        nmAddtoLimphome(canport, nmpdu.source);
    }
    else
    {
        nmAddtoPresent(canport, nmpdu.source);

        nm_OsekCB[canport].logical_successor = nmDetermineLS(nmpdu.source, nm_OsekCB[canport].nmLocalID,
                                               nm_OsekCB[canport].logical_successor);//�ж��߼��̳���

        if (nmpdu.opCode.bit.alive)
        {
            nm_OsekCB[canport].nmMerker.bit.stable = 0;//��ʾ�����ȶ�
            nm_OsekCB[canport].NetworkStatus.bit.configurationstable = 0;// ���ò��ȶ�
            printk(PRINTK_LEVEL_LOG, "Receive Alive message!!!");
        }

        if (nmpdu.opCode.bit.ring) //��ʾ��RING����
        {
            nm_OsekCancleAlarm(canport, TTyp_MASK | TMax_MASK);

        //    if ((nmpdu.source == nmpdu.destination)
        //        || (nmpdu.destination == nm_OsekCB[canport].nmLocalID)) //����Ƿ����ҵı���
           	if (nmpdu.destination == nm_OsekCB[canport].nmLocalID)
            {
                nm_OsekSetAlarm(canport, TTyp_MASK);
            }
            else
            {
                nm_OsekSetAlarm(canport, TMax_MASK);

                if (nmIsMeSkipped(nmpdu.source, nm_OsekCB[canport].nmLocalID, nmpdu.destination))
                {
                    printk(PRINTK_LEVEL_LOG, "Local ID is skipped!!!");

                    if (nm_OsekCB[canport].NetworkStatus.bit.NMactive)
                    {

                        nm_OsekCB[canport].TxPDU.opCode.byte = 0;
                        nm_OsekCB[canport].TxPDU.opCode.bit.alive = 1;

                        if (nm_OsekCB[canport].NetworkStatus.bit.bussleep)
                        {
                            nm_OsekCB[canport].TxPDU.opCode.bit.sleepInd = 1;
                        }
                        else
                        {
                            nm_OsekCB[canport].TxPDU.opCode.bit.sleepInd = 0;
                        }

                        if (D_WindowDataReq(canport, &(nm_OsekCB[canport].TxPDU),
                                            8) == false) //���ͺ���//PS����ʹ����SLEEPIND��״̬Ҳû�иģ�
                        {
                            nm_OsekSetAlarm(canport, TTx_MASK);// �����ط���ʱ��
                        }
                        else
                        {
                            printk(PRINTK_LEVEL_LOG, "Send Alive message OK!!!");
                        }

                        nm_OsekCB[canport].nmTxCount++;
                    }
                }

            }
        }
    }
}


static void nmBusSleepMain(unsigned char canport)
{
	if(nm_OsekCB[canport].busSleep_ErrFlag == 1)
	{
		
		D_Start(canport);
		nm_OsekCB[canport].busSleep_ErrFlag = 0;
	//	printk(PRINTK_LEVEL_LOG, "CAN go into normal mode!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		return;
	}

	if (nm_OsekCB[canport].goToModeUsedFlag & GOTOMODE_AWAKE_MASK)
    {
        nm_OsekCB[canport].goToModeUsedFlag &= (~GOTOMODE_AWAKE_MASK);

        printk(PRINTK_LEVEL_LOG, "WakeUp by gotoMode _awake!!!");

        D_Start(canport);//    ʹCAN�ڴ�����������״̬,���Զ������ϵı��Ľ���ACKӦ��
        TalkNM(canport); //    �����������ͨѶ
        nmInit3(canport);
		nm_OsekCB[canport].awake_ByApp_Flag = 0;
		nm_OsekCB[canport].busSleep_ErrFlag = 0;
	//	kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_STATE);//

    }

    else if (nm_OsekCB[canport].receive_NM_Flag)
    {

        nm_OsekCB[canport].receive_NM_Flag = 0;//�����־
        printk(PRINTK_LEVEL_LOG, "WakeUp by Receive NM Message!!!");

     //   D_Start(canport);//    ʹCAN�ڴ�����������״̬,���Զ������ϵı��Ľ���ACKӦ��
   		if(nm_OsekCB[canport].NetworkStatus.bit.canModeNormal == 0)
	 	{
			D_Start(canport);//    ʹCAN�ڴ�����������״̬,���Զ������ϵı��Ľ���ACKӦ��
		}
        TalkNM(canport); //    �����������ͨѶ
        nmInit3(canport);
        nmNormalStandard(canport, nm_OsekCB[canport].RxPDU);

	//	kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_STATE);//
		// ��MPU֪ͨ ������BUSSLEEP״̬
    }

    else if (nm_OsekCB[canport].awake_ByApp_Flag == 1)
    {
        nm_OsekCB[canport].awake_ByApp_Flag = 0;
#if 1   // �������APP���Ļ�������
		if(nm_OsekCB[canport].NetworkStatus.bit.canModeNormal == 0)
	 	{
			 D_Start(canport);//    ʹCAN�ڴ�����������״̬,���Զ������ϵı��Ľ���ACKӦ��
		}
        printk(PRINTK_LEVEL_LOG, "WakeUp by APP message!!!");
        TalkNM(canport); //    �����������ͨѶ
        nmInit3(canport);
	//	kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_STATE);//��MPUͬ��NM״̬תΪ��BUSSLEEP״̬
#endif
        // ��MPU֪ͨ ������BUSSLEEP״̬
        
    }
	else
    {}

	if((nm_OsekCB[canport].currentState == NM_stBusSleep)&&(nm_OsekCB[canport].NetworkStatus.bit.canModeNormal != 0))
	{
		D_Stop(canport);
		
	}

}


/*NORMAL״̬�� �Ĵ�����*/
static void nmNormalMain(unsigned char canport)
{
    if (nm_OsekCB[canport].NM_TX_OKFLAG == 1) //NM���� ���ͳɹ�
    {
        nm_OsekCB[canport].NM_TX_OKFLAG = 0;//�����־λ
        nm_OsekCB[canport].nmTxCount = 0;//������ʹ��������

        if (nm_OsekCB[canport].TxPDU.opCode.bit.ring == 1) //������͵���RING����
        {
            nm_OsekCancleAlarm(canport, TTyp_MASK | TMax_MASK);
            nm_OsekSetAlarm(canport, TMax_MASK);

            if (nm_OsekCB[canport].TxPDU.opCode.bit.sleepInd == 1)
            {
                if (nm_OsekCB[canport].NetworkStatus.bit.bussleep == 1)
                {
                    nm_OsekCB[canport].TxPDU.opCode.bit.sleepAck = 1;
                    // �����Э��ջ�е�
                    nm_OsekCB[canport].NetworkStatus.bit.sleepAck = 1;// �����DSC�ӵ�һ����־

                    nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
                    nm_OsekCB[canport].currentState = NM_stNormalPrepSleep;
                    printk(PRINTK_LEVEL_LOG,
                           "State Change!!!Can %d change to NM_stNormalPrepSleep by Tx sleepInd OK!!!", canport);
                }
            }
        }
        else//������͵Ĳ���RING����
        {
            //������������
        }
    }

    if (nm_OsekCB[canport].receive_NM_Flag) //˵�����ջ������ﻹ�ж���ȡ�����ݣ������յ��µ����ݡ�
    {
        nm_OsekCB[canport].receive_NM_Flag = 0;

        nmNormalStandard(canport, nm_OsekCB[canport].RxPDU);

        if (nm_OsekCB[canport].RxPDU.opCode.bit.sleepAck == 1)
        {
            if (nm_OsekCB[canport].NetworkStatus.bit.bussleep == 1)
            {
                D_Offline(canport);
                nm_OsekSetAlarm(canport, TWBS_MASK);
                nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
                nm_OsekCB[canport].currentState = NM_stTwbsNormal;
                printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stTwbsNormal by Receive sleepAck !!!",
                       canport);
            }
        }
    }


    if (nm_OsekCB[canport].nmTimerOverFlow & TTyp_MASK) //TTyp��ʱ�����
    {
        nm_OsekCB[canport].nmTimerOverFlow &= (~TTyp_MASK);//��� ��־λ

        nm_OsekCancleAlarm(canport, TTyp_MASK | TMax_MASK);
        nm_OsekSetAlarm(canport, TMax_MASK);

        if (nm_OsekCB[canport].NetworkStatus.bit.NMactive)
        {
            nm_OsekCB[canport].TxPDU.opCode.byte = 0;
            nm_OsekCB[canport].TxPDU.opCode.bit.ring = 1;

            if (nm_OsekCB[canport].NetworkStatus.bit.bussleep
                && nm_OsekCB[canport].NetworkStatus.bit.presleepenable)
            {
                nm_OsekCB[canport].TxPDU.opCode.bit.sleepInd = 1;
            }
            else
            {
                nm_OsekCB[canport].TxPDU.opCode.bit.sleepInd = 0;
            }

            if (D_WindowDataReq(canport, &(nm_OsekCB[canport].TxPDU),
                                8) == false) //���ͺ���//PS����ʹ����SLEEPIND��״̬Ҳû�иģ�
            {
                nm_OsekSetAlarm(canport, TTx_MASK);// �����ط���ʱ��
            }
            else
            {
                printk(PRINTK_LEVEL_LOG, "Send Ring message OK!!!");
            }

            nm_OsekCB[canport].nmTxCount++;
        }

        if (nm_OsekCB[canport].nmTxCount > TX_ERROR_LIMTT)
        {
            nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
            nm_OsekCB[canport].currentState = NM_stLimphome;
            printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stLimphome by TX Count OverFlow !!!",
                   canport);
            nm_OsekSetAlarm(canport, TSleepReqMin_LimpHome_MASK);
            nm_OsekSetAlarm(canport, TError_MASK);
		//	kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//��MPUͬ�� NM limphome״̬
        }
        else
        {
            if (nm_OsekCB[canport].nmMerker.bit.stable)
            {
                nm_OsekCB[canport].NetworkStatus.bit.configurationstable = 1;
            }
            else
            {
                nm_OsekCB[canport].nmMerker.bit.stable = 1;
            }
        }

    }

    if (nm_OsekCB[canport].nmTimerOverFlow & TMax_MASK)
    {
        nm_OsekCB[canport].nmTimerOverFlow &= (~TMax_MASK);


        nm_OsekCancleAlarm(canport, TMax_MASK);
        nmInitReset6(canport, false);// �翧Ҫ�������� ״̬�£�RESET���ǲ����������߱�־λ��
    }

    if (nm_OsekCB[canport].goToModeUsedFlag & GOTOMODE_BUSSLEEP_MASK)
    {
        nm_OsekCB[canport].goToModeUsedFlag &= (~GOTOMODE_BUSSLEEP_MASK); //�����־λ

        if (nm_OsekCB[canport].NetworkStatus.bit.NMactive)
        {
            //״̬����
        }
        else
        {
            nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
            nm_OsekCB[canport].currentState = NM_stNormalPrepSleep;
            printk(PRINTK_LEVEL_LOG,
                   "State Change!!!Can %d change to NM_stNormalPrepSleep by go to mode busSleep !!!", canport);
        }
    }
}


/*NORMAL״̬�� �Ĵ�����*/
static void nmNormalPrepSleepMain(unsigned char canport)
{

    static unsigned int a = 0;

    a++;

    if (a >= 100)
    {
        printk(PRINTK_LEVEL_LOG, "go into normal prep sleep main OK!!!");
        a = 0;

    }

    if (nm_OsekCB[canport].goToModeUsedFlag & GOTOMODE_AWAKE_MASK)
    {
        nm_OsekCB[canport].goToModeUsedFlag &= (~GOTOMODE_AWAKE_MASK);//�����־λ
        nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
        nm_OsekCB[canport].currentState = NM_stNormal;
        printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stNormal !!!", canport);
    }

    if (nm_OsekCB[canport].nmTimerOverFlow & TTyp_MASK)
    {
        nm_OsekCB[canport].nmTimerOverFlow &= (~TTyp_MASK);

        if (nm_OsekCB[canport].NetworkStatus.bit.NMactive)
        {
            nm_OsekCB[canport].TxPDU.opCode.byte = 0;
            nm_OsekCB[canport].TxPDU.opCode.bit.ring = 1;
            nm_OsekCB[canport].TxPDU.opCode.bit.sleepInd = 1;
            nm_OsekCB[canport].TxPDU.opCode.bit.sleepAck = 1;//?????????????????????????????

            if (D_WindowDataReq(canport, &(nm_OsekCB[canport].TxPDU),
                                8) == false) //���ͺ���//PS����ʹ����SLEEPIND��״̬Ҳû�иģ�
            {
                nm_OsekSetAlarm(canport, TTx_MASK);// �����ط���ʱ��
            }
            else
            {
                printk(PRINTK_LEVEL_LOG, "Send Ring message OK!!!");
            }

            nm_OsekCB[canport].nmTxCount++;
        }
    }

    if (nm_OsekCB[canport].nmTxCount > TX_ERROR_LIMTT)
    {
        nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
        nm_OsekCB[canport].currentState = NM_stLimphome;
        printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stLimphome by TX Count OverFlow !!!",
               canport);
        nm_OsekSetAlarm(canport, TSleepReqMin_LimpHome_MASK);
        nm_OsekSetAlarm(canport, TError_MASK);
		//kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//��MPUͬ�� NM limphome״̬
    }


    if (nm_OsekCB[canport].NM_TX_OKFLAG) // ������� ���ͳɹ�
    {
        if (nm_OsekCB[canport].TxPDU.opCode.bit.ring)
        {
            D_Offline(canport);//ֹͣӦ�ñ���
            nm_OsekSetAlarm(canport, TWBS_MASK);
            nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
            nm_OsekCB[canport].currentState = NM_stTwbsNormal;
            printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stTwbsNormal !!!", canport);
        }
    }


    if (nm_OsekCB[canport].nmTimerOverFlow & TMax_MASK)
    {
        nm_OsekCB[canport].nmTimerOverFlow &= (~TMax_MASK);
        nm_OsekCancleAlarm(canport, TMax_MASK);
        nmInitReset6(canport, true);
    }

    if (nm_OsekCB[canport].receive_NM_Flag)
    {
        nm_OsekCB[canport].receive_NM_Flag = 0;

        nmNormalStandard(canport, nm_OsekCB[canport].RxPDU);

        if (nm_OsekCB[canport].RxPDU.opCode.bit.sleepAck)
        {
            D_Offline(canport);
            nm_OsekSetAlarm(canport, TWBS_MASK);
            nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
            nm_OsekCB[canport].currentState = NM_stTwbsNormal;
            printk(PRINTK_LEVEL_LOG, "Receive aleepAck Flag", canport);
            printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stTwbsNormal !!!", canport);
        }
        else if (nm_OsekCB[canport].RxPDU.opCode.bit.sleepInd == 0)
        {
            nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
            nm_OsekCB[canport].currentState = NM_stNormal;
            printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stNormal !!!", canport);
        }
        else
        {}
    }
}

static void nmNormalTwbsMain(unsigned char canport)
{
    if (nm_OsekCB[canport].nmTimerOverFlow & TWBS_MASK)
    {
        nm_OsekCB[canport].nmTimerOverFlow &= (~TWBS_MASK);
        nmBusSleep(canport);
    }

    if (nm_OsekCB[canport].receive_NM_Flag)
    {
        nm_OsekCB[canport].receive_NM_Flag = 0;

        if (nm_OsekCB[canport].RxPDU.opCode.bit.sleepInd == 0)
        {
            nm_OsekCancleAlarm(canport, TError_MASK);
            nmInitReset6(canport, true);
        }
        else
        {}
    }

    if (nm_OsekCB[canport].goToModeUsedFlag & GOTOMODE_AWAKE_MASK)
    {
        nm_OsekCB[canport].goToModeUsedFlag &= (~GOTOMODE_AWAKE_MASK);
        nm_OsekCancleAlarm(canport, TError_MASK);
        nmInitReset6(canport, true);
    }
}


static void  nmLimphomeMain(unsigned char canport)
{


	if (nm_OsekCB[canport].nmTimerOverFlow & TError_MASK)
    {
        nm_OsekCB[canport].nmTimerOverFlow &= (~TError_MASK);
        D_Online(canport);//  ��һ�������� ��������

       if (nm_OsekCB[canport].NetworkStatus.bit.bussleep
            && nm_OsekCB[canport].NetworkStatus.bit.presleepenable)
       {
            nm_OsekSetAlarm(canport, TMax_MASK);

            if (nm_OsekCB[canport].NetworkStatus.bit.NMactive)
            {
                nm_OsekCB[canport].TxPDU.opCode.byte = 0;
                nm_OsekCB[canport].TxPDU.opCode.bit.limphome = 1;
                nm_OsekCB[canport].TxPDU.opCode.bit.sleepInd = 1;

                if (D_WindowDataReq(canport, &(nm_OsekCB[canport].TxPDU),
                                    8) == false) //���ͺ���//PS����ʹ����SLEEPIND��״̬Ҳû�иģ�
                {
                    nm_OsekSetAlarm(canport, TTx_MASK);// �����ط���ʱ��
                }
                else
                {
                    printk(PRINTK_LEVEL_LOG, "Send limphome message OK!!!");
                }

                //  nm_OsekCB[canport].nmTxCount++;// �������ʹ������Ϊ�˽���LIMPHOME״̬������ ��LIMPHOME�£��Ͳ��ü�����
            }
            else
            {}
			D_Offline(canport);// 20190413//����LOMPHOMEPREPSLEEP״̬�£���������Ӧ�ñ��ġ�
            nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
            nm_OsekCB[canport].currentState = NM_stLimphomePrepSleep;
            printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stLimphomePrepSleep !!!", canport);
			return;
        }
        else
        {

            nm_OsekSetAlarm(canport, TError_MASK);

            if (nm_OsekCB[canport].NetworkStatus.bit.NMactive)
            {

				nm_OsekCB[canport].TxPDU.opCode.byte = 0;
                nm_OsekCB[canport].TxPDU.opCode.bit.limphome = 1;
                nm_OsekCB[canport].TxPDU.opCode.bit.sleepInd = 0;

                if (D_WindowDataReq(canport, &(nm_OsekCB[canport].TxPDU),
                                    8) == false) //���ͺ���//PS����ʹ����SLEEPIND��״̬Ҳû�иģ�
                {
                    nm_OsekSetAlarm(canport, TTx_MASK);// �����ط���ʱ��
                }
                else
                {
                    printk(PRINTK_LEVEL_LOG, "Send limphome message OK!!!");
                }

                //nm_OsekCB[canport].nmTxCount++;// �������ʹ������Ϊ�˽���LIMPHOME״̬������ ��LIMPHOME�£����ü�����
            }
            else
            {}
        }
    }

    if (nm_OsekCB[canport].NM_TX_OKFLAG)
    {
        if (nm_OsekCB[canport].TxPDU.opCode.bit.limphome)
        {
            nm_OsekCB[canport].nmMerker.bit.limphome = 1;
        }
    }

    if (nm_OsekCB[canport].receive_NM_Flag)
    {
        nm_OsekCB[canport].receive_NM_Flag = 0;

        nmNormalStandard(canport, nm_OsekCB[canport].RxPDU);

        if (nm_OsekCB[canport].NetworkStatus.bit.NMactive)
        {
            if (nm_OsekCB[canport].nmMerker.bit.limphome)
            {
                if (nm_OsekCB[canport].NetworkStatus.bit.bussleep)
                {
                    if (nm_OsekCB[canport].RxPDU.opCode.bit.sleepAck)
                    {
                        D_Offline(canport);
                        nm_OsekSetAlarm(canport, TWBS_MASK);
                        nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
                        nm_OsekCB[canport].currentState = NM_stTwbsLimphome;
                        printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stTwbsLimphome !!!", canport);
                    }
                    else
                    {
                        nmInitReset5(canport);
						kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//��MPUͬ�� NM limphome״̬
                    }
                }
                else
                {
                    nmInitReset5(canport);
					kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//��MPUͬ�� NM limphome״̬
                }
            }
            else
            {
                if (nm_OsekCB[canport].NetworkStatus.bit.bussleep)
                {
                    if (nm_OsekCB[canport].RxPDU.opCode.bit.sleepAck)
                    {
                        D_Offline(canport);
                        nm_OsekSetAlarm(canport, TWBS_MASK);
                        nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
                        nm_OsekCB[canport].currentState = NM_stTwbsLimphome;
                        printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stTwbsLimphome !!!", canport);
                    }
                    else
                    {
                        nmInitReset5(canport);
						kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//��MPUͬ�� NM limphome״̬
                    }
                }
                else
                {
                    nmInitReset5(canport);
					kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//��MPUͬ�� NM limphome״̬
                }
            }
        }

        else//�����������������д�����
        {}
    }
}

static void nmLimphomePrepSleepMain(unsigned char canport)
{
    if (nm_OsekCB[canport].goToModeUsedFlag & GOTOMODE_AWAKE_MASK)
    {
        nm_OsekCB[canport].goToModeUsedFlag &= (~GOTOMODE_AWAKE_MASK);
        nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
        nm_OsekCB[canport].currentState = NM_stLimphome;
        printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stLimphome !!!", canport);
    }

    if (nm_OsekCB[canport].receive_NM_Flag)
    {
        nm_OsekCB[canport].receive_NM_Flag = 0;

		if (nm_OsekCB[canport].RxPDU.opCode.bit.sleepAck == 1)
        {
            D_Offline(canport);
            nm_OsekSetAlarm(canport, TWBS_MASK);
			nm_OsekCancleAlarm(canport,TError_MASK);
            nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
            nm_OsekCB[canport].currentState = NM_stTwbsLimphome;
			printk(PRINTK_LEVEL_LOG, "Receive aleepAck Flag", canport);
            printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stTwbsLimphome1111111111111111111111111111111111 !!!", canport);
        }
		else if (nm_OsekCB[canport].RxPDU.opCode.bit.sleepInd == 0)
        {
            nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
            nm_OsekCB[canport].currentState = NM_stLimphome;
            printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stLimphome !!!", canport);
        }
		else
		{}

    }

    if (nm_OsekCB[canport].nmTimerOverFlow & TMax_MASK)
    {
        nm_OsekCB[canport].nmTimerOverFlow &= (~TMax_MASK);
        D_Offline(canport);
        nm_OsekSetAlarm(canport, TWBS_MASK);
        nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
        nm_OsekCB[canport].currentState = NM_stTwbsLimphome;
        printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stTwbsLimphome !!!", canport);
    }

}

static void nmTwbsLimphomeMain(unsigned char canport)
{
    if (nm_OsekCB[canport].goToModeUsedFlag & GOTOMODE_AWAKE_MASK)
    {
        nm_OsekCB[canport].goToModeUsedFlag &= (GOTOMODE_AWAKE_MASK);

        nm_OsekCancleAlarm(canport, TWBS_MASK);
        nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
        nm_OsekCB[canport].currentState = NM_stLimphome;
        printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stLimphome !!!", canport);
    }

    if (nm_OsekCB[canport].receive_NM_Flag)
    {
        nm_OsekCB[canport].receive_NM_Flag = 0;

		if ((nm_OsekCB[canport].RxPDU.opCode.bit.sleepInd == 0)&&(nm_OsekCB[canport].RxPDU.opCode.bit.sleepAck != 1))
        {
			nm_OsekCancleAlarm(canport, TWBS_MASK);
			nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
			nm_OsekCB[canport].currentState = NM_stLimphome;
			nm_OsekSetAlarm(canport,TError_MASK);
			printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stLimphome !!!", canport);

        }
        else
        {
            
        }
    }

    if (nm_OsekCB[canport].nmTimerOverFlow & TWBS_MASK)
    {
        nm_OsekCB[canport].nmTimerOverFlow &= (~TWBS_MASK);//�����־λ
      
		kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//��MPUͬ�� NM limphome״̬
		nmBusSleep(canport);
    }

}

/*��������Ҫ��������  NM��ʱ�����������NM״̬�ı�*/
void nm_OsekMainFunction(void)//
{
    unsigned char i;

    for (i = 0; i < 2; i++)
    {
        if (nm_OsekCB[i].nm_osekEN == 0)
        {
            continue;
        }
        else
        {
            /*����TTx��ʱ������*/
            if (nm_OsekCB[i].NetworkStatus.bit.NMactive) //����������ж� ��Ҳ�ɲ����ж�
            {
                if (nm_OsekCB[i].nmTimerOverFlow & TTx_MASK) //����ط���ʱ�����
                {
                    nm_OsekCB[i].nmTimerOverFlow &= (~TTx_MASK);//�����־λ
                    nm_OsekCancleAlarm(i, TTx_MASK); //ȡ���ط���ʱ��

                    if (D_WindowDataReq(i, &(nm_OsekCB[i].TxPDU), 8) == false) //���ͺ���
                    {
                        nm_OsekSetAlarm(i, TTx_MASK);// �����ط���ʱ��
                    }
                    else
                    {
                        //  printk(PRINTK_LEVEL_LOG, "Re-send CAN message OK!!!");
                    }

                    nm_OsekCB[i].nmTxCount++;
                    //
                }
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////

            switch (nm_OsekCB[i].currentState)
            {

                case NM_stBusSleep:
                    {
                        nmBusSleepMain(i);
                        break;
                    }

                case NM_stNormal:
                    {
                        nmNormalMain(i);
                        break;
                    }

                case NM_stNormalPrepSleep:
                    {
                        nmNormalPrepSleepMain(i);
                        break;
                    }

                case NM_stTwbsNormal:
                    {
                        nmNormalTwbsMain(i);
                        break;
                    }

                case NM_stLimphome:
                    {
                        nmLimphomeMain(i);
                        break;
                    }

                case NM_stLimphomePrepSleep:
                    {
                        nmLimphomePrepSleepMain(i);
                        break;
                    }

                case NM_stTwbsLimphome:
                    {
                        nmTwbsLimphomeMain(i);
                        break;
                    }

                default:
                    break;
            }

        }
    }
}

void NM_TxConformation(unsigned char canport, FXCAN_BUFF_TYPE mail_n)
{
    if ((nm_OsekCB[canport].nm_osekEN != 0) && (mail_n == TX_CAN_NM_OSEK))
    {
        nm_OsekCB[canport].nmTxCount = 0;// ��շ��ʹ������
        nm_OsekCB[canport].NM_TX_OKFLAG = 1;
        kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//
    }
}

/*���չ��˺���
ʵ���б�CAN���պ���������
PS���˺��������ݲ��ø���*/
void NM_RxIndication(unsigned char canport, CAN_MSG *msg)
{
    //  NMPDU_ST pdu;
    if (canport >= 2)
    {
        return;
    }

    if (msg->dlc > 8)
    {
        return;
    }

    if (msg->dlc < 2)
    {
        return;
    }

	nm_OsekCB[canport].awake_ByApp_Flag = 0;
	//msg->MsgID &= 0x7fffffff;     // ȥ �����λ�ı�־λ��

    if (((msg->MsgID &0x7fffffff) <= (nm_OsekCB[canport].nmBaseADDR + nm_OsekCB[canport].nmMaxID))
        && ((msg->MsgID &0x7fffffff) >= (nm_OsekCB[canport].nmBaseADDR + nm_OsekCB[canport].nmMinID)))
    {
       // nm_OsekCB[canport].awake_ByApp_Flag = 0;
        nm_OsekCB[canport].RxPDU.source = msg->MsgID & 0xFF;
        nm_OsekCB[canport].RxPDU.destination = msg->Data[0];
        nm_OsekCB[canport].RxPDU.opCode.byte = msg->Data[1];
        memset(nm_OsekCB[canport].RxPDU.ringData, 0, 6);
        memcpy(nm_OsekCB[canport].RxPDU.ringData, msg->Data + 2, msg->dlc - 2);

        nm_OsekCB[canport].receive_NM_Flag = 1;// �յ����������

        nm_OsekCB[canport].nmRxCount = 0;//���յ�NM���ģ���Ҫ��ս��մ��������

        kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//
        return;
    }

    else if (nm_OsekCB[canport].currentState == NM_stBusSleep) //��BUS SLEEP״̬���յ��ı��Ĳ�����������ġ�
    {
       // D_Stop(canport);
        nm_OsekCB[canport].awake_ByApp_Flag = 1;//Ҫ��Ӧ�ñ��Ļ���
        //   D_Start(canport); //  ��CAN�ж��в��ܲ���CAN�ڣ���������
        kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);
    }
    else
    {}

}


/*��������ʱ����⵽���ߴ���*/
void CanBus_ErrorCheck(unsigned char canport)
{
    nm_OsekCB[canport].busSleep_ErrFlag = 1;//Ҫ��Ӧ�ñ��Ļ���
    kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);
}

void GotoMode(unsigned char canport, NMMode_E newMode)
{
    if (canport > 2)
    {
        printk(PRINTK_LEVEL_ERROR, "CAN port %d >=2", canport);

        return;
    }

    switch (newMode)
    {
        case NM_BusSleep:
            {
                nm_OsekCB[canport].NetworkStatus.bit.bussleep = 1;//��ʾ����������������
                nm_OsekCB[canport].goToModeUsedFlag |= GOTOMODE_BUSSLEEP_MASK;//��ʾ�Ѿ�������GO TO BUSSLEEP
                nm_OsekCB[canport].goToModeUsedFlag &= (~GOTOMODE_AWAKE_MASK); //ȡ���Ѿ�������GO TO AWAKE
                kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);// ��GOTOMODE������������״̬�ı�
                break;
            }

        case NM_Awake:
            {
                nm_OsekCB[canport].NetworkStatus.bit.bussleep = 0;//��ʾ������������������
                nm_OsekCB[canport].goToModeUsedFlag |= GOTOMODE_AWAKE_MASK;
                nm_OsekCB[canport].goToModeUsedFlag &= (~GOTOMODE_BUSSLEEP_MASK);
                kernelSetTaskEvent(canBasic_taskid,
                                   TASK_SYS_EVENT_REQST1);// ��GOTOMODE������������״̬�ı�
                break;
            }
    }

    return;
}


void Start_NM_Osek(unsigned char canport)  //��������ĳ�ʼ�������ġ�
{
    memset(&nm_OsekCB[0].nm_osekEN, 0, sizeof(NMControlBlock_ST)*2); //������ƿ������е�����

	if(can_basic_cfg.nm_cfg[canport].nm_type == NM_TYPE_OSEK)
	{
		nm_OsekCB[canport].nm_osekEN = 1;
		nm_OsekCB[canport].NetworkStatus.bit.bussleep = 1;//Ĭ�������㱾������������
		printk(PRINTK_LEVEL_LOG, "CAN %d start Nm_OSEK", canport);
	}


 //   nm_OsekCB[canport].NetworkStatus.bit.bussleep = 1;//Ĭ�������㱾������������
    
    NM_Osek_Init(canport);//��ʼ��NM�������

    D_Init(canport);//Ӳ����ʼ������ǰCAN�ڴ��ڼ���״̬����������Ѿ���can_device_init()������

    nm_TimerInit();//ע������NM��ض�ʱ����ֻע�᲻����
    //ʹ����״̬����  BUSSLEEP״̬

    nm_OsekCB[canport].lastState = NM_stOff;
    nm_OsekCB[canport].currentState = NM_stBusSleep;// ���õ�ǰ״̬Ϊ BUSSLEEP״̬
    printk(PRINTK_LEVEL_LOG, "can %d state from NM_stOff to NM_stBusSleep !!!", canport);
}

/*��BUSOFF������£���NM��������״̬ǿ��ת��Ϊ LIMPHOME״̬*/
void gotoLimpHome_ByBusOff(unsigned char canport)
{
    if (nm_OsekCB[canport].currentState != NM_stLimphome)
    {
        nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
        nm_OsekCB[canport].currentState = NM_stLimphome;
        printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stLimphome by BUS OFF !!!", canport);
        nm_OsekSetAlarm(canport, TSleepReqMin_LimpHome_MASK);
        nm_OsekSetAlarm(canport, TError_MASK);
    }
}


#endif


