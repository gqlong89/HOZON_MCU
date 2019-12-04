#include <config.h>
#include <flash.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <string.h>

#define STANDBY_SLEEP_TIME          (7*24*60)
#define STANDBY_DEEP_SLEEP_TIME     (14*24*60)
#define ACC_VECM_THS                (500*2048/1000)


static MCUConfStr       mcu_conf;
static unsigned int     mcu_reboot;
static unsigned int     mcu_sn;
static unsigned char    flag_standby;
static unsigned char    flag_wake_cnt;


unsigned int cfg_restore_pcode(unsigned char *pdata)
{
    ProductCodeStr str;
    unsigned int size = sizeof(ProductCodeStr);

    if (flash_eeprom_checksum_read(PRODUCT_CODE_ST_ADDR, (unsigned char *) &str, size))
    {
        if (flash_eeprom_checksum_read(PRODUCT_CODE_ST_ADDR + size, (unsigned char *) &str, size))
        {
            return 1;
        }
    }

    memcpy(pdata, str.pc, sizeof(str.pc));
    return 0;
}

unsigned int cfg_save_pcode(unsigned char *pdata)
{
    ProductCodeStr str;
    unsigned int size = sizeof(ProductCodeStr);

    memcpy(str.pc, pdata, sizeof(str.pc));
    flash_eeprom_checksum_write(PRODUCT_CODE_ST_ADDR, (unsigned char *) &str, size);
    flash_eeprom_checksum_write(PRODUCT_CODE_ST_ADDR + size, (unsigned char *) &str, size);
    return 0;
}

unsigned int cfg_restore_sn(unsigned int *sn)
{
    SerialNumStr str;
    unsigned int size = sizeof(SerialNumStr);

    if (flash_eeprom_checksum_read(SERIAL_NUM_ST_ADDR, (unsigned char *) &str, size))
    {
        if (flash_eeprom_checksum_read(SERIAL_NUM_ST_ADDR + size, (unsigned char *) &str, size))
        {
            return 1;
        }
    }

    *sn = str.sn;
    return 0;
}

unsigned int cfg_save_sn(unsigned int sn)
{
    SerialNumStr str;
    unsigned int size = sizeof(SerialNumStr);

    str.sn = sn;
    mcu_sn = sn;
    flash_eeprom_checksum_write(SERIAL_NUM_ST_ADDR, (unsigned char *) &str, size);
    flash_eeprom_checksum_write(SERIAL_NUM_ST_ADDR + size, (unsigned char *) &str, size);
    return 0;
}

unsigned int cfg_get_sn(void)
{
    return mcu_sn;
}

unsigned int cfg_restore_rbt(unsigned int *cnt)
{
    RebootCntStr str;
    unsigned int size = sizeof(RebootCntStr);

    if (flash_eeprom_checksum_read(REBOOTCNT_ST_ADDR, (unsigned char *) &str, size))
    {
        if (flash_eeprom_checksum_read(REBOOTCNT_ST_ADDR + size, (unsigned char *) &str, size))
        {
            return 1;
        }
    }

    *cnt = str.cnt;
    return 0;
}

unsigned int cfg_save_rbt(unsigned int cnt)
{
    RebootCntStr str;
    unsigned int size = sizeof(RebootCntStr);
    str.cnt = cnt;
    mcu_reboot = cnt;
    flash_eeprom_checksum_write(REBOOTCNT_ST_ADDR, (unsigned char *) &str, size);
    flash_eeprom_checksum_write(REBOOTCNT_ST_ADDR + size, (unsigned char *) &str, size);
    return 0;
}

unsigned int cfg_get_rbt(void)
{
    return mcu_reboot;
}

unsigned int cfg_restore_sysruntime(unsigned int *time)
{
    SYSRunTimeStr str;
    unsigned int size = sizeof(SYSRunTimeStr);

    if (flash_eeprom_checksum_read(SYSRUNTIME_ST_ADDR, (unsigned char *) &str, size))
    {
        if (flash_eeprom_checksum_read(SYSRUNTIME_ST_ADDR + size, (unsigned char *) &str, size))
        {
            return 1;
        }
    }

    *time = str.time;
    return 0;
}

unsigned int cfg_save_sysruntime(unsigned int time)
{
    SYSRunTimeStr str;
    unsigned int size = sizeof(SYSRunTimeStr);

    str.time = time;
    flash_eeprom_checksum_write(SYSRUNTIME_ST_ADDR, (unsigned char *) &str, size);
    flash_eeprom_checksum_write(SYSRUNTIME_ST_ADDR + size, (unsigned char *) &str, size);
    return 0;
}

