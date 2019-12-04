#include <app/scom/scom_tl_ex.h>
#include <call.h>
#include <config.h>
#include <osif.h>
#include <pm.h>
#include <stdbool.h>

#define CALL_TRIGGER_TIMEOUT    (120*100)     //120S

static uint64_t ecall_trigger_timeout;
static uint64_t bcall_trigger_timeout;
static uint64_t icall_trigger_timeout;

unsigned int call_get_btn_status(void)
{
    return 0;
}

unsigned int call_get_status(void)
{
    return (scom_get_ecall_status() | scom_get_bcall_status() | scom_get_icall_status() |
            call_get_btn_status());
}

void call_set_tigger_timeout(void)
{
    ecall_trigger_timeout = driverGetUptime();
    bcall_trigger_timeout = driverGetUptime();
    icall_trigger_timeout = driverGetUptime();
}

static unsigned int call_get_ecall_trigger_timeout(void)
{
    if ((driverGetUptime() - ecall_trigger_timeout) > CALL_TRIGGER_TIMEOUT)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static unsigned int call_get_bcall_trigger_timeout(void)
{
    if ((driverGetUptime() - bcall_trigger_timeout) > CALL_TRIGGER_TIMEOUT)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static unsigned int call_get_icall_trigger_timeout(void)
{
    if ((driverGetUptime() - icall_trigger_timeout) > CALL_TRIGGER_TIMEOUT)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

unsigned int call_get_trigger_timeout(void)
{
    unsigned int ret = 0;

    if (call_get_status())
    {
        call_set_tigger_timeout();
        return 1;
    }

    if (cfg_get_ecall())
    {
        if (!call_get_ecall_trigger_timeout())
        {
            ret = 1;
        }
    }

    if (cfg_get_bcall())
    {
        if (!call_get_bcall_trigger_timeout())
        {
            ret = 1;
        }
    }

    if (cfg_get_icall())
    {
        if (!call_get_icall_trigger_timeout())
        {
            ret = 1;
        }
    }


    return ret;
}

void call_button_poll(void)
{
    static unsigned int call_button_flag = 0;
    unsigned int call_button_status = 0;

    if (pm_get_ICALL())
    {
        //        pm_I_Indicator_ctrl(true);
        //        call_button_status = 1;
    }
    else
    {
        //        pm_I_Indicator_ctrl(false);
    }

    if (pm_get_BCALL())
    {
        pm_B_Indicator_ctrl(true);
        call_button_status = 1;
    }
    else
    {
        pm_B_Indicator_ctrl(false);
    }

    if (pm_get_ECALL())
    {
        pm_E_Indicator_ctrl(true);
        call_button_status = 1;
    }
    else
    {
        pm_E_Indicator_ctrl(false);
    }

    if (call_button_status)
    {
        if (call_button_flag == 0)
        {
            call_button_flag = 1;
            scom_tl_ex_mcu_status_syn();
        }
    }
    else
    {
        call_button_flag = 0;
    }
}
