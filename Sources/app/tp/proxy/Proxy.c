#include <app/scom/scom_tl.h>
#include <app/tp/proxy/Proxy.h>
#include <app/tp/uds/UDS_Config.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <scom_msg_def.h>
#include <string.h>
#include <sys/_stdint.h>
#include <config.h>

extern uint8_t task_uds_id;
/***********************
*数据重发
***********************/
static struct
{
    uint16_t flag;
    uint16_t len;
    uint8_t type;
    uint8_t *data;

} data_retry = {0, 0, 0, 0};

/***传输层数据通过spi传输应用层***/
void proxy_upload(uint8_t *pdata , uint16_t length, uint8_t type)
{
    data_retry.data = (uint8_t *) pdata;
    data_retry.type = type;

    if (0 != data_retry.len)
    {
        data_retry.flag ++;
        printk(PRINTK_LEVEL_DEBUG, "data covered times :%d", data_retry.flag);
    }

    data_retry.len = length;

    if (0 == scom_tl_send_frame(type, SCOM_TL_SINGLE_FRAME, 0, data_retry.data, data_retry.len))
    {
        data_retry.data += data_retry.len;
        data_retry.len -= data_retry.len;
    }

    kernelSetTaskEvent(task_uds_id, TASK_UDS_UPLOAD);

}

/*****************uds传输层循环发送数据 *****************/
void proxy_upload_retry(void)
{
    if (0 != data_retry.len)
    {
        if (0 == scom_tl_send_frame(data_retry.type, SCOM_TL_SINGLE_FRAME, 0, data_retry.data,
                                    data_retry.len))
        {
            data_retry.data += data_retry.len;
            data_retry.len -= data_retry.len;
        }

        kernelSetTaskEvent(task_uds_id, TASK_UDS_UPLOAD);
    }
}

/*****************异常事件通知*****************/
void uds_result_confirm(void *uds_para, uint32_t u32AI, uint8_t eNResult)
{
    uds_para = uds_para ? (void *)0 : (void *)1;
    static uds_ask_msg_head_def uds_ask_msg_head;

    if ((N_DRIVER_ERR != eNResult) && (N_OK != eNResult) && (N_ERROR != eNResult)) /*响应发送*/
    {
        return;
    }

    uds_ask_msg_head.CanID = u32AI;
    uds_ask_msg_head.MsgID = MSG_ID_UDS_CFM;
    uds_ask_msg_head.Result = eNResult;
    proxy_upload((uint8_t *)(&uds_ask_msg_head), sizeof(uds_ask_msg_head), SCOM_TL_CMD_UDS_MSG);
}

/*****************接收数据上传*****************/
void uds_data_indication(void *uds_para, uint32_t u32AI, uint16_t u32PDU_DLC, uint8_t *pu8_PDUData,
                         uint8_t eNResult)//u32PDU_DLC与mpu的字节不对应，需要与mpu调整
{
    uds_data_msg_head_def *uds_msg_head = (uds_data_msg_head_def *)(((uds_t *) uds_para)->PDU_MF
                                          - sizeof(uds_data_msg_head_def));
    eNResult -= eNResult;

    if (((uds_t *) uds_para)->PDU_MF != pu8_PDUData)
    {
        memcpy(((uds_t *) uds_para)->PDU_MF, pu8_PDUData, u32PDU_DLC);
    }

    uds_msg_head->CanID = u32AI;
    uds_msg_head->MsgID = MSG_ID_UDS_IND;
    uds_msg_head->DLC = u32PDU_DLC;
    proxy_upload((uint8_t *) uds_msg_head , sizeof(uds_data_msg_head_def) + u32PDU_DLC,
                 SCOM_TL_CMD_UDS_MSG);
}

/**********************************************
*函数功能 : 应用层设置传输层参数
*描述 : 统一回复接收正常
*输入 : 空
*输出 : 空
**********************************************/
static void uds_param_set_ask(void)
{
    static uds_ask_set_head_def uds_ask_set_head;
    uds_ask_set_head.MsgID = MSG_ID_UDS_SET_ASK;
    uds_ask_set_head.result = true;
    proxy_upload((uint8_t *)(&uds_ask_set_head), sizeof(uds_ask_set_head), SCOM_TL_CMD_UDS_MSG);
}

/**********************************************
*函数功能 : 应用层设置传输层参数
*描述 : 统一回复接收正常
*输入 : 空
*输出 : 空
**********************************************/
static void uds_param_set_client_ask(void)
{
    static uds_ask_set_head_def uds_ask_set_head;
    uds_ask_set_head.MsgID = MSG_ID_UDS_SET_C_ASK;
    uds_ask_set_head.result = true;
    proxy_upload((uint8_t *)(&uds_ask_set_head), sizeof(uds_ask_set_head), SCOM_TL_CMD_UDS_MSG);
}

