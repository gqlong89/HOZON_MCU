#include <gpio.h>
#include <gpio_hal.h>
#include <stdbool.h>
#include <timer.h>
#include "pm.h"

/* 4G operation type */
enum
{
    PM_4G_NO_OP = 0,
    PM_4G_RESET_OP,
    PM_4G_SAFE_ON_OP,
    PM_4G_SAFE_OFF_OP,
};

/* peripherals  operation type */
enum
{
    PM_NO_OP = 0,
    PM_ACC_RESET_OP,
};


#define PM_4G_SAFE_OFF_HOLD         (700)       //700ms (>=650ms)
#define PM_4G_SAFE_ON_HOLD          (150)       //150ms (>=100ms)
#define PM_4G_RESET_HOLD            (200)       //200ms (150~460ms)

#define PM_ACC_RESET_HOLD           (100)       //100us (no ref)

static int pm_timer_handle = -1;
static int pm_4g_timer_handle = -1;

static int pm_4g_op = PM_4G_NO_OP;
static int pm_op = PM_NO_OP;

static int virtual_on = 0;//Liu Binkui added for NOVAT,2019/04/06

GPIO_IO_CFG pm_gpio[] =
{
    {GP_EN1, GP_EN1_PIN_OUT, GPIO_OUT_HI_CONF },
    {GP_EN2, GP_EN2_PIN_OUT, GPIO_OUT_HI_CONF },
    {GP_EN3, GP_EN3_PIN_OUT, GPIO_OUT_HI_CONF },
    {GP_EN4, GP_EN4_PIN_OUT, GPIO_OUT_HI_CONF },
    {GP_EN7, GP_EN7_PIN_OUT, GPIO_OUT_HI_CONF },
    {GP_EN8, GP_EN8_PIN_OUT, GPIO_OUT_HI_CONF },

    {GP_S_CHARGE_WKUP, GP_S_CHARGE_WKUP_PIN_IN, GPIO_IN_CONF },
    {GP_F_CHARGE_WKUP, GP_F_CHARGE_WKUP_PIN_IN, GPIO_IN_CONF },
    {GP_ACC_WKUP, GP_ACC_WKUP_PIN_IN, GPIO_IN_CONF },

    {GP_ON_WKUP, GP_ON_WKUP_PIN_IN, GPIO_IN_CONF },
    {GP_USB_WKUP, GP_USB_WKUP_PIN_IN, GPIO_IN_CONF },

    {GP_RING_WKUP, GP_RING_WKUP_PIN_IN, GPIO_IN_CONF },
    {GP_RTC_WKUP, GP_RTC_WKUP_PIN_IN, GPIO_IN_CONF },
    {GP_G_WKUP, GP_G_WKUP_PIN_IN, GPIO_IN_CONF },
    {GP_MP_WKUP, GP_MP_WKUP_PIN_IN, GPIO_IN_CONF },

    {GP_4G_STATUS, GP_4G_STATUS_PIN_IN, GPIO_IN_CONF },
    {GP_4G_ON_OFF, GP_4G_ON_OFF_PIN_OUT, GPIO_OUT_LOW_CONF },
    {GP_4G_RESET, GP_4G_RESET_PIN_OUT, GPIO_OUT_LOW_CONF },
    {GP_12V_CRL, GP_12V_CRL_PIN_OUT, GPIO_OUT_HI_CONF },
    {GP_Offsef_EN, GP_Offsef_EN_PIN_OUT, GPIO_OUT_HI_CONF },
    {GP_PA_EN, GP_PA_EN_PIN_OUT, GPIO_OUT_LOW_CONF },

    {GP_CAN_STB, GP_CAN_STB_PIN_OUT, GPIO_OUT_LOW_CONF },
    {GP_BAT_K, GP_BAT_K_PIN_OUT, GPIO_OUT_LOW_CONF },
    {GP_CPU_CHARGE_ON, GP_CPU_CHARGE_ON_PIN_OUT, GPIO_OUT_LOW_CONF },
    {GP_CPU_CHARGE_TRICKLE, GP_CPU_CHARGE_TRICKLE_PIN_OUT, GPIO_OUT_LOW_CONF},
    {GP_CPU_CHARGE_TRICKLE_VARTA, GP_CPU_CHARGE_TRICKLE__VARTA_PIN_OUT, GPIO_OUT_LOW_CONF}, //just for FT CPU-charge-trickle-VARTA

    {GP_K1, GP_K1_PIN_OUT, GPIO_OUT_LOW_CONF },
    {GP_K2, GP_K2_PIN_OUT, GPIO_OUT_LOW_CONF },
    //    {GP_K3, GP_K3_PIN_OUT, GPIO_OUT_LOW_CONF },
    {GP_K4, GP_K4_PIN_OUT, GPIO_OUT_LOW_CONF },// ECALL_IL
    {GP_K5, GP_K5_PIN_OUT, GPIO_OUT_LOW_CONF },// BCALL_IL

    {GP_SLEEP_CTR, GP_SLEEP_CTR_PIN_OUT, GPIO_OUT_HI_CONF },
    {GP_ACC_G_RST, GP_ACC_G_RST_PIN_OUT, GPIO_OUT_LOW_CONF },
    {GP_WAKEUP_EC20, GP_WAKEUP_EC20_PIN_OUT, GPIO_OUT_LOW_CONF },

    #if 0
    {GP_ECALL, GP_ECALL_PIN_IN, GPIO_IN_CONF },
    {GP_ICALL, GP_ICALL_PIN_IN, GPIO_IN_CONF },
    {GP_BCALL, GP_BCALL_PIN_IN, GPIO_IN_CONF },
    #endif

};

