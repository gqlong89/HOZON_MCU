
#ifndef _CAN_VOLTAGE_H_
#define _CAN_VOLTAGE_H_

#define  COMM_VOL_NORMAL   (0)     // 通讯电压正常
#define  COMM_VOL_LOW      (1)     // 通讯电压过低
#define  COMM_VOL_HIGH     (2)     // 通讯电压过高

#define  COMM_VOL_LOW_THRESHOLD    (6.3)   // 通讯电压范围低阈值
#define  COMM_VOL_HIGH_THRESHODL   (18)  // 通讯电压范围高阈值


typedef struct
{
    unsigned char currrentState;
} COMM_VOL_ST;  //通讯电压状态机结构体

void COMM_VolInit(void);
unsigned char get_Current_COMM_VolState(void);
void COMM_VoltageCheck(void);




#endif /* INCLUDE_UDS_H_ */



