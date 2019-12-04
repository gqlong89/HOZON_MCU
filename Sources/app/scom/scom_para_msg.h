#ifndef __SCOM_PARA_MSG_H__
#define __SCOM_PARA_MSG_H__


enum SCOM_PARA_ID
{
    SCOM_PARA_ID_SYSMODE       = 0,
    SCOM_PARA_ID_BAT_TYPE,
    SCOM_PARA_ID_VTD,           // vehicle theft deterrent
    SCOM_PARA_ID_ECALL,
    SCOM_PARA_ID_BCALL,
    SCOM_PARA_ID_ICALL,
    SCOM_PARA_ID_SLP_TIME,
    SCOM_PARA_ID_DEEP_SLP_TIME,
    SCOM_PARA_ID_ACCVECMTHS,
    SCOM_PARA_ID_SN,
    SCOM_PARA_ID_MAX,
};


void scom_tl_ex_para_query(unsigned char *msg, unsigned int len);

void scom_tl_ex_para_cfg(unsigned char *msg, unsigned int len);

void scom_tl_ex_para_report(unsigned int para_id);

#endif

