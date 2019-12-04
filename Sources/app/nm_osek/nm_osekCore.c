

/*
*NM osek core 文件
*作者  DSC


****/

/*网络管理参数初始化
  需要初始化的项为：
  1:NM ID基地址
  2:本节点地址
  3:各定时器时长(单位MS)
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

NMControlBlock_ST  nm_OsekCB[2];//定义两个nm控制块变量

extern CAN_BASIC_CFG_ST can_basic_cfg;  // 外部定义的CAN基本 配置 变量 。

extern unsigned char canBasic_taskid;    

extern unsigned char scom_taskid;

void InitDirectNMparms(unsigned char canport) // 初始化直接网络管理参数
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
    nm_OsekCB[canport].TAppActive = TAppActive_Value;// 应用报文 首帧定时器

    nm_OsekCB[canport].nmBaseADDR = can_basic_cfg.nm_cfg[canport].nm_osek_cfg.nmo_base_id;
    nm_OsekCB[canport].nmMaxID = can_basic_cfg.nm_cfg[canport].nm_osek_cfg.nmo_max_id;
    nm_OsekCB[canport].nmMinID = can_basic_cfg.nm_cfg[canport].nm_osek_cfg.nmo_mix_id;
    nm_OsekCB[canport].nmLocalID = can_basic_cfg.nm_cfg[canport].nm_osek_cfg.nmo_locol_id;
}

void TalkNM(unsigned char canport) // 允许NM通讯
{
    if (canport >= 2)
    {
        printk(PRINTK_LEVEL_ERROR, "CAN port %d >=2", canport);
        return;
    }

    nm_OsekCB[canport].NetworkStatus.bit.NMactive = 1;//允许NM通讯
}

void SilentNM(unsigned char canport)  // 禁止NM通讯
{
    if (canport >= 2)
    {
        printk(PRINTK_LEVEL_ERROR, "CAN port %d >=2", canport);
        return;
    }

    nm_OsekCB[canport].NetworkStatus.bit.NMactive = 0;//禁止NM通讯
}

void D_Online(unsigned char canport) // 允许应通讯
{
    if (canport >= 2)
    {
        printk(PRINTK_LEVEL_ERROR, "CAN port %d >=2", canport);
        return;
    }

    nm_OsekCB[canport].NetworkStatus.bit.NMAPPactive = 1;   // 允许应用通讯
}

void D_Offline(unsigned char
               canport)  // 禁止应用通讯// 调用此函数后，就会进行TWBS状态，就不可以的发送应用报文 了
{
    if (canport >= 2)
    {
        printk(PRINTK_LEVEL_ERROR, "CAN port %d >=2", canport);
        return;
    }

    nm_OsekCB[canport].NetworkStatus.bit.NMAPPactive = 0;   // 禁止应用通讯
    stop_BC_timerByChl(canport);// 增加 停止应用报文 发送定时器
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
    FLEXCAN_HAL_SetOperationMode(base, FLEXCAN_NORMAL_MODE); //使CAN口处于正常工作状态
}

void D_Stop(unsigned char canport)
{
    CAN_Type *base = g_flexcanBase[canport];
    nm_OsekCB[canport].NetworkStatus.bit.canModeNormal = 0;
    FLEXCAN_HAL_SetOperationMode(base, FLEXCAN_LISTEN_ONLY_MODE); //使CAN口处监听状态
    flexcan_clear_errstatus(canport);
    flexcan_set_errint(canport, true);                           //设置允许CAN错误中断
}


void D_Init(unsigned char canport)
{
    nm_OsekCB[canport].NetworkStatus.bit.canModeNormal = 0;
    //can_init(canport, can_default_baud[canport]);//初始化CAN硬件接口
   // D_Stop(canport);// 且使之处于监听状态
   // scom_Custom_Init();
}



/*数据打包及发送函数
    返回   ture  ,发送成功，
    返回 false, 发送失败

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

    if (can_send(canport, TX_CAN_NM_OSEK, &msg) == 0) // 注意 ，所有的网络管理报文 使用特定的邮箱进行发送。
    {
        return true;   // 发送成功
    }

    return false;  //发送失败
}



/**/
void  nmInitReset6(unsigned char    canport, bool sleepInd) //reset from normal
{
    D_Online(canport);//所有RESET后，都允许发送应用函数

    nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
    nm_OsekCB[canport].currentState = NM_stInitReset;
    printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stInitReset !!!", canport);
	

    memset(nm_OsekCB[canport].normalList, 0, 32); //清空正常状态节点列表。
    nmAddtoPresent(canport, nm_OsekCB[canport].nmLocalID);//将本节点加入到正常节点列表中，协议栈要求

    nm_OsekCancleAlarm(canport, TTyp_MASK);
    nm_OsekCancleAlarm(canport, TMax_MASK);
    nm_OsekCancleAlarm(canport, TError_MASK);
    nm_OsekCancleAlarm(canport, TTx_MASK);
    nm_OsekCancleAlarm(canport, TWBS_MASK);
    nm_OsekCancleAlarm(canport, TSleepReqMin_LimpHome_MASK);  // 这个地方要改一下,可以搞成合在一起停止
    nm_OsekCancleAlarm(canport, TSleepReqMin_Normal_MASK);  //暂停所有定时器工作

    nm_OsekCB[canport].nmRxCount++;   // 发送错误计数器加1.

    /*下面要做的事情是发送Alive报文*/

	if ((nm_OsekCB[canport].lastState == NM_stBusSleep)
            || (nm_OsekCB[canport].lastState == NM_stTwbsLimphome)
            || (nm_OsekCB[canport].lastState == NM_stTwbsNormal))
    {
         nm_OsekSetAlarm(canport, TappActive_MASK); //开启首帧应用报文定时器

    }
    /*因为后面一定会发ALIVE，故，先把发送次数加了，为了开启尽快Ttyp定时器*/
	if (nm_OsekCB[canport].NetworkStatus.bit.NMactive != 0) //表示已经调用过TaklNM
	{
		nm_OsekCB[canport].nmTxCount++;
	}
    /*要发发送函数之前开启相应定时器， 否则会导致Ttyp不准 */
	if ((nm_OsekCB[canport].nmTxCount <= TX_ERROR_LIMTT)
        && (nm_OsekCB[canport].nmRxCount <= RX_ERROR_LIMIT))
    {
        nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
        nm_OsekCB[canport].currentState = NM_stNormal;
        printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stNormal !!!", canport);
        nm_OsekSetAlarm(canport, TTyp_MASK);

        nm_OsekSetAlarm(canport, TSleepReqMin_Normal_MASK);
        nm_OsekCB[canport].NetworkStatus.bit.presleepenable = 0;//  没允许休眠
    }
    else  //  表示发送或接收错误超过阈值，要进入LIMPHOME状态
    {
        nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
        nm_OsekCB[canport].currentState = NM_stLimphome;
        printk(PRINTK_LEVEL_LOG, "State Change!!!Can %d change to NM_stLimphome !!!", canport);

        nm_OsekSetAlarm(canport, TError_MASK);
        //开启LIMPHOME  错误定时计时器
        nm_OsekSetAlarm(canport, TSleepReqMin_LimpHome_MASK);
        nm_OsekCB[canport].NetworkStatus.bit.presleepenable = 0;
	//	kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//向MPU同步 NM limphome状态
    }
	
	/*配置Alive报文*/

    if (nm_OsekCB[canport].NetworkStatus.bit.NMactive != 0) //表示已经调用过TaklNM
    {
        nm_OsekCB[canport].logical_successor =
        nm_OsekCB[canport].nmLocalID;// 在没有入环的时候 ，逻辑继承都就是节点本身

        nm_OsekCB[canport].NetworkStatus.bit.sleepAck = 0;//不允许第一条RING报文  就发送SLEEPACK

        nm_OsekCB[canport].TxPDU.opCode.byte = 0;//清除操作码
        nm_OsekCB[canport].TxPDU.opCode.bit.alive = 1;

        if (nm_OsekCB[canport].NetworkStatus.bit.bussleep && sleepInd)
        {
            nm_OsekCB[canport].TxPDU.opCode.bit.sleepInd = 1;
        }
        else
        {
            nm_OsekCB[canport].TxPDU.opCode.bit.sleepInd = 0;
        }

        if (D_WindowDataReq(canport, &(nm_OsekCB[canport].TxPDU), 8) == false) //发送函数
        {
            nm_OsekSetAlarm(canport, TTx_MASK);// 启动重发定时器
        }
        else
        {
            printk(PRINTK_LEVEL_LOG, "Send Alive message OK!!!");
        }

        if ((nm_OsekCB[canport].lastState == NM_stBusSleep)
            || (nm_OsekCB[canport].lastState == NM_stTwbsLimphome)
            || (nm_OsekCB[canport].lastState == NM_stTwbsNormal))
        {
            nm_OsekSetAlarm(canport, TappActive_MASK); //开启首帧应用报文定时器

        }
    }
}


