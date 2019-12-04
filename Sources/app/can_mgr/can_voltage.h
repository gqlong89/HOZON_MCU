
#ifndef _CAN_VOLTAGE_H_
#define _CAN_VOLTAGE_H_

#define  COMM_VOL_NORMAL   (0)     // ͨѶ��ѹ����
#define  COMM_VOL_LOW      (1)     // ͨѶ��ѹ����
#define  COMM_VOL_HIGH     (2)     // ͨѶ��ѹ����

#define  COMM_VOL_LOW_THRESHOLD    (6.3)   // ͨѶ��ѹ��Χ����ֵ
#define  COMM_VOL_HIGH_THRESHODL   (18)  // ͨѶ��ѹ��Χ����ֵ


typedef struct
{
    unsigned char currrentState;
} COMM_VOL_ST;  //ͨѶ��ѹ״̬���ṹ��

void COMM_VolInit(void);
unsigned char get_Current_COMM_VolState(void);
void COMM_VoltageCheck(void);




#endif /* INCLUDE_UDS_H_ */