unsigned int cfg_restore_candftbaud(CanDftBaudStr *pdata)
{
    unsigned int size = sizeof(CanDftBaudStr);

    if (flash_eeprom_checksum_read(CAN_DFTBAUD_ST_ADDR, (unsigned char *) pdata, size))
    {
        if (flash_eeprom_checksum_read(CAN_DFTBAUD_ST_ADDR + size, (unsigned char *) pdata, size))
        {
            return 1;
        }
    }

    return 0;
}

unsigned int cfg_save_candftbaud(CanDftBaudStr *data)
{
    unsigned int size = sizeof(CanDftBaudStr);
    flash_eeprom_checksum_write(CAN_DFTBAUD_ST_ADDR, (unsigned char *) data, size);
    flash_eeprom_checksum_write(CAN_DFTBAUD_ST_ADDR + size, (unsigned char *) data, size);
    return 0;
}

unsigned int cfg_restore_mcu_conf(MCUConfStr *conf)
{
    unsigned int size = sizeof(MCUConfStr);

    if (flash_eeprom_checksum_read(MCU_CONF_ST_ADDR, (unsigned char *) conf, size))
    {
        if (flash_eeprom_checksum_read(MCU_CONF_ST_ADDR + size, (unsigned char *) conf, size))
        {
            return 1;
        }
    }

    return 0;
}

unsigned int cfg_save_mcu_conf(MCUConfStr *conf)
{
    unsigned int size = sizeof(MCUConfStr);

    flash_eeprom_checksum_write(MCU_CONF_ST_ADDR, (unsigned char *) conf, size);
    flash_eeprom_checksum_write(MCU_CONF_ST_ADDR + size, (unsigned char *) conf, size);
    return 0;
}

MCUConfStr *cfg_get_mcu_conf(void)
{
    return &mcu_conf;
}

unsigned int cfg_restore_canmsgsend(CanMsgSendStr *msg)
{
    unsigned int size = sizeof(CanMsgSendStr);

    if (flash_eeprom_checksum_read(CAN_MSG_SEND_ST_ADDR, (unsigned char *) msg, size))
    {
        if (flash_eeprom_checksum_read(CAN_MSG_SEND_ST_ADDR + size, (unsigned char *) msg, size))
        {
            return 1;
        }
    }

    return 0;
}

unsigned int cfg_save_canmsgsend(unsigned char *msg)
{
    unsigned int size = sizeof(CanMsgSendStr);

    flash_eeprom_checksum_write(CAN_MSG_SEND_ST_ADDR, (unsigned char *) msg, size);
    flash_eeprom_checksum_write(CAN_MSG_SEND_ST_ADDR + size, (unsigned char *) msg, size);
    return 0;
}

/* mcu confige begin */
unsigned char cfg_get_sysmode(void)
{
    return mcu_conf.sys_mode;
}

void cfg_save_sysmode(unsigned char mode)
{
    unsigned int size = sizeof(MCUConfStr);

    if (mcu_conf.sys_mode != mode)
    {
        mcu_conf.sys_mode = mode;
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR, (unsigned char *) &mcu_conf, size);
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR + size, (unsigned char *) &mcu_conf, size);
    }
}

unsigned char cfg_get_batterytype(void)
{
    return mcu_conf.bat_type;
}

void cfg_save_batterytype(unsigned char type)
{
    unsigned int size = sizeof(MCUConfStr);

    if (mcu_conf.bat_type != type)
    {
        mcu_conf.bat_type = type;
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR, (unsigned char *) &mcu_conf, size);
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR + size, (unsigned char *) &mcu_conf, size);
    }
}

unsigned char cfg_get_anti_thief(void)
{
    return mcu_conf.anti_thief_enable;
}

void cfg_save_anti_thief(unsigned char enable)
{
    unsigned int size = sizeof(MCUConfStr);

    if (mcu_conf.anti_thief_enable != enable)
    {
        mcu_conf.anti_thief_enable = enable;
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR, (unsigned char *) &mcu_conf, size);
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR + size, (unsigned char *) &mcu_conf, size);
    }
}

unsigned char cfg_get_ecall(void)
{
    return mcu_conf.ecall_enbale;
}

unsigned char cfg_get_bcall(void)
{
    return mcu_conf.bcall_enbale;
}

unsigned char cfg_get_icall(void)
{
    return mcu_conf.icall_enbale;
}

void cfg_save_ecall(unsigned char enable)
{
    unsigned int size = sizeof(MCUConfStr);

    if (mcu_conf.ecall_enbale != enable)
    {
        mcu_conf.ecall_enbale = enable;
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR, (unsigned char *) &mcu_conf, size);
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR + size, (unsigned char *) &mcu_conf, size);
    }
}

