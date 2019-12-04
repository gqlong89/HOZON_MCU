#include <app/scom/scom_para_msg.h>
#include <app/scom/scom_tl.h>
#include <config.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <scom_msg_def.h>


static void scom_tl_ex_para_set_sysmode(unsigned char *msg)
{
    unsigned char sysmode;
    sysmode = msg[sizeof(SCOM_TL_MSG_HDR) + 2];
    cfg_save_sysmode(sysmode);
}

static void scom_tl_ex_para_set_bat_type(unsigned char *msg)
{
    unsigned char type;
    type = msg[sizeof(SCOM_TL_MSG_HDR) + 2];
    cfg_save_batterytype(type);
}

static void scom_tl_ex_para_set_vtd(unsigned char *msg)
{
    unsigned char anti_thief_enable;
    anti_thief_enable = msg[sizeof(SCOM_TL_MSG_HDR) + 2];
    cfg_save_anti_thief(anti_thief_enable);
}

static void scom_tl_ex_para_set_ecall(unsigned char *msg)
{
    unsigned char enable;
    enable = msg[sizeof(SCOM_TL_MSG_HDR) + 2];
    cfg_save_ecall(enable);
}

static void scom_tl_ex_para_set_bcall(unsigned char *msg)
{
    unsigned char enable;
    enable = msg[sizeof(SCOM_TL_MSG_HDR) + 2];
    cfg_save_bcall(enable);
}

static void scom_tl_ex_para_set_icall(unsigned char *msg)
{
    unsigned char enable;
    enable = msg[sizeof(SCOM_TL_MSG_HDR) + 2];
    cfg_save_icall(enable);
}

static void scom_tl_ex_para_set_sleeptime(unsigned char *msg)
{
    unsigned int *time;
    time = (unsigned int *) &msg[sizeof(SCOM_TL_MSG_HDR) + 2];
    cfg_save_standbysleep(*time);
}

static void scom_tl_ex_para_set_dsleeptime(unsigned char *msg)
{
    unsigned int *time;
    time = (unsigned int *) &msg[sizeof(SCOM_TL_MSG_HDR) + 2];
    cfg_save_deep_standbysleep(*time);
}


static void scom_tl_ex_para_set_accvecmths(unsigned char *msg)
{
    unsigned short *accvecmths;
    accvecmths = (unsigned short *) &msg[sizeof(SCOM_TL_MSG_HDR) + 2];
    cfg_save_acc_vecm_ths(*accvecmths);
}

static void scom_tl_ex_para_set_sn(unsigned char *msg)
{
    unsigned int *SN;
    SN = (unsigned int *) &msg[sizeof(SCOM_TL_MSG_HDR) + 2];
    taskPrintf(TASK_LEVEL_ERROR, "SN = %u!", *SN);
    cfg_save_sn(*SN);
}

static void scom_tl_ex_para_set(unsigned int para_id, unsigned char *msg)
{
    switch (para_id)
    {
        case SCOM_PARA_ID_SYSMODE:
            scom_tl_ex_para_set_sysmode(msg);
            break;

        case SCOM_PARA_ID_BAT_TYPE:
            scom_tl_ex_para_set_bat_type(msg);
            break;

        case SCOM_PARA_ID_VTD:
            scom_tl_ex_para_set_vtd(msg);
            break;

        case SCOM_PARA_ID_ECALL:
            scom_tl_ex_para_set_ecall(msg);
            break;

        case SCOM_PARA_ID_BCALL:
            scom_tl_ex_para_set_bcall(msg);
            break;

        case SCOM_PARA_ID_ICALL:
            scom_tl_ex_para_set_icall(msg);
            break;

        case SCOM_PARA_ID_SLP_TIME:
            scom_tl_ex_para_set_sleeptime(msg);
            break;

        case SCOM_PARA_ID_DEEP_SLP_TIME:
            scom_tl_ex_para_set_dsleeptime(msg);
            break;

        case SCOM_PARA_ID_ACCVECMTHS:
            scom_tl_ex_para_set_accvecmths(msg);
            break;

        case SCOM_PARA_ID_SN:
            scom_tl_ex_para_set_sn(msg);
            break;

        default:
            taskPrintf(TASK_LEVEL_ERROR, "unknown para_id=%u!", para_id);
            break;
    }
}

