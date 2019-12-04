#include <app/can_mgr/can_mgr.h>
#include <app/scom/scom_tl.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <mid/msg/msg_parse.h>
#include <scom_msg_def.h>
#include <string.h>

static int               scom_tl_status = SCOM_TL_STATE_DISCONNECTED;
static scom_tx_fun       scom_tl_tx_fun;
static scom_msg_proc_fun scom_tl_proc_fun;

/*******************************************************************
 function:     scom_tl_init
 description:  initiaze transport layer
 input:        none
 output:       none
 return:       none
 ********************************************************************/
void scom_tl_init(void)
{
    scom_tl_tx_fun   = NULL;
    scom_tl_proc_fun = NULL;
    return;
}

/*******************************************************************
 function:     scom_tl_reg_tx_fun
 description:  register sending function
 input:        scom_tx_fun tx_fun, send message function
 output:       none
 return:       none
 ********************************************************************/
void scom_tl_reg_tx_fun(scom_tx_fun tx_fun)
{
    scom_tl_tx_fun = tx_fun;
}

/*******************************************************************
 function:     scom_tl_reg_proc_fun
 description:  register application message process function
 input:        scom_msg_proc_fun proc_fun
 output:       none
 return:       none
 ********************************************************************/
void scom_tl_reg_proc_fun(scom_msg_proc_fun proc_fun)
{
    scom_tl_proc_fun = proc_fun;
}

/*******************************************************************
 function:     scom_tl_get_state
 description:  get transport layer state
 input:        none
 output:       none
 return:       the current state
 ********************************************************************/
int scom_tl_get_state(void)
{
    return scom_tl_status;
}

/*******************************************************************
 function:     scom_tl_send_frame
 description:  send one frame to peer
 input:        unsigned char type, message_type;
               unsigned char frame_type, frame type;
               unsigned short frame_no, frame no;
               unsigned char *data, message body;
               unsigned int len, message body length
 output:       none
 return:       0 indicates success;
               others indicate failed
 ********************************************************************/
int scom_tl_send_frame(unsigned char msg_type, unsigned char frame_type,
                       unsigned short frame_no,
                       unsigned char *data, unsigned int len)
{
    int ret;
    unsigned char cs = 0;
    unsigned int msg_len;
    SCOM_TL_MSG_HDR msg_hdr;
    static unsigned char omsg[1024];

    // taskPrintf(TASK_LEVEL_DEBUG, "send [%u,%u,%u]", msg_type, frame_type, len);

    msg_hdr.msg_type = msg_type;
    msg_hdr.ftype = frame_type;
    msg_hdr.fno = frame_no;
    msg_hdr.flen = len;

    ret = msg_encode((unsigned char *) &msg_hdr, sizeof(msg_hdr),
                     omsg, sizeof(omsg), FIRST_SEG, &cs);

    if (ret <= 0)
    {
        taskPrintf(TASK_LEVEL_ERROR, "msg_encode hdr failed");
        return -1;
    }

    //taskPrintf( TASK_LEVEL_LOG, "msg_encode1:%u", ret );

    msg_len = ret;
    ret = msg_encode(data, len, omsg + msg_len, sizeof(omsg) - msg_len,
                     LAST_SEG, &cs);

    if (ret <= 0)
    {
        taskPrintf(TASK_LEVEL_ERROR, "msg_encode failed");
        return -1;
    }
    else
    {
        //taskPrintf( TASK_LEVEL_LOG, "msg_encode2:%u", ret );
        msg_len += ret;
    }

    if (NULL != scom_tl_tx_fun)
    {
        if (0 == scom_tl_tx_fun(omsg, msg_len))
        {
            ret = 0;
        }
        else
        {
            taskPrintf(TASK_LEVEL_LOG, "send failed [%u,%u,%u]", msg_type, frame_type, len);
            ret = -2;
        }
    }

    return ret;
}

/*******************************************************************
 function:     scom_tl_send_con_req
 description:  send connect request to peer
 input:        none
 output:       none
 return:       none
 ********************************************************************/
void scom_tl_send_con_req(void)
{
    int ret;

    ipc_clean();

    ret = scom_tl_send_frame(SCOM_TL_CMD_CON_REQ, SCOM_TL_SINGLE_FRAME,
                             0, NULL, 0);

    if (ret != 0)
    {
        taskPrintf(TASK_LEVEL_ERROR, "send connect request failed, ret:0x%08x", ret);
    }

    scom_tl_status = SCOM_TL_STATE_CONNECTING;

    return;
}