/*用于从limpHome状态  转向RESET状态时，所调用的RESET函数
  在这个情况下，没有清空 LIMPHOME表
*/
void nmInitReset5(unsigned char canport)// reset from limpHome
{

    nm_OsekCancleAlarm(canport, TError_MASK);//取消ERROR定时器
    nm_OsekCB[canport].nmMerker.bit.limphome = 0;
    nm_OsekCB[canport].nmTxCount = 0;
    nm_OsekCB[canport].nmRxCount = 0;
    nmInitReset6(canport, true);

}


void nmInit3(unsigned char canport)//reset form bussleep
{
    memset(nm_OsekCB[canport].limphomeList, 0, 32); //清空LIMPHOME 节点列表
    nm_OsekCB[canport].nmTxCount = 0;
    nm_OsekCB[canport].nmRxCount = 0;
    nmInitReset6(canport, false);//上电阶段，ALIVE报文不能发SLEED。IND
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

    nm_OsekCB[canport].nmTimerOverFlow = 0;//清空定时器标志
    nm_OsekCB[canport].receive_NM_Flag = 0;// 清空接收标志

    nm_OsekCancleAlarm(canport, TTx_MASK | TTyp_MASK | TMax_MASK | TError_MASK |
                       TSleepReqMin_LimpHome_MASK | TSleepReqMin_Normal_MASK | TWBS_MASK); //停止所有的定时器
    //向MPU通知转向BUSSLEEP
	kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_STATE);//
}


