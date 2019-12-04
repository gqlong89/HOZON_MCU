





/*can 网络基础 配置文件*/




#ifndef __CAN_BASIC_CFG_H__
#define __CAN_BASIC_CFG_H__


typedef void(*busoff_revover_handle_cb)(unsigned char port);  // 定义BUSOFF恢复函数的回调函数类型。
typedef void(*busoff_state_report_MPU_cb)(unsigned char port);// busoff 状态向MPU汇报回调函数。


typedef void(* comm_vol_state_reportMPU_cb)(void);// 通讯电压状态向MPU汇报回调函数


#define  FUNC_USED         (1)
#define  FUNC_UNUSED       (0)


#define  USED_NM_AUTOSAR    FUNC_USED
#define  USED_NM_OSEK       FUNC_UNUSED


typedef enum
{
	NM_TYPE_NONE = 0,
	NM_TYPE_AUTOSAR,
	NM_TYPE_OSEK,	
}NM_TYPE_E;// 网络管理类型。

typedef struct
{
	unsigned int nma_base_id;  // 其它 a代表AUTOSAR
	unsigned int nma_locol_id;
	unsigned int nma_max_id;
	unsigned int nma_mix_id;
}NM_AUTOSAR_CFG_ST;

typedef struct
{
	unsigned int nmo_base_id;
	unsigned int nmo_locol_id;
	unsigned int nmo_max_id;
	unsigned int nmo_mix_id;	
}NM_OSEK_CFG_ST;




typedef struct
{
	
	NM_TYPE_E nm_type;//定义CAN端口网络管理类型。	

	NM_AUTOSAR_CFG_ST nm_autosar_cfg;

	NM_OSEK_CFG_ST    nm_osek_cfg;


}NM_CFG_ST;//网络管理配置结构体。



typedef struct
 {
 	unsigned char    report_to_MPU_EN;        // !0:要向MPU汇报BUSOFF状态。0：不需要汇报。
	unsigned int     fast_recover_cycle;   // 快恢复周期
	unsigned int     slow_recover_cycle;   // 慢恢复周期
	unsigned char    fast_recover_num;     // 快恢复次数
	busoff_revover_handle_cb busoff_recover_fun;     // 定义恢复函数的回调函数类型。
	busoff_state_report_MPU_cb busoff_report_fun;    //

}BUSOFF_CFG_ST;// BUSOFF 配置结构体

typedef struct
{
	unsigned char report_to_MPU_EN;            // !0: 要向MPU汇报通讯电压状态；0：不需要汇报。
	float         high_thd;                 //  通讯电压范围高域值
	float         low_thd;                  //  通讯电压范围低域值
	comm_vol_state_reportMPU_cb comm_vol_report_fun;
	
}COMM_VOL_CFG_ST;  // 通讯电压配置结构体

typedef struct
{	
	NM_CFG_ST nm_cfg[2];
	BUSOFF_CFG_ST busoff_cfg;
	COMM_VOL_CFG_ST comm_vol_cfg;	
}CAN_BASIC_CFG_ST;



void can_basic_cfg_Init(void);

#endif