void cfg_save_bcall(unsigned char enable)
{
    unsigned int size = sizeof(MCUConfStr);

    if (mcu_conf.bcall_enbale != enable)
    {
        mcu_conf.bcall_enbale = enable;
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR, (unsigned char *) &mcu_conf, size);
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR + size, (unsigned char *) &mcu_conf, size);
    }
}

void cfg_save_icall(unsigned char enable)
{
    unsigned int size = sizeof(MCUConfStr);

    if (mcu_conf.icall_enbale != enable)
    {
        mcu_conf.icall_enbale = enable;
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR, (unsigned char *) &mcu_conf, size);
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR + size, (unsigned char *) &mcu_conf, size);
    }
}

unsigned int cfg_get_standbysleep(void)
{
    return mcu_conf.sleep_time;
}

void cfg_save_standbysleep(unsigned int time)
{
    unsigned int size = sizeof(MCUConfStr);

    if (mcu_conf.sleep_time != time)
    {
        mcu_conf.sleep_time = time;
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR, (unsigned char *) &mcu_conf, size);
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR + size, (unsigned char *) &mcu_conf, size);
    }
}

unsigned int cfg_get_deep_standbysleep(void)
{
    return mcu_conf.deep_sleep_time;
}

void cfg_save_deep_standbysleep(unsigned int time)
{
    unsigned int size = sizeof(MCUConfStr);

    if (mcu_conf.deep_sleep_time != time)
    {
        mcu_conf.deep_sleep_time = time;
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR, (unsigned char *) &mcu_conf, size);
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR + size, (unsigned char *) &mcu_conf, size);
    }
}

unsigned short cfg_get_acc_vecm_ths(void)
{
    return mcu_conf.acc_vecm_ths;
}

void cfg_save_acc_vecm_ths(unsigned short acc_vecm_ths)
{
    unsigned int size = sizeof(MCUConfStr);

    if (mcu_conf.acc_vecm_ths != acc_vecm_ths)
    {
        mcu_conf.acc_vecm_ths = acc_vecm_ths;
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR, (unsigned char *) &mcu_conf, size);
        flash_eeprom_checksum_write(MCU_CONF_ST_ADDR + size, (unsigned char *) &mcu_conf, size);
    }
}

void cfg_init(void)
{
    if (cfg_restore_mcu_conf(&mcu_conf))
    {
        memset(&mcu_conf, 0, sizeof(mcu_conf));
        mcu_conf.ver = MCU_CONFIG_VERSION;
        mcu_conf.sys_mode = 0;
        mcu_conf.bat_type = 0;
        mcu_conf.anti_thief_enable = 0;
        mcu_conf.ecall_enbale = 0;
        mcu_conf.bcall_enbale = 0;
        mcu_conf.icall_enbale = 0;
        mcu_conf.sleep_time = STANDBY_SLEEP_TIME;
        mcu_conf.deep_sleep_time = STANDBY_DEEP_SLEEP_TIME;
        mcu_conf.acc_vecm_ths = ACC_VECM_THS;
        cfg_save_mcu_conf(&mcu_conf);
    }

    if (cfg_restore_rbt(&mcu_reboot))
    {
        mcu_reboot = 0;
        cfg_save_rbt(mcu_reboot);
    }

    if (cfg_restore_sn(&mcu_sn))
    {
        mcu_reboot = 0;
        cfg_save_sn(mcu_sn);
    }

    if (cfg_restore_standbyflag(&flag_standby))
    {
        cfg_save_standbyflag(STANDBY_NO_SLEEP);
        cfg_save_wake_cnt(0);
    }

    if (cfg_restore_wake_cnt(&flag_wake_cnt))
    {
        cfg_save_standbyflag(STANDBY_NO_SLEEP);
        cfg_save_wake_cnt(0);
    }
}
/* mcu confige end   */

unsigned int cfg_restore_standbyflag(unsigned char *flag)
{
    MCUFlag1Str str;
    unsigned int size = sizeof(MCUFlag1Str);

    if (flash_eeprom_checksum_read(MCU_FLAG1_ST_ADDR, (unsigned char *) &str, size))
    {
        if (flash_eeprom_checksum_read(MCU_FLAG1_ST_ADDR + size, (unsigned char *) &str, size))
        {
            return 1;
        }
    }

    *flag = str.standby;
    return 0;
}

unsigned int cfg_save_standbyflag(unsigned char flag)
{
    MCUFlag1Str str;
    unsigned int size = sizeof(MCUFlag1Str);

    if (flag_standby != flag)
    {
        str.standby  = flag;
        flag_standby = flag;
        flash_eeprom_checksum_write(MCU_FLAG1_ST_ADDR, (unsigned char *) &str, size);
        flash_eeprom_checksum_write(MCU_FLAG1_ST_ADDR + size, (unsigned char *) &str, size);
    }

    return 0;
}