/*判断逻辑继承者
在接收到NM报文时被调用
S：接收的报文的源地址
R：本地地址
L：逻辑继承者
返回值 ：为最新的逻辑继承者*/
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


/*判断自己是否被滑过
在接收到NM报文时被调用
S：收到的NM报文的源地址
R：本地地址
D：收到的NM报文的目的地址
返回值 ：
     false:没有被滑过
     true: 被滑过*/
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


/*将某个节点从某个状态列表中移除*/
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
    }                                          //用一位表示一个节点，不错的想法
}


/*将节点加入到某个状态列表中*/
void nmAddToConfig(unsigned char canport, ConfigKindName_E configKind, unsigned char nodeId)
{
    unsigned char *config = NULL;

    switch (configKind)
    {
        case NM_ckNormal:
            config = nm_OsekCB[canport].normalList;//将节点加入 到NORMALL列表中
            break;

        case NM_ckLimphome:
            config = nm_OsekCB[canport].limphomeList;//将节点加入到LIMPHOME列表中
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


/*将节点加入到正常状态列表中*/
void nmAddtoPresent(unsigned char canport, unsigned char nodeId)
{
    nmAddToConfig(canport, NM_ckNormal , nodeId);
    nmRemoveFromConfig(canport, NM_ckLimphome, nodeId);
}

/*将节点加入到LIMPHOME列表中*/
void nmAddtoLimphome(unsigned char canport, unsigned char nodeId)
{
    nmAddToConfig(canport, NM_ckLimphome, nodeId);
    //是不是要从正常列表中移除，要讨论一下，不过目前没用到这一块。
}

/*此函数是在NORMAL状态下，处理接收到的NM报文标准函数，协议栈中的函数*/
static void nmNormalStandard(unsigned char canport, NMPDU_ST nmpdu)
{
    nm_OsekCB[canport].nmRxCount = 0;//接收到NM报文，就要清空接收错误计数器

    if (nmpdu.opCode.bit.limphome)
    {
        nmAddtoLimphome(canport, nmpdu.source);
    }
    else
    {
        nmAddtoPresent(canport, nmpdu.source);

        nm_OsekCB[canport].logical_successor = nmDetermineLS(nmpdu.source, nm_OsekCB[canport].nmLocalID,
                                               nm_OsekCB[canport].logical_successor);//判断逻辑继承者

        if (nmpdu.opCode.bit.alive)
        {
            nm_OsekCB[canport].nmMerker.bit.stable = 0;//表示环不稳定
            nm_OsekCB[canport].NetworkStatus.bit.configurationstable = 0;// 配置不稳定
            printk(PRINTK_LEVEL_LOG, "Receive Alive message!!!");
        }

        if (nmpdu.opCode.bit.ring) //表示是RING报文
        {
            nm_OsekCancleAlarm(canport, TTyp_MASK | TMax_MASK);

        //    if ((nmpdu.source == nmpdu.destination)
        //        || (nmpdu.destination == nm_OsekCB[canport].nmLocalID)) //如果是发给我的报文
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
                                            8) == false) //发送函数//PS：即使用了SLEEPIND其状态也没有改，
                        {
                            nm_OsekSetAlarm(canport, TTx_MASK);// 启动重发定时器
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

        D_Start(canport);//    使CAN口处于正常工作状态,可以对总线上的报文进行ACK应答
        TalkNM(canport); //    允许网络管理通讯
        nmInit3(canport);
		nm_OsekCB[canport].awake_ByApp_Flag = 0;
		nm_OsekCB[canport].busSleep_ErrFlag = 0;
	//	kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_STATE);//

    }

    else if (nm_OsekCB[canport].receive_NM_Flag)
    {

        nm_OsekCB[canport].receive_NM_Flag = 0;//清零标志
        printk(PRINTK_LEVEL_LOG, "WakeUp by Receive NM Message!!!");

     //   D_Start(canport);//    使CAN口处于正常工作状态,可以对总线上的报文进行ACK应答
   		if(nm_OsekCB[canport].NetworkStatus.bit.canModeNormal == 0)
	 	{
			D_Start(canport);//    使CAN口处于正常工作状态,可以对总线上的报文进行ACK应答
		}
        TalkNM(canport); //    允许网络管理通讯
        nmInit3(canport);
        nmNormalStandard(canport, nm_OsekCB[canport].RxPDU);

	//	kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_STATE);//
		// 向MPU通知 ，脱离BUSSLEEP状态
    }

    else if (nm_OsekCB[canport].awake_ByApp_Flag == 1)
    {
        nm_OsekCB[canport].awake_ByApp_Flag = 0;
#if 1   // 如果允许APP报文唤醒网络
		if(nm_OsekCB[canport].NetworkStatus.bit.canModeNormal == 0)
	 	{
			 D_Start(canport);//    使CAN口处于正常工作状态,可以对总线上的报文进行ACK应答
		}
        printk(PRINTK_LEVEL_LOG, "WakeUp by APP message!!!");
        TalkNM(canport); //    允许网络管理通讯
        nmInit3(canport);
	//	kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_STATE);//向MPU同步NM状态转为非BUSSLEEP状态
#endif
        // 向MPU通知 ，脱离BUSSLEEP状态
        
    }
	else
    {}

	if((nm_OsekCB[canport].currentState == NM_stBusSleep)&&(nm_OsekCB[canport].NetworkStatus.bit.canModeNormal != 0))
	{
		D_Stop(canport);
		
	}

}


/*NORMAL状态下 的处理函数*/
static void nmNormalMain(unsigned char canport)
{
    if (nm_OsekCB[canport].NM_TX_OKFLAG == 1) //NM报文 发送成功
    {
        nm_OsekCB[canport].NM_TX_OKFLAG = 0;//清除标志位
        nm_OsekCB[canport].nmTxCount = 0;//清除发送错误计数器

        if (nm_OsekCB[canport].TxPDU.opCode.bit.ring == 1) //如果发送的是RING报文
        {
            nm_OsekCancleAlarm(canport, TTyp_MASK | TMax_MASK);
            nm_OsekSetAlarm(canport, TMax_MASK);

            if (nm_OsekCB[canport].TxPDU.opCode.bit.sleepInd == 1)
            {
                if (nm_OsekCB[canport].NetworkStatus.bit.bussleep == 1)
                {
                    nm_OsekCB[canport].TxPDU.opCode.bit.sleepAck = 1;
                    // 这个是协议栈中的
                    nm_OsekCB[canport].NetworkStatus.bit.sleepAck = 1;// 这个是DSC加的一个标志

                    nm_OsekCB[canport].lastState = nm_OsekCB[canport].currentState;
                    nm_OsekCB[canport].currentState = NM_stNormalPrepSleep;
                    printk(PRINTK_LEVEL_LOG,
                           "State Change!!!Can %d change to NM_stNormalPrepSleep by Tx sleepInd OK!!!", canport);
                }
            }
        }
        else//如果发送的不是RING报文
        {
            //则不做任务事情
        }
    }

    if (nm_OsekCB[canport].receive_NM_Flag) //说明接收缓冲区里还有读读取的数据，即接收到新的数据。
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


    if (nm_OsekCB[canport].nmTimerOverFlow & TTyp_MASK) //TTyp定时器溢出
    {
        nm_OsekCB[canport].nmTimerOverFlow &= (~TTyp_MASK);//清除 标志位

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
                                8) == false) //发送函数//PS：即使用了SLEEPIND其状态也没有改，
            {
                nm_OsekSetAlarm(canport, TTx_MASK);// 启动重发定时器
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
		//	kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//向MPU同步 NM limphome状态
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
        nmInitReset6(canport, false);// 电咖要求，在正常 状态下，RESET后是不允许发送休眠标志位的
    }

    if (nm_OsekCB[canport].goToModeUsedFlag & GOTOMODE_BUSSLEEP_MASK)
    {
        nm_OsekCB[canport].goToModeUsedFlag &= (~GOTOMODE_BUSSLEEP_MASK); //清除标志位

        if (nm_OsekCB[canport].NetworkStatus.bit.NMactive)
        {
            //状态不变
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


/*NORMAL状态下 的处理函数*/
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
        nm_OsekCB[canport].goToModeUsedFlag &= (~GOTOMODE_AWAKE_MASK);//清除标志位
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
                                8) == false) //发送函数//PS：即使用了SLEEPIND其状态也没有改，
            {
                nm_OsekSetAlarm(canport, TTx_MASK);// 启动重发定时器
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
		//kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//向MPU同步 NM limphome状态
    }


    if (nm_OsekCB[canport].NM_TX_OKFLAG) // 如果报文 发送成功
    {
        if (nm_OsekCB[canport].TxPDU.opCode.bit.ring)
        {
            D_Offline(canport);//停止应用报文
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
        D_Online(canport);//  这一步有用吗 ？？？？

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
                                    8) == false) //发送函数//PS：即使用了SLEEPIND其状态也没有改，
                {
                    nm_OsekSetAlarm(canport, TTx_MASK);// 启动重发定时器
                }
                else
                {
                    printk(PRINTK_LEVEL_LOG, "Send limphome message OK!!!");
                }

                //  nm_OsekCB[canport].nmTxCount++;// 计数发送错误就是为了进行LIMPHOME状态，所以 在LIMPHOME下，就不用计数。
            }
            else
            {}
			D_Offline(canport);// 20190413//即在LOMPHOMEPREPSLEEP状态下，不允许发送应用报文。
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
                                    8) == false) //发送函数//PS：即使用了SLEEPIND其状态也没有改，
                {
                    nm_OsekSetAlarm(canport, TTx_MASK);// 启动重发定时器
                }
                else
                {
                    printk(PRINTK_LEVEL_LOG, "Send limphome message OK!!!");
                }

                //nm_OsekCB[canport].nmTxCount++;// 计数发送错误就是为了进行LIMPHOME状态，所以 在LIMPHOME下，就用计数。
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
						kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//向MPU同步 NM limphome状态
                    }
                }
                else
                {
                    nmInitReset5(canport);
					kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//向MPU同步 NM limphome状态
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
						kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//向MPU同步 NM limphome状态
                    }
                }
                else
                {
                    nmInitReset5(canport);
					kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//向MPU同步 NM limphome状态
                }
            }
        }

        else//不会出现这个情况，不写了这里。
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
        nm_OsekCB[canport].nmTimerOverFlow &= (~TWBS_MASK);//清除标志位
      
		kernelSetTaskEvent(scom_taskid, SPI_TLS_NM_LIMPHOME_STATE);//向MPU同步 NM limphome状态
		nmBusSleep(canport);
    }

}