/**********************************************
*函数功能 : 应用层设置传输层参数
*输入 : uds - - 待设置对象
*       uds_set - - 应用层下发设置参数
*输出 : 空
**********************************************/
static uint32_t uds_param_set(uds_t *uds, uint8_t *uds_set)
{
    uds_set_def *uds_tmp = (uds_set_def *)((uint8_t *)uds_set + sizeof(uint8_t));
    uds->canport = uds_tmp->canport;
    uds->u8FillValue = uds_tmp->u8FillValue;
    uds->u8FC_STmin = uds_tmp->u8FC_Stmin;
    uds->u8FC_BS = uds_tmp->u8FC_BS;
    uds->u16N_Bs = uds_tmp->u16N_Bs;
    uds->u32Phy_CANID = uds_tmp->u32CANID_Phy;
    uds->u32Fun_CANID = uds_tmp->u32CANID_Fun;
    uds->u32Respond_CANID = uds_tmp->u32CANID_Respond;
    uds->u16N_Cr = uds_tmp->u16N_Cr;
    uds->u16N_Bs = uds_tmp->u16N_Bs;
    L_Data_Init(uds);
    UDS_CAN_Init(uds);
    uds_param_set_ask();
    return ERR_OK;
}

/**************************uds param set******************************/
static uint32_t uds_data_request(uds_data_msg_head_def *uds_msg)
{
    uds_t *tmp_uds = 0;
    uint32_t ret = 1;
    uint8_t *data = (uint8_t *)uds_msg + sizeof(uds_data_msg_head_def);

    if (pt_ServerUDS->u32Respond_CANID == uds_msg->CanID)
    {
        tmp_uds = pt_ServerUDS;
    }
    else if((pt_ClientUDS->u32Phy_CANID == uds_msg->CanID) ||
            (pt_ClientUDS->u32Fun_CANID == uds_msg->CanID))
    {
        tmp_uds = pt_ClientUDS;
    }
    else
    {
        return ERR_CANID;
    }

    tmp_uds->LData_PDU_DLC = uds_msg->DLC;

    for (uint32_t i = 0; i < uds_msg->DLC; i++)
    {
        tmp_uds->PDU_MF[i] = *(data + i);
    }

    if ((tmp_uds->LData_eWorkStatus == NL_IDLE_STATUS)
        || (tmp_uds->LData_eWorkStatus == NL_SEGMENT_READ_STATUS))
    {
        //tmp_uds->u32ReqCANID = tmp_uds->u32Phy_CANID;
        tmp_uds->u32ReqCANID = uds_msg->CanID;
        //#if UDSRUNMODE
        if(tmp_uds->canport == CAN_PORT0)
        {
            if(tmp_uds->LData_PDU_DLC <= 62)        
                ret = SF_Request__(tmp_uds, tmp_uds->u32ReqCANID, tmp_uds->LData_PDU_DLC, tmp_uds->PDU_MF); 
            else
                ret = FF_Request__(tmp_uds, tmp_uds->u32ReqCANID, tmp_uds->LData_PDU_DLC, tmp_uds->PDU_MF);
 
        //#else      
        }
        else
        {
            if (tmp_uds->LData_PDU_DLC <= UDS_SF_PDU_DLC_MAX_)
            {
                //ret = SF_Request__(tmp_uds, tmp_uds->u32Phy_CANID, tmp_uds->LData_PDU_DLC, tmp_uds->PDU_MF);
                ret = SF_Request__(tmp_uds, tmp_uds->u32ReqCANID, tmp_uds->LData_PDU_DLC, tmp_uds->PDU_MF);
            }
            else
            {
                //ret = FF_Request__(tmp_uds, tmp_uds->u32Phy_CANID, tmp_uds->LData_PDU_DLC, tmp_uds->PDU_MF);
                ret = FF_Request__(tmp_uds, tmp_uds->u32ReqCANID, tmp_uds->LData_PDU_DLC, tmp_uds->PDU_MF);
            }
        //#endif
        }
 
    }

    if (0 == ret)
    {
        if (tmp_uds->LData_PDU_DLC > UDS_SF_PDU_DLC_MAX_)
        {
            //uds_result_confirm(tmp_uds, tmp_uds->u32Phy_CANID, N_OK);
            //uds_result_confirm(tmp_uds, tmp_uds->u32ReqCANID, N_OK);
        }
    }
    else
    {
        //uds_result_confirm(tmp_uds, tmp_uds->u32Phy_CANID, N_DRIVER_ERR);
        uds_result_confirm(tmp_uds, tmp_uds->u32ReqCANID, N_DRIVER_ERR);
    }

    return ERR_OK;

}

