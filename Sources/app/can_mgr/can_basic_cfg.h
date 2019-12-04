





/*can ������� �����ļ�*/




#ifndef __CAN_BASIC_CFG_H__
#define __CAN_BASIC_CFG_H__


typedef void(*busoff_revover_handle_cb)(unsigned char port);  // ����BUSOFF�ָ������Ļص��������͡�
typedef void(*busoff_state_report_MPU_cb)(unsigned char port);// busoff ״̬��MPU�㱨�ص�������


typedef void(* comm_vol_state_reportMPU_cb)(void);// ͨѶ��ѹ״̬��MPU�㱨�ص�����


#define  FUNC_USED         (1)
#define  FUNC_UNUSED       (0)


#define  USED_NM_AUTOSAR    FUNC_USED
#define  USED_NM_OSEK       FUNC_UNUSED


typedef enum
{
	NM_TYPE_NONE = 0,
	NM_TYPE_AUTOSAR,
	NM_TYPE_OSEK,	
}NM_TYPE_E;// ����������͡�

typedef struct
{
	unsigned int nma_base_id;  // ���� a����AUTOSAR
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
	
	NM_TYPE_E nm_type;//����CAN�˿�����������͡�	

	NM_AUTOSAR_CFG_ST nm_autosar_cfg;

	NM_OSEK_CFG_ST    nm_osek_cfg;


}NM_CFG_ST;//����������ýṹ�塣



typedef struct
 {
 	unsigned char    report_to_MPU_EN;        // !0:Ҫ��MPU�㱨BUSOFF״̬��0������Ҫ�㱨��
	unsigned int     fast_recover_cycle;   // ��ָ�����
	unsigned int     slow_recover_cycle;   // ���ָ�����
	unsigned char    fast_recover_num;     // ��ָ�����
	busoff_revover_handle_cb busoff_recover_fun;     // ����ָ������Ļص��������͡�
	busoff_state_report_MPU_cb busoff_report_fun;    //

}BUSOFF_CFG_ST;// BUSOFF ���ýṹ��

typedef struct
{
	unsigned char report_to_MPU_EN;            // !0: Ҫ��MPU�㱨ͨѶ��ѹ״̬��0������Ҫ�㱨��
	float         high_thd;                 //  ͨѶ��ѹ��Χ����ֵ
	float         low_thd;                  //  ͨѶ��ѹ��Χ����ֵ
	comm_vol_state_reportMPU_cb comm_vol_report_fun;
	
}COMM_VOL_CFG_ST;  // ͨѶ��ѹ���ýṹ��

typedef struct
{	
	NM_CFG_ST nm_cfg[2];
	BUSOFF_CFG_ST busoff_cfg;
	COMM_VOL_CFG_ST comm_vol_cfg;	
}CAN_BASIC_CFG_ST;



void can_basic_cfg_Init(void);

#endif