/*本函数主要处理由于  NM定时器溢出触发的NM状态改变*/
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
            /*处理TTx超时的问题*/
            if (nm_OsekCB[i].NetworkStatus.bit.NMactive) //这个条件可判断 ，也可不用判断
            {
                if (nm_OsekCB[i].nmTimerOverFlow & TTx_MASK) //如果重发定时器溢出
                {
                    nm_OsekCB[i].nmTimerOverFlow &= (~TTx_MASK);//清除标志位
                    nm_OsekCancleAlarm(i, TTx_MASK); //取消重发定时器

                    if (D_WindowDataReq(i, &(nm_OsekCB[i].TxPDU), 8) == false) //发送函数
                    {
                        nm_OsekSetAlarm(i, TTx_MASK);// 启动重发定时器
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
        nm_OsekCB[canport].nmTxCount = 0;// 清空发送错误次数
        nm_OsekCB[canport].NM_TX_OKFLAG = 1;
        kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//
    }
}

/*接收过滤函数
实际中被CAN接收函数所调用
PS：此函数中内容不用更改*/
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
	//msg->MsgID &= 0x7fffffff;     // 去 掉最高位的标志位。

    if (((msg->MsgID &0x7fffffff) <= (nm_OsekCB[canport].nmBaseADDR + nm_OsekCB[canport].nmMaxID))
        && ((msg->MsgID &0x7fffffff) >= (nm_OsekCB[canport].nmBaseADDR + nm_OsekCB[canport].nmMinID)))
    {
       // nm_OsekCB[canport].awake_ByApp_Flag = 0;
        nm_OsekCB[canport].RxPDU.source = msg->MsgID & 0xFF;
        nm_OsekCB[canport].RxPDU.destination = msg->Data[0];
        nm_OsekCB[canport].RxPDU.opCode.byte = msg->Data[1];
        memset(nm_OsekCB[canport].RxPDU.ringData, 0, 6);
        memcpy(nm_OsekCB[canport].RxPDU.ringData, msg->Data + 2, msg->dlc - 2);

        nm_OsekCB[canport].receive_NM_Flag = 1;// 收到网络管理报文

        nm_OsekCB[canport].nmRxCount = 0;//接收到NM报文，就要清空接收错误计数器

        kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);//
        return;
    }

    else if (nm_OsekCB[canport].currentState == NM_stBusSleep) //在BUS SLEEP状态下收到的报文不是网络管理报文。
    {
       // D_Stop(canport);
        nm_OsekCB[canport].awake_ByApp_Flag = 1;//要被应用报文唤醒
        //   D_Start(canport); //  在CAN中断中不能操作CAN口，会死机。
        kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);
    }
    else
    {}

}