unsigned char cfg_get_standbyflag(void)
{
    return flag_standby;
}

unsigned int cfg_restore_batchktime(unsigned int *time)
{
    MCUFlag2Str str;
    unsigned int size = sizeof(MCUFlag2Str);

    if (flash_eeprom_checksum_read(MCU_FLAG2_ST_ADDR, (unsigned char *) &str, size))
    {
        if (flash_eeprom_checksum_read(MCU_FLAG2_ST_ADDR + size, (unsigned char *) &str, size))
        {
            return 1;
        }
    }

    *time = str.time;
    return 0;
}

unsigned int cfg_save_batchktime(unsigned int time)
{
    MCUFlag2Str str;
    unsigned int size = sizeof(MCUFlag2Str);
    str.time = time;
    flash_eeprom_checksum_write(MCU_FLAG2_ST_ADDR, (unsigned char *) &str, size);
    flash_eeprom_checksum_write(MCU_FLAG2_ST_ADDR + size, (unsigned char *) &str, size);
    return 0;
}

unsigned int cfg_restore_wake_cnt(unsigned char *wake_cnt)
{
    MCUFlag3Str str;
    unsigned int size = sizeof(MCUFlag3Str);

    if (flash_eeprom_checksum_read(MCU_FLAG3_ST_ADDR, (unsigned char *) &str, size))
    {
        if (flash_eeprom_checksum_read(MCU_FLAG3_ST_ADDR + size, (unsigned char *) &str, size))
        {
            return 1;
        }
    }

    *wake_cnt = str.wake_cnt;

    return 0;
}

unsigned int cfg_save_wake_cnt(unsigned char wake_cnt)
{
    MCUFlag3Str str;
    unsigned int size = sizeof(MCUFlag3Str);

    if (flag_wake_cnt != wake_cnt)
    {
        str.wake_cnt  = wake_cnt;
        flag_wake_cnt = wake_cnt;
        flash_eeprom_checksum_write(MCU_FLAG3_ST_ADDR, (unsigned char *) &str, size);
        flash_eeprom_checksum_write(MCU_FLAG3_ST_ADDR + size, (unsigned char *) &str, size);
    }

    return 0;
}

unsigned char cfg_get_wake_cnt(void)
{
    return flag_wake_cnt;
}





unsigned int cfg_restore_uds(unsigned char *pdata)
{
    UDSconfigStr str;
    unsigned int size = sizeof(str);

    if (flash_eeprom_checksum_read(UDS_CONFIG_ST_ADDR, (unsigned char *) &str, size))
    {
        if (flash_eeprom_checksum_read(UDS_CONFIG_ST_ADDR + size, (unsigned char *) &str, size))
        {
            return 1;
        }
    }

    memcpy(pdata, (unsigned char *) & (str.uds_cfg), sizeof(str.uds_cfg));
    return 0;
}

unsigned int cfg_save_uds(unsigned char *pdata)
{
    UDSconfigStr str;
    unsigned int size = sizeof(UDSconfigStr);

    uds_set_def uds_set;
    cfg_restore_uds((unsigned char *)&uds_set);

    memcpy((unsigned char *) & (str.uds_cfg), pdata, sizeof(str.uds_cfg));

    if (memcmp((unsigned char *)&uds_set, (unsigned char *)&str.uds_cfg, sizeof(str.uds_cfg)) != 0)
    {
        taskPrintf(TASK_LEVEL_DEBUG, "save uds config");
        flash_eeprom_checksum_write(UDS_CONFIG_ST_ADDR, (unsigned char *) &str, size);
        flash_eeprom_checksum_write(UDS_CONFIG_ST_ADDR + size, (unsigned char *) &str, size);
    }

    return 0;
}

unsigned int cfg_erase(unsigned int start_addr, unsigned int end_addr)
{
    unsigned int flash_addr = start_addr;
    unsigned char buf[64];
    unsigned int size = 0;
    unsigned int lastLen = end_addr - start_addr;
    memset(buf, 0xFF, sizeof(buf));

    while (lastLen > 0)
    {
        if (lastLen > sizeof(buf))
        {
            size = sizeof(buf);
        }
        else
        {
            size = lastLen;
        }

        Flash_EEPROM_WriteBuffer(buf, flash_addr, size);
        lastLen -= size;
        flash_addr += size;
    }

    return 0;
}

void cfg_dump(unsigned int addr, unsigned int len)
{
    dumphex((unsigned char *)addr, len, __func__, __LINE__);
}