/*******************************************************************
 function:     scom_tl_send_con_res
 description:  send connect response to peer
 input:        none
 output:       none
 return:       none
 ********************************************************************/
void scom_tl_send_con_res(void)
{
    int ret;

    ret = scom_tl_send_frame(SCOM_TL_CMD_CON_RES, SCOM_TL_SINGLE_FRAME, 0, NULL, 0);

    if (ret != 0)
    {
        taskPrintf(TASK_LEVEL_ERROR, "send connect response failed, ret:%d", ret);
    }

    return;
}

/*******************************************************************
 function:     scom_tl_proc_con_req
 description:  process connect request from peer
 input:        none
 output:       none
 return:       none
 ********************************************************************/
void scom_tl_proc_con_req(void)
{
    ipc_clean();
    scom_tl_send_con_res();

    if (scom_tl_status < SCOM_TL_STATE_IDLE)
    {
        scom_tl_status = SCOM_TL_STATE_IDLE;
        taskPrintf(TASK_LEVEL_ERROR, "spi is connected");
    }

    can_start_cantag();

    scom_set_mcu_upg_state(MCU_CHK_VER_STATE);
}

/*******************************************************************
 function:     scom_tl_proc_con_res
 description:  proc connect response from peer
 input:        none
 output:       none
 return:       none
 ********************************************************************/
void scom_tl_proc_con_res(void)
{
    ipc_clean();

    if (scom_tl_status < SCOM_TL_STATE_IDLE)
    {
        scom_tl_status = SCOM_TL_STATE_IDLE;
        taskPrintf(TASK_LEVEL_ERROR, "spi is connected");
    }

    can_start_cantag();

    scom_set_mcu_upg_state(MCU_CHK_VER_STATE);
}

/*******************************************************************
 function:     scom_tl_send_msg
 description:  send command to peers immediately
 input:        unsigned char msgid, message id;
 unsigned char *data, message body;
 unsigned int len,    message body length;
 output:       none
 return:       0 indicates success;
 others indicate failed
 ********************************************************************/
int scom_tl_send_msg(unsigned char msgid, unsigned char *data, unsigned int len)
{
    int ret;

    if (len > SCOM_TL_FRAME_LEN)
    {
        taskPrintf(TASK_LEVEL_ERROR, "too long msg");
        return -1;
    }

    ret = scom_tl_send_frame(msgid, SCOM_TL_SINGLE_FRAME, 0, data, len);

    if (ret != 0)
    {
        taskPrintf(TASK_LEVEL_ERROR, "send frame failed, ret:%d", ret);
    }

    return ret;
}

/*******************************************************************
 function:     scom_tl_msg_proc
 description:  send command to mcu
 input:        unsigned char *msg, message body;
 output:       unsigned int len, message length;
 return:       none
 ********************************************************************/
void scom_tl_msg_proc(unsigned char *msg, unsigned int len)
{
    SCOM_TL_MSG_HDR *tl_hdr = (SCOM_TL_MSG_HDR *) msg;

    switch (tl_hdr->msg_type)
    {
        case SCOM_TL_DATA_REQ:
            break;

        case SCOM_TL_DATA_OK_RES:
            #if 0 // mcu appimg updata by bootloader
            scom_tl_proc_data_ok_res(msg, len);
            #endif
            break;

        case SCOM_TL_DATA_NG_RES:
            #if 0 // mcu appimg updata by bootloader
            scom_tl_proc_data_ng_res(msg, len);
            #endif
            break;

        case SCOM_TL_CMD_CON_REQ:
            scom_tl_proc_con_req();
            break;

        case SCOM_TL_CMD_CON_RES:
            scom_tl_proc_con_res();
            break;

        default:
            if (NULL != scom_tl_proc_fun)
            {
                scom_tl_proc_fun(msg, len);
            }

            break;
    }
}

/*******************************************************************
 function:     scom_tl_timeout_proc
 description:  resend timer timeout message
 input:        none
 output:       none
 return:       none
 ********************************************************************/
void scom_tl_timeout_proc(void)
{
    if (SCOM_TL_STATE_DISCONNECTED == scom_tl_status
        || SCOM_TL_STATE_CONNECTING == scom_tl_status)
    {
        scom_tl_send_con_req();
        return;
    }
}


