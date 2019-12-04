#include <app/can_mgr/can_hardware.h>
#include <app/can_mgr/can_mgr.h>
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
#include <app/can_mgr/can_broadCast.h>
#include <app/can_mgr/can_basic_cfg.h>
#include <app/custom/custom.h>
#include <app/nm_autosar/nm_autosar.h>



static CAN_PARAM_STR can_param[CAN_MAX_PORT];      
static MES_PARAM_STR mes_param[CAN_MES_NUM];       


CanDftBaudStr can_baud;             
CanDftBaudStr can_default_baud;      
extern unsigned char custom_can_ctrl_state;


#if USED_NM_OSEK
extern NMControlBlock_ST  nm_OsekCB[2];
#endif

#if USED_NM_AUTOSAR
extern CAN_PORT_STATE nm_canState[2];/*定义两个CAN网络状态变量*/
#endif

uint8_t mailrange[5][3] = {
    {TX_CAN_UDS, TX_CAN_BUFF_MAX, TX_CAN_GENERAL1 },
    {TX_CAN_UDS, TX_CAN_BUFF_MAX, TX_CAN_GENERAL1 },
    {TX_CAN_UDS, TX_CAN_BUFF_MAX, TX_CAN_GENERAL1 },
    {SPI_CAN_GENERAL0, SPI_CAN_BUFF_MAX, SPI_CAN_GENERAL0 },
    {SPI_CAN_GENERAL0, SPI_CAN_BUFF_MAX, SPI_CAN_GENERAL0 }
};

static void can_restore_dflt_baud(void)
{
    CanDftBaudStr cfg;
    unsigned int i, back = 0;

    memset( &cfg, 0, sizeof( cfg ) );
    memcpy( (void *) &can_default_baud, (void *) &cfg, sizeof( can_default_baud ) );
    if( cfg_restore_candftbaud( &cfg ) )
    {
        back = 1;
    }
    else
    {
        for( i = 0; i < CAN_MAX_PORT; i++ )
        {
            if( 0 > cfg.baud[i].can_normal_baud )
            {
                cfg.baud[i].can_normal_baud = can_default_baud.baud[i].can_normal_baud = 0;
                back = 1;
            }
            else
            {
                can_default_baud.baud[i].can_normal_baud = cfg.baud[i].can_normal_baud;
            }

            if( 0 > cfg.baud[i].canfd_baud )
            {
                cfg.baud[i].canfd_baud = can_default_baud.baud[i].canfd_baud = 0;
                back = 1;
            }
            else
            {
                can_default_baud.baud[i].canfd_baud = cfg.baud[i].canfd_baud;
                if(i == 0)                {
                    can_default_baud.baud[i].canfd_brs = cfg.baud[i].canfd_brs;
                    can_default_baud.baud[i].canfd_en = cfg.baud[i].canfd_en;
                }
            }
        }
    }

    if (back)
    {
        cfg_save_candftbaud(&cfg);
    }
}

static void can_restore_msg_send(void)
{
    CanMsgSendStr cfg;
    unsigned int i, back = 0;

    if (cfg_restore_canmsgsend(&cfg))
    {
        memset(&cfg, 0, sizeof(cfg));
    }
    else
    {
        for (i = 0; i < CAN_MES_NUM; i++)
        {
            if (CYCLE_FOREVER > cfg.info[i].cycles)
            {
                back = 1;
                memset(&cfg.info[i], 0, sizeof(MES_PARAM_STR));
            }
        }
    }

    memcpy(mes_param, cfg.info, sizeof(mes_param));

    if (back)
    {
        cfg_save_canmsgsend((unsigned char *)&cfg);
    }
}

