
#ifndef _CAN_BASIC_H_
#define _CAN_BASIC_H


typedef enum
{
	WP_TYPE_ON_HD = 0,      // on 线类型
	WP_TYPE_ON_VT,          // 虚拟ON线类型
	WP_TYPE_CHARGE_HD,      // 充电线  类型
	WP_TYPE_ACC_HD,         // ACC线类型
	WP_TYPE_ALL             // 所有类型 
}WAKEUP_TYPE_E;        // 唤醒条件枚举




#define StayAwake_Default                   (0)
#define StayAwake_KL15                   (1<<0)
#define StayAwake_Charge                 (1<<1)
#define StayAwake_Diagnosis              (1<<3)
#define StayAwake_ACC                    (1<<4)
#define StayAwake_Telematics_service     (1<<5)   // ON线不在，而远程服务在


#define Signal_ON       StayAwake_KL15
#define Signal_ACC      StayAwake_ACC
#define Signal_S_Charge StayAwake_Charge
#define Signal_F_Charge StayAwake_Charge   
#define Signal_ON_VT    StayAwake_Telematics_service


#define DONOT_CARE   (3)


typedef struct
{
	unsigned char  last_state;// 本地唤醒条件 上一个状态
	unsigned char  current_state; //本地唤醒条件 最新 状态。
}LOCAL_WAKEUPCOND_ST;   // 本地唤醒条件结构体


extern unsigned int canBasic_task(unsigned int mask);
void update_NM_Local_WakeupCondition(WAKEUP_TYPE_E wp_type,unsigned char state);
void local_Condition_trigger_NM(void);



#endif /* INCLUDE_UDS_H_ */



