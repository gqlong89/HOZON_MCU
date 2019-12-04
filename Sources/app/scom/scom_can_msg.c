/**
 * @Title: scom_can_msg.c
 * @author yuzhimin
 * @date Sep 4, 2017
 * @version V1.0
 */

#include <app/can_mgr/can_mgr.h>
#include <app/scom/scom_can_msg.h>
#include <can_api.h>
#include <config.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <scom_msg_def.h>
#include <string.h>
#include <sys/_stdint.h>

static void set_can_report(unsigned char *msg, unsigned int len)
{
    unsigned int i;
    unsigned char no = len / sizeof(CAN_REPORT_CFG);
    CAN_REPORT_CFG *cfg;
    CAN_SEND_MSG canMsg;

    for (i = 0; i < no; i++)
    {
        cfg = (CAN_REPORT_CFG *) &msg[i];
        taskPrintf(TASK_LEVEL_LOG, "rtype:0x%x,ftype:0x%x,port:%u,dlc:%u,id:0x%08x:val:%u,times:%u!",
                   cfg->rtype, cfg->ftype, cfg->port, cfg->dlc, cfg->id, cfg->val, cfg->times);

        canMsg.MsgID = cfg->id;
        canMsg.DLC = cfg->dlc;

        if (cfg->ftype == CAN_FRAME_EXTENDED)
        {
            canMsg.isEID = 1;
        }

        memcpy(&canMsg.Data, cfg->data, sizeof(canMsg.Data));

        if (0 == (cfg->rtype & CAN_REPORT_MASK))
        {
            can_mes_stop(&canMsg, cfg->port);
        }
        else
        {
            can_mes_set(&canMsg, cfg->port, cfg->times, cfg->val);
        }

        i += sizeof(CAN_PORT_CFG);
    }
}

static void scom_can_set_baud(unsigned char *body, unsigned int len)
{
    UNUSED(len);
    uint8_t i;
    int16_t *can_baud = (int16_t *) body ;

    for (i = 0; i < CAN_MAX_PORT; i++)
    {
        if (can_baud[i] > 0)
        {
            can_set_baud(i, can_baud[i]);
        }
        else if (can_baud[i] == 0)
        {
            can_deinit(i);
        }
    }
}

static void scom_can_set_default_baud(unsigned char *body, unsigned int len)
{
    UNUSED(len);
    CanDftBaudStr cfg;

  //  memcpy((void *)can_default_baud, (void *)body, sizeof(can_default_baud));
  //  memcpy((void *)&cfg, (void *)can_default_baud, sizeof(can_default_baud));

    cfg_save_candftbaud(&cfg);
}

void scom_can_baud_sync(void)
{
    scom_tl_send_frame(SCOM_TL_CMD_CAN_BAUD, SCOM_TL_SINGLE_FRAME, 0,
                       (uint8_t *)&can_baud, sizeof(can_baud));
}

void scom_can_default_baud_sync(void)
{
    scom_tl_send_frame(SCOM_TL_CMD_CAN_DEFAULT_BAUD, SCOM_TL_SINGLE_FRAME, 0,
                       (uint8_t *)&can_default_baud, sizeof(can_default_baud));
}

void scom_can_proc(SCOM_TL_MSG_HDR *hdr, unsigned char *body)
{
    switch (hdr->msg_type)
    {
        case SCOM_TL_CMD_CAN_SEND_CFG:
            set_can_report(body, hdr->flen);
            break;

        case SCOM_TL_CMD_CAN_BAUD:
            scom_can_set_baud(body, hdr->flen);
            break;

        case SCOM_TL_CMD_CAN_DEFAULT_BAUD:
            scom_can_set_default_baud(body, hdr->flen);
            break;

        default:
            taskPrintf(TASK_LEVEL_ERROR, "unknown can msgid=%u!", hdr->msg_type);
            break;
    }
}