static uint8_t can_save_msg_send(MES_PARAM_STR *mdata)
{
    uint8_t ret = 1, i = 0, j = 0, k = 0;
    CanMsgSendStr buf;

    if (!cfg_restore_canmsgsend(&buf))
    {
        for (i = 0; i < CAN_MES_NUM; i++)
        {
            if (buf.info[i].mes.MsgID == mdata->mes.MsgID
                && buf.info[i].port == mdata->port)                          
            {
                j = i;
                k = 1;
                break;
            }
            else if (0 == buf.info[i].cycles)
            {
                j = (k == 0) ? i : j;
                k = 1;
            }
        }

        if (1 == k)                                                                     {
            if (0 == mdata->cycles)
            {
                memset(&buf.info[j], 0, sizeof(MES_PARAM_STR));
            }
            else
            {
                memcpy(&buf.info[j], mdata, sizeof(MES_PARAM_STR));  
            }

            cfg_save_canmsgsend((unsigned char *) &buf);
            ret = 0;
        }
    }

    return ret;
}

int can_init( uint8_t canport, tCanBaud baud )
{
     if (CAN_PORT2 < canport)
    {
#if defined (SUPPORT_SPI_CAN)
        //spican_init( canport, baud.can_normal_baud );
        //can_baud.baud[canport] = baud;
#endif
    }
    //    else if (can_get_flexcan_phy_num() > canport)
    else if( CAN_PORT3 > canport )
    {
        if( 0 == flexcan_init( canport, baud, FLEXCAN_NORMAL_MODE))
        {
            can_baud.baud[canport] = baud;
            if(canport)
            {
                can_baud.baud[canport].canfd_en = 0;
                can_baud.baud[canport].canfd_brs = 0;
                can_baud.baud[canport].canfd_baud = 0;
            }
            return 0;
        }
        else
        {
            memset((unsigned char *)&can_baud.baud[canport], 0, sizeof(can_baud.baud[canport]));
            return -1;
        }
    }
    return 0;
}


void can_deinit(uint8_t canport)
{
    if (CAN_PORT2 < canport)
    {
        #if defined (SUPPORT_SPI_CAN)
        spican_deinit(canport);
        #endif
    }
    else if (CAN_PORT3 > canport)
    {
        if (0 == flexcan_deinit(canport))
        {
            //can_baud[canport] = 0;
        }
        else
        {
            printk(PRINTK_LEVEL_ERROR, "can deinit fail");
        }
    }

    return;
}

void can_deinit_all(void)
{
    for (int i = 0; i < CAN_MAX_PORT; i++)
    {
        can_deinit(i);
    }
}

static inline void can_param_init(void)
{
    memset( can_param, 0, sizeof( can_param ) );
    memset( (void *) &can_baud, 0, sizeof( can_baud ) );
    can_restore_dflt_baud();
    can_restore_msg_send();
//    can_read_auto_baud();
}

void can_mes_set(CAN_SEND_MSG *canMsg, uint8_t port, uint32_t cycles, uint32_t period)
{
    uint8_t i = 0, j = 0, k = 0;

    if (0 == cycles)                                  {
        cycles = CYCLE_FOREVER;
    }

    for (; i < CAN_MES_NUM; i++)
    {
        if (mes_param[i].cycles)
        {
            if (mes_param[i].mes.MsgID == canMsg->MsgID
                && mes_param[i].port == port)    
            {
                j = i;
                k = 1;
                break;
            }
        }
        else
        {
            j = (k == 0) ? i : j;
            k = 1;
        }
    }

    if (1 == k)                                                                  {

        mes_param[j].cur_val = 1000;

        mes_param[j].per_time = period;
        mes_param[j].cycles = cycles;
        mes_param[j].port = port;
        memcpy(&mes_param[j].mes, canMsg, sizeof(CAN_SEND_MSG));        
        can_save_msg_send(&mes_param[j]);
    }
}

void can_mes_stop(CAN_SEND_MSG *canMsg, uint8_t port)
{
    uint8_t i = 0;

    for (; i < CAN_MES_NUM; i++)
    {
        if ((mes_param[i].mes.MsgID == canMsg->MsgID) && (mes_param[i].port == port))
        {
            mes_param[i].cycles = 0;
            can_save_msg_send(&mes_param[i]);
            memset(&mes_param[i], 0, sizeof(mes_param[i]));
            break;
        }
    }
}