void pm_set_virtual_on(int on)
{
    if (on < 0 || on > 1)
    {
        return;
    }

    virtual_on = on;
}

/**
* return:
*       ture: virtual_on is exist,can not sleep
*       fale: virtual_on is not exist,can sleep
* Liu Binkui added for NOVAT,2019/04/06
*/
bool pm_get_virtual_on_status(void)
{
    return virtual_on ? true : false;
}

void pm_gpio_init(void)
{
    unsigned int i;

    for (i = 0; i < sizeof(pm_gpio) / sizeof(pm_gpio[0]); i++)
    {
        gpio_io_init(&pm_gpio[i]);
    }
}

void pm_4g_timeout_handler(int handle)
{
    if (handle != pm_4g_timer_handle)
    {
        return;
    }

    ltimer_stop(handle);

    switch (pm_4g_op)
    {
        case PM_4G_RESET_OP:
            GPIO_HAL_WritePin(GP_4G_RESET, GP_4G_RESET_PIN_OUT, 0U);
            pm_4g_op = PM_4G_NO_OP;
            break;

        case PM_4G_SAFE_ON_OP:
            GPIO_HAL_WritePin(GP_4G_ON_OFF, GP_4G_ON_OFF_PIN_OUT, 0U);
            pm_4g_op = PM_4G_NO_OP;
            break;

        case PM_4G_SAFE_OFF_OP:
            GPIO_HAL_WritePin(GP_4G_ON_OFF, GP_4G_ON_OFF_PIN_OUT, 0U);
            pm_4g_op = PM_4G_NO_OP;
            break;

        default:
            pm_4g_op = PM_4G_NO_OP;
            break;
    }
}

void pm_timeout_handler(int handle)
{
    if (handle != pm_timer_handle)
    {
        return;
    }

    htimer_stop(handle);

    switch (pm_op)
    {
        case PM_ACC_RESET_OP:
            GPIO_HAL_WritePin(GP_ACC_G_RST, GP_ACC_G_RST_PIN_OUT, 0U);
            pm_op = PM_NO_OP;
            break;

        default:
            pm_op = PM_NO_OP;
            break;
    }
}


void pm_init(void)
{
    pm_gpio_init();
    pm_4g_timer_handle = ltimer_create((timeout_handler) pm_4g_timeout_handler);
    pm_timer_handle = htimer_create((timeout_handler) pm_timeout_handler);
}

void pm_sys_on(unsigned char is_power_on_4G)
{
    pm_5v_ctrl(true);
    pm_can_stb_ctrl(true);
    pm_vsc_ctrl(true);
    pm_4g_power_ctrl(true);
    pm_3v3c_ctrl(true);
    pm_12v_ctrl(true);

    if (is_power_on_4G)
    {
        pm_4g_safe_on();
    }

    pm_wakeup_ec20_ctrl(true);
}

void pm_sys_off(void)
{
    pm_5v_ctrl(false);
    pm_3v3c_ctrl(false);
    pm_12v_ctrl(false);
    pm_offset_ctrl(false);
    pm_wakeup_ec20_ctrl(false);
}

void pm_5v_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_EN1, GP_EN1_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_EN1, GP_EN1_PIN_OUT, 0U);
    }
}

void pm_12v_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_12V_CRL, GP_12V_CRL_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_12V_CRL, GP_12V_CRL_PIN_OUT, 0U);
    }
}

void pm_3v3c_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_EN3, GP_EN3_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_EN3, GP_EN3_PIN_OUT, 0U);
    }
}

