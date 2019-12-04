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
}NM_STA_REPORT_ST;  //向MPU同步NM BUSSLEEP状态。

typedef struct
{
	unsigned char port;
	unsigned char busoff_state;
}BUSOFF_STA_REPORT_ST;  // 向MPU同步BUSOFF状态时所用的结构体


typedef struct
{
	unsigned char port;
	unsigned char limphome_state;
}LIMPHOME_STA_REPORT_ST;//向MPU同步NM LIMPHOME状态。

typedef enum
{
    mes_app = 0,
    mes_NM,
    mes_all,
} MES_TYPE_E;


/*按位来表示，对CAN报文类型的收发控制*/
#define  APP_DIS   (0x01<<0)             //第0位，表示禁止应用报文收发
#define  NM_DIS    (0x01<<1)             //第1位，表示禁止NM报文发收发


extern void scom_Custom_Init(void);

void scom_tl_NM_bussleep_state_syn(void);
void scom_tl_can_busoff_state_syn(void);
void scom_tl_NM_limphome_state_syn(void);


#endif