void scom_tl_ex_para_cfg(unsigned char *msg, unsigned int len)
{
    UNUSED(len);
    unsigned int para_id;

    para_id = msg[sizeof(SCOM_TL_MSG_HDR)] + (msg[sizeof(SCOM_TL_MSG_HDR) + 1] << 8);

    scom_tl_ex_para_set(para_id, msg);
}

static void scom_tl_ex_para_report_sysmode(void)
{
    unsigned char data[8] = {0};
    unsigned int size = 0;

    data[size++] = SCOM_PARA_ID_SYSMODE;
    data[size++] = SCOM_PARA_ID_SYSMODE >> 8;

    data[size++] = cfg_get_sysmode();

    scom_tl_send_frame(SCOM_TL_CMD_PARA_REPORT, SCOM_TL_SINGLE_FRAME, 0, data, size);
}

static void scom_tl_ex_para_report_bat_type(void)
{
    unsigned char data[8] = {0};
    unsigned int size = 0;

    data[size++] = SCOM_PARA_ID_BAT_TYPE;
    data[size++] = SCOM_PARA_ID_BAT_TYPE >> 8;

    data[size++] = cfg_get_batterytype();

    scom_tl_send_frame(SCOM_TL_CMD_PARA_REPORT, SCOM_TL_SINGLE_FRAME, 0, data, size);
}

static void scom_tl_ex_para_report_vtd(void)
{
    unsigned char data[8] = {0};
    unsigned int size = 0;

    data[size++] = SCOM_PARA_ID_VTD;
    data[size++] = SCOM_PARA_ID_VTD >> 8;

    data[size++] = cfg_get_anti_thief();

    scom_tl_send_frame(SCOM_TL_CMD_PARA_REPORT, SCOM_TL_SINGLE_FRAME, 0, data, size);
}

static void scom_tl_ex_para_report_ecall(void)
{
    unsigned char data[8] = {0};
    unsigned int size = 0;

    data[size++] = SCOM_PARA_ID_ECALL;
    data[size++] = SCOM_PARA_ID_ECALL >> 8;

    data[size++] = cfg_get_ecall();

    scom_tl_send_frame(SCOM_TL_CMD_PARA_REPORT, SCOM_TL_SINGLE_FRAME, 0, data, size);
}

static void scom_tl_ex_para_report_bcall(void)
{
    unsigned char data[8] = {0};
    unsigned int size = 0;

    data[size++] = SCOM_PARA_ID_BCALL;
    data[size++] = SCOM_PARA_ID_BCALL >> 8;

    data[size++] = cfg_get_bcall();

    scom_tl_send_frame(SCOM_TL_CMD_PARA_REPORT, SCOM_TL_SINGLE_FRAME, 0, data, size);
}

static void scom_tl_ex_para_report_icall(void)
{
    unsigned char data[8] = {0};
    unsigned int size = 0;

    data[size++] = SCOM_PARA_ID_ICALL;
    data[size++] = SCOM_PARA_ID_ICALL >> 8;

    data[size++] = cfg_get_icall();

    scom_tl_send_frame(SCOM_TL_CMD_PARA_REPORT, SCOM_TL_SINGLE_FRAME, 0, data, size);
}

static void scom_tl_ex_para_report_standbytime(void)
{
    unsigned char data[8] = {0};
    unsigned int size = 0;

    data[size++] = SCOM_PARA_ID_SLP_TIME;
    data[size++] = SCOM_PARA_ID_SLP_TIME >> 8;

    data[size++] = cfg_get_standbysleep();
    data[size++] = cfg_get_standbysleep() >> 8;
    data[size++] = cfg_get_standbysleep() >> 16;
    data[size++] = cfg_get_standbysleep() >> 24;

    scom_tl_send_frame(SCOM_TL_CMD_PARA_REPORT, SCOM_TL_SINGLE_FRAME, 0, data, size);
}

