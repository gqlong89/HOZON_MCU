#ifndef PROXY_HEAD
#define PROXY_HEAD

#include <stdbool.h>
#include <sys/_stdint.h>

/****************************************************UDS******************************************************/

/********************uds msg head length*********************/
#define   UDS_DATA_HEAD_LENGTH          7   /*����ͷԤ�� ��С�� uds_data_msg_head_def*/

/***********IPC UDS ��������************/
/*mpu����mcu�����*/
#define MSG_ID_UDS_SET_S        0xF1
#define MSG_ID_UDS_SET_C        0xF2
/*mpu�������ݵ�mcu�����*/
#define MSG_ID_UDS_REQ          0xF3
/*mcu���������ϴ�mpuӦ�ò�*/
#define MSG_ID_UDS_IND          0xF4
/*mcu confirm*/
#define MSG_ID_UDS_CFM          0xF5
/*mpu����ˢд����*/
#define MSG_ID_UDS_UPGRADE  0xF6
/*mcu ���ز������ý�� */
#define MSG_ID_UDS_SET_ASK  0xF7
/*mcu ����UDS Client�������ý�� */
#define MSG_ID_UDS_SET_C_ASK  0xF8

/*******��֡����SF��������󳤶�*******/
#define UDS_SF_PDU_DLC_MAX_         (7)

/*******uds �����Ӧ�ò� ������********/
#define ERR_OK      0
#define ERR_CANID   0x80000001
#define ERR_STEP    0x80000002
#define ERR_UNDEF       0x8000000F


#define TASK_UDS_UPLOAD         TASK_SYS_EVENT_REQST2

/***********************
*uds֡ͷ
***********************/
#pragma pack(1)
/*���ò���*/
typedef struct
{
    uint8_t MsgID;
    bool result;
} uds_ask_set_head_def;

/*�շ�����*/
typedef struct
{
    uint8_t MsgID;
    uint32_t CanID;
    uint16_t DLC;
} uds_data_msg_head_def;

/*�����Ӧ��Ӧ�ò�*/
typedef struct
{
    uint8_t MsgID;
    uint32_t CanID;
    uint8_t Result;
} uds_ask_msg_head_def;
#pragma pack(0)
/***********************
*uds���������б�
***********************/
typedef struct
{
    uint8_t canport;
    uint8_t u8FillValue;            // �������ݳ��Ȳ���8�ֽ�ʱ�����ֵ
    uint8_t u8FC_Stmin;         //֪ͨ����֡���ͷ������ͼ��ʱ��
    uint8_t u8FC_BS;                //��֪����֡���ͷ������շ�������֡��������
    uint16_t u16N_Bs;               //�ȴ�����֡��ʱʱ��
    uint16_t u16N_Cr;               //�ȴ�����֡��ʱʱ��
    uint32_t u32CANID_Phy;          //���������ַ���豸�����ʱʹ��
    uint32_t u32CANID_Fun;          //���ع��ܵ�ַ���豸�����ʱʹ��
    uint32_t u32CANID_Respond;    //������Ӧ��ַ���豸�����ʱʹ��

} uds_set_def;



void proxy_upload_retry(void);
uint32_t proxy_download(uint8_t *pdata, uint32_t length);
void proxy_upload(uint8_t *pdata , uint16_t length, uint8_t type);



void uds_result_confirm(void *uds_para, uint32_t u32AI, uint8_t eNResult);
void uds_data_indication(void *uds_para, uint32_t u32AI, uint16_t u32PDU_DLC, uint8_t *pu8_PDUData,
                         uint8_t eNResult);

#endif