uint32_t proxy_download(uint8_t *pdata, uint32_t length)
{
    SCOM_TL_MSG_HDR *scom_tl_msg = (SCOM_TL_MSG_HDR *) pdata;
    uint8_t *msg_head = pdata + sizeof(SCOM_TL_MSG_HDR);
    uint32_t ret = ERR_UNDEF;

    length -= length;

    switch (scom_tl_msg->msg_type)
    {
        case SCOM_TL_CMD_UDS_MSG:
            if (MSG_ID_UDS_SET_S == * msg_head)
            {
                cfg_save_uds(msg_head + 1);
                ret = uds_param_set(pt_ServerUDS, msg_head);
                uds_param_set_ask();
            }
            else if (MSG_ID_UDS_SET_C == * msg_head)
            {
                ret = uds_param_set(pt_ClientUDS, msg_head);
                uds_param_set_client_ask();
            }
            else if (MSG_ID_UDS_REQ == * msg_head)
            {
                ret = uds_data_request((uds_data_msg_head_def *)msg_head);
            }
            else if (MSG_ID_UDS_UPGRADE == * msg_head)
            {

            }

            break;

        default:
            break;
    }

    return ret;
}

#if 0
/******************************
*uds测试 ， 模拟应用层发送数据
*
*****************************/
#include <ring_buffer.h>
extern ring_buffer  ipcspi_rxring;

extern uint8_t scom_taskid;
int msg_encode(uint8_t *imsg, uint32_t ilen, uint8_t *omsg, uint32_t olen,
               uint8_t flag,
               uint8_t *base_cs);
static int scom_tl_send_frame_test(uint8_t msg_type, uint8_t frame_type,
                                   uint16_t frame_no,
                                   uint8_t *data, uint32_t len)
{
    int ret;
    uint8_t cs = 0;
    uint32_t msg_len;
    SCOM_TL_MSG_HDR msg_hdr;
    static uint8_t omsg[512];

    msg_hdr.msg_type = msg_type;
    msg_hdr.ftype = frame_type;
    msg_hdr.fno = frame_no;
    msg_hdr.flen = len;

    ret = msg_encode((uint8_t *) &msg_hdr, sizeof(msg_hdr),
                     omsg, sizeof(omsg), 0, &cs);

    if (ret <= 0)
    {
        taskPrintf(TASK_LEVEL_ERROR, "msg_encode hdr failed");
        return -1;
    }

    msg_len = ret;
    ret = msg_encode(data, len, omsg + msg_len, sizeof(omsg) - msg_len,
                     2, &cs);

    if (ret <= 0)
    {
        taskPrintf(TASK_LEVEL_ERROR, "msg_encode failed");
        return -1;
    }
    else
    {
        msg_len += ret;
    }

    if (rb_unused_len(&ipcspi_rxring) < msg_len)
    {
        printk(PRINTK_LEVEL_ERROR, "ipc tx queue overflow.");
        return 1;
    }

    rb_put(&ipcspi_rxring, omsg, msg_len);

    return 0;
}

extern unsigned char scom_taskid;
/*
 * uds 功能测试
 * 通过SCOM，CAN卡模拟应用层向传输层发送数据
 */
void uds_test(uint8_t *tmp , uint8_t len, uint8_t start)
{
    static uint8_t buff[512];
    static uds_data_msg_head_def msg_head = {MSG_ID_UDS_REQ, 0x07f9, 0};
    static uint32_t length = sizeof(uds_data_msg_head_def);

    memcpy(buff + length, tmp, len);
    length += len;
    msg_head.DLC += len;

    if (1 == start)
    {
        memcpy(buff, &msg_head, sizeof(uds_data_msg_head_def));
        scom_tl_send_frame_test(SCOM_TL_CMD_UDS_MSG, 0, 0, buff, length);
        kernelSetTaskEvent(scom_taskid, TASK_SYS_EVENT_READ);
        length = sizeof(uds_data_msg_head_def);
        msg_head.DLC = 0;
    }
}

/*
 * j1939 功能测试
 * 通过SCOM，CAN卡模拟应用层向传输层发送数据
 */
void j1939_test(uint8_t *tmp , uint8_t len, uint8_t start)
{
    static uint8_t buff[512];
    static j1939_data_msg_head_def msg_head = {MSG_ID_J1939_REQ, 0, 0xEA0102, 0};
    static uint32_t length = sizeof(j1939_data_msg_head_def);

    memcpy(buff + length, tmp, len);
    length += len;
    msg_head.DLC += len;

    if (1 == start)
    {
        memcpy(buff, &msg_head, sizeof(j1939_data_msg_head_def));
        scom_tl_send_frame_test(SCOM_TL_CMD_J1939_DATA, 0, 0, buff, length);
        kernelSetTaskEvent(scom_taskid, TASK_SYS_EVENT_READ);
        length = sizeof(j1939_data_msg_head_def);
        msg_head.DLC = 0;
    }
}
#endif