/*****************************************************************************
 *   Function   :    can_send
 *   Description:
 *   Inputs     :
 *                  canport : can port
 *                  can_mail: number of mail box
 *                  canMsg:   pointer to sending data
 *   Outputs    :   send result  0 - send complete
 *                               !0 - send failed
 *   Notes      :   when can_mail == MAIL_TRY (0xff) , traverse all the mailbox to send message
 ****************************************************************************/
uint8_t can_send(uint8_t canport, uint8_t can_mail, CAN_SEND_MSG *canMsg)
{
    uint8_t ret = 1;
    static unsigned char bak_mail = 0;

    if (check_busOffState(canport) == 1)
    {
		return (2);
    }

    if (get_Current_COMM_VolState() != COMM_VOL_NORMAL)
    {
		return (3);
    }

#if USED_NM_OSEK
   if((nm_OsekCB[canport].NetworkStatus.bit.NMAPPactive != 1)&&(can_mail != TX_CAN_NM_OSEK))
   {
		return (4);
   }
#endif

#if USED_NM_AUTOSAR
	if(canport == CAN_PORT1) 
	{
		if((nm_canState[canport].nm_currentState == NM_SLEEP_MODE_STATE)||(nm_canState[canport].nm_currentState == NM_PREPARE_SLEEP_MODE_SATATE))
		{
			if(can_mail != TX_CAN_NORMAL_NM_AUTOSAR)
			{
				return(5);
			}
		}
	}
#endif


	if((custom_can_ctrl_state & APP_DIS)&&(canport == 1))    // 挡APP报文。
	{
		if(can_mail > TX_CAN_NORMAL_NM_AUTOSAR)
		{
			return(6);
		}
	}

	if((custom_can_ctrl_state & NM_DIS)&&(canport == 1))  // 挡NM报文。
	{
		if(can_mail == TX_CAN_NORMAL_NM_AUTOSAR)
		{
			return(7);
		}
	}

    if (CAN_PORT2 < canport)
    {
        #if defined (SUPPORT_SPI_CAN)

        if (SPI_CAN_BUFF_MAX > can_mail)
        {
            if (!Get_send_sta(canport, can_mail, 0))
            {
                ret = spican_send(canport, (uint8_t *) canMsg, can_mail);       //block
            }
        }
        else if (MAIL_TRY == can_mail)
        {
            for (uint8_t i = SPI_CAN_GENERAL0; i < SPI_CAN_BUFF_MAX; i++)
            {
                if (!Get_send_sta(canport, i, 0))
                {
                    ret = spican_send(canport, (uint8_t *) canMsg, i);
                    break;
                }
            }
        }

        #endif
    }
    else
    {
        if (MAIL_TRY == can_mail)
        {
            uint8_t i;

			if((canport == CAN_PORT0)&&(can_default_baud.baud[canport].canfd_en == 1))
			{
				for (i = TX_CAN_GENERAL1; i <= TX_CAN_BUFF_MAX; i++)
            	{
               		if (bak_mail == i)
                	{
                    	continue;
                	}

                	INT_SYS_DisableIRQGlobal();

                	if (!flexcan_tx_mb_no_block(canport, (uint8_t *) canMsg, i))

                	{
                    	INT_SYS_EnableIRQGlobal();
                    	ret = 0;
                    	break;
                	}

                	INT_SYS_EnableIRQGlobal();
            	}
			}
			else
			{
				for (i = TX_CAN_NORMAL_GENERAL1; i <= TX_CAN_NORMAL_BUFF_MAX; i++)
            	{
               		if (bak_mail == i)
                	{
                    	continue;
                	}

                	INT_SYS_DisableIRQGlobal();

                	if (!flexcan_tx_mb_no_block(canport, (uint8_t *) canMsg, i))
				//	if (!flexcan_tx_mb_no_block(canport, (uint8_t *) canMsg, TX_CAN_NORMAL_GENERAL1))

                	{
                    	INT_SYS_EnableIRQGlobal();
                    	ret = 0;
                    	break;
                	}
                	INT_SYS_EnableIRQGlobal();
            	}
			}			
        }
        else if ((can_mail == TX_CAN_NM)||(can_mail == TX_CAN_UDS)||(can_mail == TX_CAN_NORMAL_UDS) || (can_mail == TX_CAN_NORMAL_NM_OSEK)||(can_mail == TX_CAN_NORMAL_NM_AUTOSAR))
        {
			INT_SYS_DisableIRQGlobal();
			if (flexcan_tx_mb_no_block(canport, (unsigned char *)canMsg, can_mail) == 0)
            {
                ret = 0;
            }
			INT_SYS_EnableIRQGlobal();
        }
		else{}

            bak_mail = can_mail;
    }

    return ret;
}


