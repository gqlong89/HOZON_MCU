#ifndef __CUSTOM_H__
#define __CUSTOM_H__
#include <mid/kernel/task.h>
#include <can_api.h>
#include <app/scom/scom_tl.h>
typedef enum CUSTOM_EVENT
{
    CUSTOM_CTRL_EVT = TASK_SYS_EVENT_READ,
} CUSTOM_EVENT;

/* transport layer ack */
typedef enum CSTM_TL_CFM
{
    NORMAL   = 0xFD,
    ABNORMAL = 0xFE,
    BUSYING  = 0xFF,
} CSTM_TL_CFM;

typedef struct
{
	unsigned char port;
	unsigned char nm_state;
}NM_STA_REPORT_ST;  //��MPUͬ��NM BUSSLEEP״̬��

typedef struct
{
	unsigned char port;
	unsigned char busoff_state;
}BUSOFF_STA_REPORT_ST;  // ��MPUͬ��BUSOFF״̬ʱ���õĽṹ��


typedef struct
{
	unsigned char port;
	unsigned char limphome_state;
}LIMPHOME_STA_REPORT_ST;//��MPUͬ��NM LIMPHOME״̬��

typedef enum
{
    mes_app = 0,
    mes_NM,
    mes_all,
} MES_TYPE_E;


/*��λ����ʾ����CAN�������͵��շ�����*/
#define  APP_DIS   (0x01<<0)             //��0λ����ʾ��ֹӦ�ñ����շ�
#define  NM_DIS    (0x01<<1)             //��1λ����ʾ��ֹNM���ķ��շ�


extern void scom_Custom_Init(void);

void scom_tl_NM_bussleep_state_syn(void);
void scom_tl_can_busoff_state_syn(void);
void scom_tl_NM_limphome_state_syn(void);


#endif
