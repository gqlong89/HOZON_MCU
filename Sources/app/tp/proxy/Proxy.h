#ifndef PROXY_HEAD
#define PROXY_HEAD

#include <stdbool.h>
#include <sys/_stdint.h>

/****************************************************UDS******************************************************/

/********************uds msg head length*********************/
#define   UDS_DATA_HEAD_LENGTH          7   /*数据头预留 不小于 uds_data_msg_head_def*/

/***********IPC UDS 报文类型************/
/*mpu设置mcu传输层*/
#define MSG_ID_UDS_SET_S        0xF1
#define MSG_ID_UDS_SET_C        0xF2
/*mpu发送数据到mcu传输层*/
#define MSG_ID_UDS_REQ          0xF3
/*mcu接收数据上传mpu应用层*/
#define MSG_ID_UDS_IND          0xF4
/*mcu confirm*/
#define MSG_ID_UDS_CFM          0xF5
/*mpu传输刷写数据*/
#define MSG_ID_UDS_UPGRADE  0xF6
/*mcu 返回参数设置结果 */
#define MSG_ID_UDS_SET_ASK  0xF7
/*mcu 返回UDS Client参数设置结果 */
#define MSG_ID_UDS_SET_C_ASK  0xF8

/*******单帧报文SF，数据最大长度*******/
#define UDS_SF_PDU_DLC_MAX_         (7)

/*******uds 传输层应用层 错误码********/
#define ERR_OK      0
#define ERR_CANID   0x80000001
#define ERR_STEP    0x80000002
#define ERR_UNDEF       0x8000000F


#define TASK_UDS_UPLOAD         TASK_SYS_EVENT_REQST2

/***********************
*uds帧头
***********************/
#pragma pack(1)
/*设置参数*/
typedef struct
{
    uint8_t MsgID;
    bool result;
} uds_ask_set_head_def;

/*收发数据*/
typedef struct
{
    uint8_t MsgID;
    uint32_t CanID;
    uint16_t DLC;
} uds_data_msg_head_def;

/*传输层应答应用层*/
typedef struct
{
    uint8_t MsgID;
    uint32_t CanID;
    uint8_t Result;
} uds_ask_msg_head_def;
#pragma pack(0)
/***********************
*uds参数设置列表
***********************/
typedef struct
{
    uint8_t canport;
    uint8_t u8FillValue;            // 报文数据长度不足8字节时，填充值
    uint8_t u8FC_Stmin;         //通知连续帧发送方，发送间隔时间
    uint8_t u8FC_BS;                //告知连续帧发送方，接收方的连续帧接收能力
    uint16_t u16N_Bs;               //等待控制帧超时时间
    uint16_t u16N_Cr;               //等待连续帧超时时间
    uint32_t u32CANID_Phy;          //本地物理地址，设备被诊断时使用
    uint32_t u32CANID_Fun;          //本地功能地址，设备被诊断时使用
    uint32_t u32CANID_Respond;    //本地响应地址，设备被诊断时使用

} uds_set_def;



void proxy_upload_retry(void);
uint32_t proxy_download(uint8_t *pdata, uint32_t length);
void proxy_upload(uint8_t *pdata , uint16_t length, uint8_t type);



void uds_result_confirm(void *uds_para, uint32_t u32AI, uint8_t eNResult);
void uds_data_indication(void *uds_para, uint32_t u32AI, uint16_t u32PDU_DLC, uint8_t *pu8_PDUData,
                         uint8_t eNResult);

#endif