/*****************************************************************************
 *   Function   :    can_send_busoff
 *   Description:    ??劋绨珻AN BUS OFF ? *   Inputs     :
 *                  canport : can port
 *                  can_mail: number of mail box
 *                  canMsg:   pointer to sending data
 *   Outputs    :   send result  0 - send complete
 *                               !0 - send failed
 *   Notes      :   when can_mail == MAIL_TRY (0xff) , traverse all the mailbox to send message
 ****************************************************************************/
uint8_t can_send_busoff(uint8_t canport, uint8_t can_mail, CAN_SEND_MSG *canMsg)
{
    uint8_t ret = 1;
    static unsigned char bak_mail = 0;

    if (get_Current_COMM_VolState() != COMM_VOL_NORMAL)
    {
		return (3);
    }

#if USED_NM_OSEK 
   if((nm_OsekCB[canport].NetworkStatus.bit.NMAPPactive != 1)&&(can_mail != TX_CAN_NM_OSEK))
   {
		return (4);
   }
#endif
    if (CAN_PORT2 < canport)
    {
        #if defined (SUPPORT_SPI_CAN)

        if (SPI_CAN_BUFF_MAX > can_mail)
        {
            if (!Get_send_sta(canport, can_mail, 0))
            {
                ret = spican_send(canport, (uint8_t *) canMsg, can_mail);       //block
            }
        }
        else if (MAIL_TRY == can_mail)
        {
            for (uint8_t i = SPI_CAN_GENERAL0; i < SPI_CAN_BUFF_MAX; i++)
            {
                if (!Get_send_sta(canport, i, 0))
                {
                    ret = spican_send(canport, (uint8_t *) canMsg, i);
                    break;
                }
            }
        }

        #endif
    }
    else
    {
        if (MAIL_TRY == can_mail)
        {
            uint8_t i;

            for (i = TX_CAN_GENERAL1; i <= TX_CAN_BUFF_MAX; i++)
            {
                if (bak_mail == i)
                {
                    continue;
                }

                INT_SYS_DisableIRQGlobal();

                if (!flexcan_tx_mb_no_block(canport, (uint8_t *) canMsg, i))

                {
                    INT_SYS_EnableIRQGlobal();
                    ret = 0;
                    break;
                }

                INT_SYS_EnableIRQGlobal();
            }
        }
        else if ((can_mail == TX_CAN_UDS) || (can_mail == TX_CAN_NM))
        {
			INT_SYS_DisableIRQGlobal();
			if (flexcan_tx_mb_no_block(canport, (unsigned char *)canMsg, can_mail) == 0)
            {
                ret = 0;
            }
			INT_SYS_EnableIRQGlobal();
        }
		else
		{}
        bak_mail = can_mail;
    }
    return ret;
}

