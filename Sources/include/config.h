#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <can_api.h>
#include <app/can_mgr/can_mgr.h>
#include <app/tp/proxy/Proxy.h>

typedef enum
{
    STANDBY_NO_SLEEP = 0,
    STANDBY_SLEEP,
    STANDBY_DEEP_SLEEP,
} STANDBY_SLEEP_TYPE;

/***************    General configuration parameter begin       ***************/
#pragma pack(1)
typedef struct
{
    unsigned char pc[10];
    unsigned char cs;
} ProductCodeStr;
#pragma pack(0)

#pragma pack(1)
typedef struct
{
    unsigned int sn;
    unsigned char cs;
} SerialNumStr;
#pragma pack(0)

/* reboot counter structure */
#pragma pack(1)
typedef struct
{
    unsigned int cnt;
    unsigned char cs;
} RebootCntStr;
#pragma pack(0)

/* SYS run time structure */
#pragma pack(1)
typedef struct
{
    unsigned int time;
    unsigned char cs;
} SYSRunTimeStr;
#pragma pack(0)


/* MCU configuration structure */
// This configuration version is: 0x0001
#define MCU_CONFIG_VERSION          0x0001

enum CFG_BATTERY_TYPE
{
    BATTERY_UNKNOWN = 0,
    BATTERY_VARTA_450,
    BATTERY_VARTA_500,
    BATTERY_PANASONIC_500,
    BATTERY_TYPE_MAX,
};

#pragma pack(1)
typedef struct
{
    unsigned short ver;
    unsigned char  sys_mode;
    unsigned char  bat_type;
    unsigned char  anti_thief_enable;
    unsigned char  ecall_enbale;
    unsigned char  bcall_enbale;
    unsigned char  icall_enbale;
    unsigned int   sleep_time;
    unsigned int   deep_sleep_time;
    unsigned short acc_vecm_ths;
    unsigned char  reserved[21];
    unsigned char  cs;
} MCUConfStr;
#pragma pack(0)

#pragma pack(1)
typedef struct
{
    MCUConfStr mcu_conf;
    unsigned int sn;
} ConfigSyncStr;
#pragma pack(0)

/* can msg structure */
#pragma pack(1)
typedef struct
{
    MES_PARAM_STR   info[CAN_MES_NUM];
    unsigned char   cs;
} CanMsgSendStr;
#pragma pack(0)

#pragma pack(1)
typedef struct
{
    uds_set_def     uds_cfg;
    unsigned char   cs;
} UDSconfigStr;
#pragma pack(0)
/***************    General configuration parameter end     ***************/


/***************    MCU flag parameters begin       ***************/
/* MCU flag1 structure */
#pragma pack(1)
typedef struct
{
    unsigned char   standby;
    unsigned char   cs;
} MCUFlag1Str;
#pragma pack(0)

/* MCU flag2 structure */
/* Record the time of the last battery test */
#pragma pack(1)
typedef struct
{
    unsigned int    time;
    unsigned char   cs;
} MCUFlag2Str;
#pragma pack(0)

#pragma pack(1)
typedef struct
{
    unsigned char   wake_cnt;
    unsigned char   cs;
} MCUFlag3Str;
#pragma pack(0)

/***************    MCU flag parameters end     ***************/


/***************    Custom configuration parameter begin       ***************/


/***************    Custom configuration parameter end       ***************/


/*--------------------------------------------*/
/* Flash   PARA Section #1                    */
/* addr:   0x14000000U - 0x140003FFU          */
/* size:   1024  Byte                         */
/* used:    436  Byte                         */
/*--------------------------------------------*/
/* memory map */
#define FLASH_SEC1_START_ADDR           (APP_PARA_START_ADDR)
#define PRODUCT_CODE_ST_ADDR            (FLASH_SEC1_START_ADDR)
#define SERIAL_NUM_ST_ADDR              (PRODUCT_CODE_ST_ADDR   + sizeof(ProductCodeStr) * 2)
#define REBOOTCNT_ST_ADDR               (SERIAL_NUM_ST_ADDR     + sizeof(SerialNumStr) * 2)
#define SYSRUNTIME_ST_ADDR              (REBOOTCNT_ST_ADDR      + sizeof(RebootCntStr) * 2)
#define CAN_DFTBAUD_ST_ADDR             (SYSRUNTIME_ST_ADDR     + sizeof(SYSRunTimeStr) * 2)
#define MCU_CONF_ST_ADDR                (CAN_DFTBAUD_ST_ADDR    + sizeof(CanDftBaudStr) * 2)
#define CAN_MSG_SEND_ST_ADDR            (MCU_CONF_ST_ADDR       + sizeof(MCUConfStr) * 2)
#define UDS_CONFIG_ST_ADDR              (CAN_MSG_SEND_ST_ADDR   + sizeof(CanMsgSendStr) * 2)
#define FLASH_SEC1_END_ADDR             (UDS_CONFIG_ST_ADDR     + sizeof(UDSconfigStr) * 2)

