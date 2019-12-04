#include <acc.h>
#include <config.h>
#include <i2c.h>
#include <mid/acc/fxls_acc.h>
#include <mid/acc/icm_acc.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>

static volatile unsigned int acc_init_cnt = 0;
static ACC_TYPE acc_type = ACC_UNKNOWN;
char accstatus = ACC_R_UNKNOW;

int acc_device_init(void)
{
    int ret = 0;

    if (acc_init_cnt > 3)
    {
        driverI2CInit();
        acc_init_cnt = 0;
        printk(PRINTK_LEVEL_ERROR, "REINIT I2C DRIVER");
    }

    if (is_icm_acc())
    {
        if (icm_acc_init())
        {
            acc_init_cnt = 0;
        }
        else
        {
            ret = -1;
            acc_init_cnt++;
        }

        acc_type = ACC_ICM20600;
    }
    else if (is_fxls_acc())
    {
        if (fxls_acc_init())
        {
            acc_init_cnt = 0;
        }
        else
        {
            acc_init_cnt++;
            ret = -1;
        }

        acc_type = ACC_FXLS8471Q;
    }
    else
    {
        acc_type = ACC_UNKNOWN;
        acc_init_cnt++;
        ret = -1;
        printk(PRINTK_LEVEL_ERROR, "unknow acc type");
    }

    return ret;

}

void acc_standby(void)
{
    if (ACC_ICM20600 == acc_type)
    {
        icm_standby();
    }
    else if (ACC_FXLS8471Q == acc_type)
    {
        fxls_standby();
    }
    else
    {

    }
}

void acc_sleep(void)
{
    if (ACC_ICM20600 == acc_type)
    {
        icm_sleep();
    }
    else if (ACC_FXLS8471Q == acc_type)
    {
        fxls_sleep();
    }
    else
    {

    }
}

void acc_check(void)
{
    double angle[3];
    int ret;
    static unsigned char anti_thief_enable_bak;
    static int err_cnt = 0;

    if (anti_thief_enable_bak != cfg_get_anti_thief())
    {
        anti_thief_enable_bak = cfg_get_anti_thief();
        ret = acc_device_init();
    }

    if (ACC_ICM20600 == acc_type)
    {
        ret = icm_get_xyz(angle);
    }
    else if (ACC_FXLS8471Q == acc_type)
    {
        fxls_get_interrupt_src();
        ret = fxls_get_xyz(angle);
    }
    else
    {
        ret = acc_device_init();
    }

    if (ret < 0)
    {
        err_cnt++;
        accstatus = ACC_R_ERROR;
    }
    else
    {
        err_cnt = 0;
        accstatus = ACC_R_OK;
    }

    if (err_cnt > 3)
    {
        printk(PRINTK_LEVEL_ERROR, "read error max, reinit acc");
        acc_type = ACC_UNKNOWN;
        err_cnt = 0;
    }
}