void pm_4g_power_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_EN2, GP_EN2_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_EN2, GP_EN2_PIN_OUT, 0U);
    }
}

bool pm_get_4g_status(void)
{
    if (!GPIO_ReadPinInput(GP_4G_STATUS, GP_4G_STATUS_PIN_IN))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void pm_4g_safe_on(void)
{
    //if( !pm_get_4g_status() && PM_4G_NO_OP == pm_4g_op )
    if (PM_4G_NO_OP == pm_4g_op)
    {
        GPIO_HAL_WritePin(GP_4G_ON_OFF, GP_4G_ON_OFF_PIN_OUT, 1U);
        ltimer_start(pm_4g_timer_handle, PM_4G_SAFE_ON_HOLD);
        pm_4g_op = PM_4G_SAFE_ON_OP;
    }
}

void pm_4g_safe_off(void)
{
    //if( pm_get_4g_status() && PM_4G_NO_OP == pm_4g_op )
    if (PM_4G_NO_OP == pm_4g_op)
    {
        GPIO_HAL_WritePin(GP_4G_ON_OFF, GP_4G_ON_OFF_PIN_OUT, 1U);
        ltimer_start(pm_4g_timer_handle, PM_4G_SAFE_OFF_HOLD);
        pm_4g_op = PM_4G_SAFE_OFF_OP;
    }
}

void pm_4g_reset(void)
{
    //if( pm_get_4g_status() && PM_4G_NO_OP == pm_4g_op )
    if (PM_4G_NO_OP == pm_4g_op)
    {
        GPIO_HAL_WritePin(GP_4G_RESET, GP_4G_RESET_PIN_OUT, 1U);
        ltimer_start(pm_4g_timer_handle, PM_4G_RESET_HOLD);
        pm_4g_op = PM_4G_RESET_OP;
    }
}

void pm_4g_op_clear(void)
{
    ltimer_stop(pm_4g_timer_handle);
    pm_4g_op = PM_4G_NO_OP;
}

void pm_acc_reset(void)
{
    if (PM_NO_OP == pm_op)
    {
        GPIO_HAL_WritePin(GP_ACC_G_RST, GP_ACC_G_RST_PIN_OUT, 1U);
        htimer_start(pm_timer_handle, PM_ACC_RESET_HOLD);
        pm_op = PM_ACC_RESET_OP;
    }
}


void pm_can_stb_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_CAN_STB, GP_CAN_STB_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_CAN_STB, GP_CAN_STB_PIN_OUT, 0U);
    }
}

void pm_bat_k_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_BAT_K, GP_BAT_K_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_BAT_K, GP_BAT_K_PIN_OUT, 0U);
    }
}

void pm_cpu_charge_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_CPU_CHARGE_ON, GP_CPU_CHARGE_ON_PIN_OUT, 1U);
        //        printf("GP_CPU_CHARGE_TRICKLE = 0x%x\r\n", GPIO_HAL_ReadPins(GP_CPU_CHARGE_TRICKLE) & 0x1000); //for test;
    }
    else
    {
        GPIO_HAL_WritePin(GP_CPU_CHARGE_ON, GP_CPU_CHARGE_ON_PIN_OUT, 0U);
    }
}

void pm_cpu_charge_trickle_ctrl(bool enable)            //GP_CPU_CHARGE_TRICKLE Switch : Cindy
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_CPU_CHARGE_TRICKLE, GP_CPU_CHARGE_TRICKLE_PIN_OUT, 1U);
        //        printf("GP_CPU_CHARGE_TRICKLE = 0x%x\r\n", GPIO_HAL_ReadPins(GP_CPU_CHARGE_TRICKLE) & 0x10000); //for test;
    }
    else
    {
        GPIO_HAL_WritePin(GP_CPU_CHARGE_TRICKLE, GP_CPU_CHARGE_TRICKLE_PIN_OUT, 0U);
    }
}

void pm_cpu_charge_trickle_varta_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_CPU_CHARGE_TRICKLE_VARTA, GP_CPU_CHARGE_TRICKLE__VARTA_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_CPU_CHARGE_TRICKLE_VARTA, GP_CPU_CHARGE_TRICKLE__VARTA_PIN_OUT, 0U);
    }
}


void pm_vsc_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_EN4, GP_EN4_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_EN4, GP_EN4_PIN_OUT, 0U);
    }
}

void pm_ethernet_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_EN7, GP_EN7_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_EN7, GP_EN7_PIN_OUT, 0U);
    }
}