static void scom_tl_ex_para_report_deepstandbytime(void)
{
    unsigned char data[8] = {0};
    unsigned int size = 0;

    data[size++] = SCOM_PARA_ID_DEEP_SLP_TIME;
    data[size++] = SCOM_PARA_ID_DEEP_SLP_TIME >> 8;

    data[size++] = cfg_get_deep_standbysleep();
    data[size++] = cfg_get_deep_standbysleep() >> 8;
    data[size++] = cfg_get_deep_standbysleep() >> 16;
    data[size++] = cfg_get_deep_standbysleep() >> 24;

    scom_tl_send_frame(SCOM_TL_CMD_PARA_REPORT, SCOM_TL_SINGLE_FRAME, 0, data, size);
}

static void scom_tl_ex_para_report_accvecmths(void)
{
    unsigned char data[8] = {0};
    unsigned int size = 0;

    data[size++] = SCOM_PARA_ID_ACCVECMTHS;
    data[size++] = SCOM_PARA_ID_ACCVECMTHS >> 8;

    data[size++] = cfg_get_acc_vecm_ths();
    data[size++] = cfg_get_acc_vecm_ths() >> 8;

    scom_tl_send_frame(SCOM_TL_CMD_PARA_REPORT, SCOM_TL_SINGLE_FRAME, 0, data, size);
}

static void scom_tl_ex_para_report_sn(void)
{
    unsigned char data[8] = {0};
    unsigned int size = 0;

    data[size++] = SCOM_PARA_ID_SN;
    data[size++] = SCOM_PARA_ID_SN >> 8;

    data[size++] = cfg_get_sn();
    data[size++] = cfg_get_sn() >> 8;
    data[size++] = cfg_get_sn() >> 16;
    data[size++] = cfg_get_sn() >> 24;

    scom_tl_send_frame(SCOM_TL_CMD_PARA_REPORT, SCOM_TL_SINGLE_FRAME, 0, data, size);
}

void scom_tl_ex_para_report(unsigned int para_id)
{
    switch (para_id)
    {
        case SCOM_PARA_ID_SYSMODE:
            scom_tl_ex_para_report_sysmode();
            break;

        case SCOM_PARA_ID_BAT_TYPE:
            scom_tl_ex_para_report_bat_type();
            break;

        case SCOM_PARA_ID_VTD:
            scom_tl_ex_para_report_vtd();
            break;

        case SCOM_PARA_ID_ECALL:
            scom_tl_ex_para_report_ecall();
            break;

        case SCOM_PARA_ID_BCALL:
            scom_tl_ex_para_report_bcall();
            break;

        case SCOM_PARA_ID_ICALL:
            scom_tl_ex_para_report_icall();
            break;

        case SCOM_PARA_ID_SLP_TIME:
            scom_tl_ex_para_report_standbytime();
            break;

        case SCOM_PARA_ID_DEEP_SLP_TIME:
            scom_tl_ex_para_report_deepstandbytime();
            break;

        case SCOM_PARA_ID_ACCVECMTHS:
            scom_tl_ex_para_report_accvecmths();
            break;

        case SCOM_PARA_ID_SN:
            scom_tl_ex_para_report_sn();
            break;

        default:
            taskPrintf(TASK_LEVEL_ERROR, "unknown para_id=%u!", para_id);
            break;
    }
}

void scom_tl_ex_para_query(unsigned char *msg, unsigned int len)
{
    UNUSED(len);
    unsigned int para_id;

    para_id = msg[sizeof(SCOM_TL_MSG_HDR)] + (msg[sizeof(SCOM_TL_MSG_HDR) + 1] << 8);

    scom_tl_ex_para_report(para_id);
}