/*总线休眠时，检测到总线错误*/
void CanBus_ErrorCheck(unsigned char canport)
{
    nm_OsekCB[canport].busSleep_ErrFlag = 1;//要被应用报文唤醒
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
                nm_OsekCB[canport].NetworkStatus.bit.bussleep = 1;//表示本地休眠条件成立
                nm_OsekCB[canport].goToModeUsedFlag |= GOTOMODE_BUSSLEEP_MASK;//表示已经调用于GO TO BUSSLEEP
                nm_OsekCB[canport].goToModeUsedFlag &= (~GOTOMODE_AWAKE_MASK); //取消已经调用了GO TO AWAKE
                kernelSetTaskEvent(canBasic_taskid, TASK_SYS_EVENT_REQST1);// 由GOTOMODE触发任务，引起状态改变
                break;
            }

        case NM_Awake:
            {
                nm_OsekCB[canport].NetworkStatus.bit.bussleep = 0;//表示本地休眠条件不成立
                nm_OsekCB[canport].goToModeUsedFlag |= GOTOMODE_AWAKE_MASK;
                nm_OsekCB[canport].goToModeUsedFlag &= (~GOTOMODE_BUSSLEEP_MASK);
                kernelSetTaskEvent(canBasic_taskid,
                                   TASK_SYS_EVENT_REQST1);// 由GOTOMODE触发任务，引起状态改变
                break;
            }
    }

    return;
}