/*--------------------------------------------*/
/* Flash   PARA Section #2                    */
/* addr:   0x14000400U - 0x140005FFU          */
/* size:    512   Byte                        */
/* used:     14   Byte                        */
/*--------------------------------------------*/
/* memory map */
#define FLASH_SEC2_START_ADDR           (APP_PARA_START_ADDR + 1024)
#define MCU_FLAG1_ST_ADDR               (FLASH_SEC2_START_ADDR)
#define MCU_FLAG2_ST_ADDR               (MCU_FLAG1_ST_ADDR      + sizeof(MCUFlag1Str) * 2)
#define MCU_FLAG3_ST_ADDR               (MCU_FLAG2_ST_ADDR      + sizeof(MCUFlag2Str) * 2)
#define FLASH_SEC2_END_ADDR             (MCU_FLAG3_ST_ADDR      + sizeof(MCUFlag3Str) * 2)

/*--------------------------------------------*/
/* Flash   PARA Section #3                    */
/* addr:   0x14000600U - 0x140009FFU          */
/* size:   1024   Byte                        */
/* used:     50   Byte                        */
/*--------------------------------------------*/
/* memory map */
#define FLASH_SEC3_START_ADDR           (APP_PARA_START_ADDR + 1024 + 512)
#define FLASH_SEC3_END_ADDR             (FLASH_SEC3_START_ADDR)

unsigned int cfg_restore_pcode(unsigned char *pdata);
unsigned int cfg_save_pcode(unsigned char *pdata);

unsigned int cfg_restore_sn(unsigned int *sn);
unsigned int cfg_save_sn(unsigned int sn);
unsigned int cfg_get_sn(void);

unsigned int cfg_restore_rbt(unsigned int *cnt);
unsigned int cfg_save_rbt(unsigned int cnt);
unsigned int cfg_get_rbt(void);

unsigned int cfg_restore_sysruntime(unsigned int *time);
unsigned int cfg_save_sysruntime(unsigned int time);

unsigned int cfg_restore_candftbaud(CanDftBaudStr *pdata);
unsigned int cfg_save_candftbaud(CanDftBaudStr *data);

unsigned int cfg_restore_mcu_conf(MCUConfStr *conf);
unsigned int cfg_save_mcu_conf(MCUConfStr *conf);
MCUConfStr *cfg_get_mcu_conf(void);

unsigned int cfg_restore_canmsgsend(CanMsgSendStr *msg);
unsigned int cfg_save_canmsgsend(unsigned char *msg);

/* mcu confige begin */
unsigned char cfg_get_sysmode(void);
void cfg_save_sysmode(unsigned char mode);
unsigned char cfg_get_batterytype(void);
void cfg_save_batterytype(unsigned char type);
unsigned char cfg_get_anti_thief(void);
void cfg_save_anti_thief(unsigned char enable);
unsigned char cfg_get_ecall(void);
unsigned char cfg_get_bcall(void);
unsigned char cfg_get_icall(void);
void cfg_save_ecall(unsigned char enable);
void cfg_save_bcall(unsigned char enable);
void cfg_save_icall(unsigned char enable);
unsigned int cfg_get_standbysleep(void);
void cfg_save_standbysleep(unsigned int time);
unsigned int cfg_get_deep_standbysleep(void);
void cfg_save_deep_standbysleep(unsigned int time);
unsigned short cfg_get_acc_vecm_ths(void);
void cfg_save_acc_vecm_ths(unsigned short acc_vecm_ths);
void cfg_init(void);
/* mcu confige end   */

unsigned int  cfg_restore_standbyflag(unsigned char *flag);
unsigned int  cfg_save_standbyflag(unsigned char flag);
unsigned char cfg_get_standbyflag(void);

unsigned int cfg_restore_batchktime(unsigned int *time);
unsigned int cfg_save_batchktime(unsigned int time);

unsigned int  cfg_restore_wake_cnt(unsigned char *wake_cnt);
unsigned int  cfg_save_wake_cnt(unsigned char wake_cnt);
unsigned char cfg_get_wake_cnt(void);


/*uds cfg*/
unsigned int cfg_restore_uds(unsigned char *pdata);
unsigned int cfg_save_uds(unsigned char *pdata);

unsigned int cfg_erase(unsigned int start_addr, unsigned int end_addr);
void cfg_dump(unsigned int addr, unsigned int len);

#endif