void pm_vbus_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_EN8, GP_EN8_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_EN8, GP_EN8_PIN_OUT, 0U);
    }
}


void pm_offset_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_Offsef_EN, GP_Offsef_EN_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_Offsef_EN, GP_Offsef_EN_PIN_OUT, 0U);
    }
}

void pm_ap_en_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_PA_EN, GP_PA_EN_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_PA_EN, GP_PA_EN_PIN_OUT, 0U);
    }
}

bool pm_get_s_charge_wakeup(void)
{
    if (!GPIO_ReadPinInput(GP_S_CHARGE_WKUP, GP_S_CHARGE_WKUP_PIN_IN))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool pm_get_f_charge_wakeup(void)
{
    if (!GPIO_ReadPinInput(GP_F_CHARGE_WKUP, GP_F_CHARGE_WKUP_PIN_IN))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool pm_get_acc_wakeup(void)
{
    if (!GPIO_ReadPinInput(GP_ACC_WKUP, GP_ACC_WKUP_PIN_IN))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool pm_get_on_wakeup(void)
{
    if (!GPIO_ReadPinInput(GP_ON_WKUP, GP_ON_WKUP_PIN_IN))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool pm_get_usb_wakeup(void)
{
    if (!GPIO_ReadPinInput(GP_USB_WKUP, GP_USB_WKUP_PIN_IN))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool pm_get_ring_wakeup(void)
{
    if (!GPIO_ReadPinInput(GP_RING_WKUP, GP_RING_WKUP_PIN_IN))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool pm_get_rtc_wakeup(void)
{
    if (!GPIO_ReadPinInput(GP_RTC_WKUP, GP_RTC_WKUP_PIN_IN))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool pm_get_gsensor_wakeup(void)
{
    if (!GPIO_ReadPinInput(GP_G_WKUP, GP_G_WKUP_PIN_IN))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool pm_get_pm_wakeup(void)
{
    if (GPIO_ReadPinInput(GP_MP_WKUP, GP_MP_WKUP_PIN_IN))
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool pm_get_ICALL(void)
{
    #if 0

    if (!GPIO_ReadPinInput(GP_ICALL, GP_ICALL_PIN_IN))
    {
        return true;
    }
    else
    {
        return false;
    }

    #endif
    return false;
}

bool pm_get_BCALL(void)
{
    #if 0

    if (!GPIO_ReadPinInput(GP_BCALL, GP_BCALL_PIN_IN))
    {
        return true;
    }
    else
    {
        return false;
    }

    #endif
    return false;
}

bool pm_get_ECALL(void)
{
    #if 0

    if (!GPIO_ReadPinInput(GP_ECALL, GP_ECALL_PIN_IN))
    {
        return true;
    }
    else
    {
        return false;
    }

    #endif
    return false;
}

bool pm_get_usb_status(void)
{
    if (!GPIO_ReadPinInput(GP_USB_WKUP, GP_USB_WKUP_PIN_IN))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void pm_DC1_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_K1, GP_K1_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_K1, GP_K1_PIN_OUT, 0U);
    }
}

void pm_DC2_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_K2, GP_K2_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_K2, GP_K2_PIN_OUT, 0U);
    }
}

void pm_I_Indicator_ctrl(bool enable)
{
    if (enable)
    {
        //        GPIO_HAL_WritePin(GP_K3, GP_K3_PIN_OUT, 1U);
    }
    else
    {
        //        GPIO_HAL_WritePin(GP_K3, GP_K3_PIN_OUT, 0U);
    }
}

void pm_B_Indicator_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_K4, GP_K4_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_K4, GP_K4_PIN_OUT, 0U);
    }
}

void pm_E_Indicator_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_K5, GP_K5_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_K5, GP_K5_PIN_OUT, 0U);
    }
}

void pm_sleep_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_SLEEP_CTR, GP_SLEEP_CTR_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_SLEEP_CTR, GP_SLEEP_CTR_PIN_OUT, 0U);
    }
}

void pm_wakeup_ec20_ctrl(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_WAKEUP_EC20, GP_WAKEUP_EC20_PIN_OUT, 1U);
    }
    else
    {
        GPIO_HAL_WritePin(GP_WAKEUP_EC20, GP_WAKEUP_EC20_PIN_OUT, 0U);
    }
}


void pm_off_mode(void)
{
    //pm_sys_off();
    //pm_4g_power_ctrl( false );

    /* stop power sullpy by battery*/
    pm_vsc_ctrl(false);

    /* stop power sullpy by KL30 */
    pm_sleep_ctrl(false);
}