void Start_NM_Osek(unsigned char canport)  //是在任务的初始化里做的。
{
    memset(&nm_OsekCB[0].nm_osekEN, 0, sizeof(NMControlBlock_ST)*2); //清零控制块中所有的数据

	if(can_basic_cfg.nm_cfg[canport].nm_type == NM_TYPE_OSEK)
	{
		nm_OsekCB[canport].nm_osekEN = 1;
		nm_OsekCB[canport].NetworkStatus.bit.bussleep = 1;//默认是满足本地休眠条件的
		printk(PRINTK_LEVEL_LOG, "CAN %d start Nm_OSEK", canport);
	}


 //   nm_OsekCB[canport].NetworkStatus.bit.bussleep = 1;//默认是满足本地休眠条件的
    
    NM_Osek_Init(canport);//初始化NM网络参数

    D_Init(canport);//硬件初始化，当前CAN口处于监听状态。这个事情已经在can_device_init()做过了

    nm_TimerInit();//注册所有NM相关定时器，只注册不开启
    //使网络状态处于  BUSSLEEP状态

    nm_OsekCB[canport].lastState = NM_stOff;
    nm_OsekCB[canport].currentState = NM_stBusSleep;// 设置当前状态为 BUSSLEEP状态
    printk(PRINTK_LEVEL_LOG, "can %d state from NM_stOff to NM_stBusSleep !!!", canport);
}

/*在BUSOFF的情况下，将NM网络管理的状态强制转换为 LIMPHOME状态*/
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