int can_receive(uint32_t canport, CAN_MSG *tCANMsg)
{
    if (0 == UDS_CANAccept(canport, tCANMsg))// 没有收到诊断报文
    {
      //  tCANMsg->dlc = tCANMsg->dlc | ((canport + 1) & 0xF) << 4;
        tCANMsg->port = canport + 1;

        if ((tCANMsg->MsgID < 0X400) || (tCANMsg->MsgID > 0x7ff)) //应用报文
        {
            if ((custom_can_ctrl_state & APP_DIS)&&(canport == 1))
            {
                printk(PRINTK_LEVEL_LOG, "app message NOT SEND TO mpu");//打印 丢掉应用报文
                return 0;
            }

            else
            {
                return 1;
            }
        }
        else if ((tCANMsg->MsgID >= 0X400) && (tCANMsg->MsgID <= 0x7ff)) // NM报文
        {
            if ((custom_can_ctrl_state & NM_DIS)&&(canport == 1))
            {
                printk(PRINTK_LEVEL_LOG, "NM message NOT SEND TO mpu");//打印 丢掉NM报文
                return 0;
            }

            else
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 0;
    }
}


void can_device_init(void)
{
  //  can_param_init();

    can_default_baud.baud[0].canfd_baud = 4000;
    can_default_baud.baud[0].can_normal_baud = 500;
    can_default_baud.baud[0].canfd_brs = 1;
    can_default_baud.baud[0].canfd_en = 1;

	can_default_baud.baud[1].can_normal_baud = 500;
    
    for (uint8_t i = 0; i < CAN_MAX_PORT; i++)
    {
        if( 0 < can_default_baud.baud[i].can_normal_baud )
        {
            if(can_init( i, can_default_baud.baud[i]) != 0)
            {
                memset((unsigned char *)&can_default_baud.baud[i], 0, sizeof(can_default_baud.baud[i]));
            }
        }
    }

    return;
}



void can_wakeup_init(void)
{
    unsigned char i;
    for (i = 0; i < CAN_MAX_PORT; i++)
    {
        if (can_default_baud.baud[i].can_normal_baud > 0 )
        {
             can_init(i,can_default_baud.baud[i]);
        }
    }
    set_BCParam_Manual();
}


void can_err_set(uint8_t port, uint8_t state)
{
    if (STATE_RECEIVE == state)
    {
        can_param[port].workstate = state;
    }
    else if (STATE_SEND == state)
    {
        can_param[port].workstate = state;  //added by Cindy
    }
    else if (STATE_Z != state)
    {
        can_param[port].workstate |= state;
        //        can_errtimer_start(port, state);
    }
    else
    {
        can_param[port].workstate = 0;
    }

    can_busOffMgr(port, state); //
}






void can_rectime_set(uint8_t port)
{
    can_param[port].rectime = driverGetUptime();
}

uint8_t can_sta_get(uint8_t port)
{
    if (0 == driverGetUptime())
    {
        return RET_INVALID;
    }

    if (driverGetUptime() < (CAN_NODATA_TIME + can_param[port].rectime))
    {
        return RET_HAVE_DATA;
    }
    else
    {
        return RET_NO_DATA;
    }
}

uint8_t can_state_lost(uint8_t port)
{
    if (0 == driverGetUptime())
    {
        return MPU_SYN_DATA_STATUS_UNKONW;
    }

    if (driverGetUptime() < (CAN_LOST_TIME + can_param[port].rectime))
    {
        return MPU_SYN_DATA_STATUS_OK;
    }
    else
    {
        return MPU_SYN_DATA_STATUS_ERROR;
    }
}

uint8_t can_state_busoff(uint8_t port)
{
    if (STATE_BUSOFF & can_param[port].workstate)
    {
        return MPU_SYN_DATA_STATUS_ERROR;
    }
    else
    {
        return MPU_SYN_DATA_STATUS_OK;
    }
}

uint8_t can_state_buserr(uint8_t port)
{
    if (STATE_ERR & can_param[port].workstate)
    {
        return MPU_SYN_DATA_STATUS_ERROR;
    }
    else
    {
        return MPU_SYN_DATA_STATUS_OK;
    }
}

void can_set_baud(uint8_t port, int16_t baud)
{
    if( can_baud.baud[port].can_normal_baud != baud )
    {
        can_baud.baud[port].can_normal_baud = baud;
        can_init (port, can_baud.baud[port]);
    }
}

void can_start_cantag(void)
{
    static int start_flag = 0;

    if (!start_flag)
    {
        SysTick_clr_tag();
        ctimer_start();
        start_flag = 1;
    }
}

