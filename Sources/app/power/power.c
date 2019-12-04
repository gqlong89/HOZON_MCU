/****************************************************
 *          Power function file                     *
 *                             www.intest.com.cn    *
 ****************************************************/

#include <acc.h>
#include <adc.h>
#include <app/can_mgr/can_mgr.h>
#include <app/power/power.h>
#include <can_api.h>
#include <config.h>
#include <dbg_uart.h>
#include <flash.h>
#include <gpio.h>
#include <i2c.h>
#include <ipc_spi.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <pin.h>
#include <pm.h>
#include <port_hal.h>
#include <power_mode_switch.h>
#include <stdbool.h>
#include <S32K144.h>
#include <thres_diag_futian.h>
#include <timer.h>
#include <wdg.h>


enum
{
    OUTPUT_INVALID,
    OUTPUT_HIGH,
    OUTPUT_LOW
};

static int s_dc1 = OUTPUT_INVALID;
static int s_dc2 = OUTPUT_INVALID;
unsigned int WakeUpSource = 0;

/* Hardline wake */
unsigned int driverGetHardLineWakeSignals(void)
{
    unsigned int i = 0;

    i = (pm_get_s_charge_wakeup() ? POWERSTATUS_MASK_NCHARGE : 0);   /* A7, charge */
    i = i | (pm_get_acc_wakeup() ? POWERSTATUS_MASK_ACC : 0);     /* A1, ACC */
    i = i | (pm_get_on_wakeup() ? POWERSTATUS_MASK_ON : 0);       /*E12, ON */
    i = i | (pm_get_usb_wakeup() ? POWERSTATUS_MASK_USB : 0);     /*E13, USB */
    i = i | (pm_get_f_charge_wakeup() ? POWERSTATUS_MASK_FCHARGE : 0);   /* A5, fast charge */

    return i;
}

/* RTC wakeup */
unsigned int driverGetRTCWakeSignal(void)
{
    unsigned int i = 0;

    i = i | (pm_get_rtc_wakeup() ? POWERSTATUS_MASK_RTC : 0);
    return i;
}

/* Ring wakeup */
unsigned int driverGetRingWakeSignal(void)
{
    unsigned int i = 0;

    i = i | (pm_get_ring_wakeup() ? 0 : POWERSTATUS_MASK_RING);
    return i;
}

void driverSetWakeUpSrc(unsigned int wakesrc)
{
    WakeUpSource = wakesrc;
}

void driverInitWakeUpSrc(void)
{
    WakeUpSource = 0;

    if (pm_get_acc_wakeup())
    {
        WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_ACC);
    }

    if (pm_get_on_wakeup())
    {
        WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_ON);
    }

    if (pm_get_s_charge_wakeup())
    {
        WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_NCHARGE);
    }

    if (pm_get_f_charge_wakeup())
    {
        WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_FCHARGE);
    }

    cfg_save_standbyflag(STANDBY_NO_SLEEP);
    cfg_save_wake_cnt(0);
}

unsigned int driverGetWakeUpSrc(void)
{
    return WakeUpSource;
}

/* #1 DC out
 * on == 0 means output high voltage
 * on == 1 means output low voltage
 */
void driverDcOut1(unsigned int on)
{
    if (on)
    {
        pm_DC1_ctrl(1);
        s_dc1 = OUTPUT_LOW;
    }
    else
    {
        pm_DC1_ctrl(0);
        s_dc1 = OUTPUT_HIGH;
    }
}

/* #2 DC out
 * on == 0 means output high voltage
 * on == 1 means output low voltage
 */
void driverDcOut2(unsigned int on)
{
    if (on)
    {
        pm_DC2_ctrl(1);
        s_dc2 = OUTPUT_LOW;
    }
    else
    {
        pm_DC2_ctrl(0);
        s_dc2 = OUTPUT_HIGH;
    }
}

/* #1 DC out status */
unsigned int driverGetDcOut1Status(void)
{
    return (unsigned int)((s_dc1 == OUTPUT_HIGH) ? 0 : 1);
}

/* #2 DC out status */
unsigned int driverGetDcOut2Status(void)
{
    return (unsigned int)((s_dc2 == OUTPUT_HIGH) ? 0 : 1);
}

float get_powervoltage(void)
{
    return (0.00936 * adc_Abat() * VOLTAGE_SLOPE + VOLTAGE_COMPENSATION);
}

void PORTA1_ISR(void)
{
    WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_ON);
}

void PORTA12_ISR(void)
{
    WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_CAN2);
}

void PORTA15_ISR(void)
{
    WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_FCHARGE);
}

void PORTB9_ISR(void)
{
    WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_RING);
}

void PORTB15_ISR(void)
{
    WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_PM);
}


void PORTC7_ISR(void)
{
    WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_RTC);
}

void PORTC16_ISR(void)
{
    WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_CAN3);
}

void PORTD17_ISR(void)
{
    WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_RF);
}

void PORTE2_ISR(void)
{
    WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_NCHARGE);
}

void PORTE4_ISR(void)
{
    WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_CAN1);
}

void PORTE7_ISR(void)
{
    WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_G_SENSOR);
}

void PORTE12_ISR(void)
{
    WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_ACC);

    printk(PRINTK_LEVEL_LOG, " PORTE12_ISR ");
}

void PORTE13_ISR(void)
{
    WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_USB);
}

void PORTB17_ISR(void)
{
	WakeUpSource = (WakeUpSource | POWERSTATUS_MASK_BTN);
	printk(PRINTK_LEVEL_LOG, " PORTB17_ISR ");
}

void gpio_wakeup_init(void)
{
    GPIO_INT_CFG PORTA1;
    GPIO_INT_CFG PORTA12;
    GPIO_INT_CFG PORTA15;
    GPIO_INT_CFG PORTB9;
    GPIO_INT_CFG PORTB15;
    GPIO_INT_CFG PORTC7;
    GPIO_INT_CFG PORTC16;
    GPIO_INT_CFG PORTD17;
    GPIO_INT_CFG PORTE2;
    GPIO_INT_CFG PORTE4;
    GPIO_INT_CFG PORTE7;
    GPIO_INT_CFG PORTE12;
    GPIO_INT_CFG PORTE13;
	GPIO_INT_CFG PORTB17;

    /* ON */
    PORTA1.port_num = PORT_NUM_A;
    PORTA1.port_pin = 1;
    PORTA1.handler = (port_handler) PORTA1_ISR;
    PORTA1.port_cfg = PORT_INT_FALLING_EDGE;
    gpio_isr_init(&PORTA1);

    /* CAN1 */
    PORTA12.port_num = PORT_NUM_A;
    PORTA12.port_pin = 12;
    PORTA12.handler = (port_handler) PORTA12_ISR;
    PORTA12.port_cfg = PORT_INT_FALLING_EDGE;
    PORT_HAL_SetMuxModeSel(PORTA, 12, PORT_MUX_AS_GPIO);
    gpio_isr_init(&PORTA12);

    /* FAST charge */
    PORTA15.port_num = PORT_NUM_A;
    PORTA15.port_pin = 15;
    PORTA15.handler = (port_handler) PORTA15_ISR;
    PORTA15.port_cfg = PORT_INT_FALLING_EDGE;
    gpio_isr_init(&PORTA15);

    /* RING */
    PORTB9.port_num = PORT_NUM_B;
    PORTB9.port_pin = 9;
    PORTB9.handler = (port_handler) PORTB9_ISR;
    PORTB9.port_cfg = PORT_INT_FALLING_EDGE;
    gpio_isr_init(&PORTB9);

    /* PM */
    PORTB15.port_num = PORT_NUM_B;
    PORTB15.port_pin = 15;
    PORTB15.handler = (port_handler) PORTB15_ISR;
    PORTB15.port_cfg = PORT_INT_RISING_EDGE;
    gpio_isr_init(&PORTB15);

    /* RTC */
    PORTC7.port_num = PORT_NUM_C;
    PORTC7.port_pin = 7;
    PORTC7.handler = (port_handler) PORTC7_ISR;
    PORTC7.port_cfg = PORT_INT_FALLING_EDGE;
    gpio_isr_init(&PORTC7);

    /* CAN2 */
    PORTC16.port_num = PORT_NUM_C;
    PORTC16.port_pin = 16;
    PORTC16.handler = (port_handler) PORTC16_ISR;
    PORTC16.port_cfg = PORT_INT_FALLING_EDGE;
    PORT_HAL_SetMuxModeSel(PORTC, 16, PORT_MUX_AS_GPIO);
    gpio_isr_init(&PORTC16);

    /* RF */
    PORTD17.port_num = PORT_NUM_D;
    PORTD17.port_pin = 17;
    PORTD17.handler  = (port_handler) PORTD17_ISR;
    PORTD17.port_cfg = PORT_INT_FALLING_EDGE;
    gpio_isr_init(&PORTD17);

    /* SLOW charge */
    PORTE2.port_num = PORT_NUM_E;
    PORTE2.port_pin = 2;
    PORTE2.handler  = (port_handler) PORTE2_ISR;
    PORTE2.port_cfg = PORT_INT_FALLING_EDGE;
    gpio_isr_init(&PORTE2);

    /* CAN0 */
    PORTE4.port_num = PORT_NUM_E;
    PORTE4.port_pin = 4;
    PORTE4.handler = (port_handler) PORTE4_ISR;
    PORTE4.port_cfg = PORT_INT_FALLING_EDGE;
    PORT_HAL_SetMuxModeSel(PORTE, 4, PORT_MUX_AS_GPIO);
    gpio_isr_init(&PORTE4);

    /* G-SENSOR */
    if (cfg_get_anti_thief())
    {
        PORTE7.port_num = PORT_NUM_E;
        PORTE7.port_pin = 7;
        PORTE7.handler = (port_handler) PORTE7_ISR;
        PORTE7.port_cfg = PORT_INT_FALLING_EDGE;

        gpio_isr_init(&PORTE7);
    }

    /* ACC */
    PORTE12.port_num = PORT_NUM_E;
    PORTE12.port_pin = 12;
    PORTE12.handler  = (port_handler) PORTE12_ISR;
    PORTE12.port_cfg = PORT_INT_FALLING_EDGE;
    gpio_isr_init(&PORTE12);

    /* USB */
    PORTE13.port_num = PORT_NUM_E;
    PORTE13.port_pin = 13;
    PORTE13.handler  = (port_handler) PORTE13_ISR;
    PORTE13.port_cfg = PORT_INT_FALLING_EDGE;
    gpio_isr_init(&PORTE13);

	/* BTN_CON  BTN_CON必须接高电平触发,BTN_CON接高电平时，mos管导通,BTN_CON_MCU为低电平;
    BTN_CON接低电平时，mos管不导通，BTN_CON_MCU为高电平*/
    PORTB17.port_num = PORT_NUM_B;
    PORTB17.port_pin = 17;
    PORTB17.handler = (port_handler) PORTB17_ISR;
    PORTB17.port_cfg = PORT_INT_EITHER_EDGE;
    gpio_isr_init(&PORTB17);

}

void set_all_pin_disable(void)
{
    int i;

    for (i = 0; i < 18; i++)
    {
        //taskPrintf( TASK_LEVEL_LOG, "set_all_pin_disable PORTA: \t%d\r\n", i);
        if ((i != 1) && (i != 5) && (i != 12) && (i != 14) && (i != 15))
        {
            PORT_HAL_SetMuxModeSel(PORTA, i, PORT_PIN_DISABLED);
        }
    }

    for (i = 0; i < 18; i++)
    {
        //taskPrintf( TASK_LEVEL_LOG, "set_all_pin_disable PORTB: \t%d\r\n", i);
        if ((i != 9) && (i != 15)&& (i != 17))
        {
            PORT_HAL_SetMuxModeSel(PORTB, i, PORT_PIN_DISABLED);
        }
    }

    for (i = 0; i < 18; i++)
    {
        //taskPrintf( TASK_LEVEL_LOG, "set_all_pin_disable PORTC: \t%d\r\n", i);
        if ((i != 7) && (i != 16))
        {
            PORT_HAL_SetMuxModeSel(PORTC, i, PORT_PIN_DISABLED);
        }
    }

    for (i = 0; i < 18; i++)
    {
        //taskPrintf( TASK_LEVEL_LOG, "set_all_pin_disable PORTD: \t%d\r\n", i);
        if (GPIO_ReadPinInput(GP_EN2, GP_EN2_PIN_OUT) == 1)     /*Listen*/
        {
            if ((i != 10) && (i != 17))
            {
                PORT_HAL_SetMuxModeSel(PORTD, i, PORT_PIN_DISABLED);
            }
        }
        else                                                    /*sleep mode keep the PTD8 = 0*/
        {
            if ((i != 8) && (i != 10) && (i != 17))
            {
                PORT_HAL_SetMuxModeSel(PORTD, i, PORT_PIN_DISABLED);
            }
        }
    }

    for (i = 0; i < 17; i++)
    {
        //taskPrintf( TASK_LEVEL_LOG, "set_all_pin_disable PORTE: \t%d\r\n", i);
        if ((i != 2) && (i != 4) && (i != 7) && (i != 12)  && (i != 13))
        {
            PORT_HAL_SetMuxModeSel(PORTE, i, PORT_PIN_DISABLED);
        }
    }

    /* G-SENSOR */
    if (!cfg_get_anti_thief())
    {
        PORT_HAL_SetMuxModeSel(PORTE, 7, PORT_PIN_DISABLED);
    }
}

static void mcu_deinit(void)
{
    can_deinit_all();

    if (!cfg_get_anti_thief())
    {
        acc_standby();
    }
    else
    {
        acc_sleep();
    }

    timer_sleep();
    dbgUartwait();
}

void PowerInit(void)
{
    pm_init();
    pm_sys_on(1);
    power_mode_switch_init();

    driverInitWakeUpSrc();
}

static void Power_wakeup_init(unsigned char is_power_on_4G)
{
    pin_init();

    gpio_isr_clear();

    pm_gpio_init();
    timer_wakeup();

    pm_sys_on(is_power_on_4G);

    driverWDGFeed();

    can_wakeup_init();

    kernelMcumgrReinit();

    driverI2CInit();

    acc_device_init();

    ipc_wakeup_init();

    #if FUTIAN_DIAG_ENABLE
    set_spk_mic_test();
    #endif

    /*wake up time: 5ms*/

}

void PowerListen(void)
{
    WakeUpSource = 0;
    *((volatile unsigned int *) WAKEUPSOURCE_ADDR_START) = WakeUpSource;
    printk(PRINTK_LEVEL_LOG, " switch mode to VLPS");
    pm_4g_op_clear();
    pm_sys_off();
    mcu_deinit();
    gpio_isr_clear();
    gpio_wakeup_init();
    set_all_pin_disable();
    power_mode_switch_VLPS();
    *((volatile unsigned int *) WAKEUPSOURCE_ADDR_START) = WakeUpSource;
    Power_wakeup_init(0);
    printk(PRINTK_LEVEL_LOG, " switch Listen mode to WAKEUP");
}


void PowerSleep(void)
{
    WakeUpSource = 0;
    *((volatile unsigned int *) WAKEUPSOURCE_ADDR_START) = WakeUpSource;
    printk(PRINTK_LEVEL_LOG, " switch mode to VLPS");
    pm_4g_op_clear();
    pm_sys_off();
    pm_4g_power_ctrl(false);
    mcu_deinit();
    gpio_wakeup_init();
    set_all_pin_disable();
    power_mode_switch_VLPS();
    *((volatile unsigned int *) WAKEUPSOURCE_ADDR_START) = WakeUpSource;
    //driverSaveWakeUpSource();
    Power_wakeup_init(1);
    printk(PRINTK_LEVEL_LOG, " switch Sleep mode to WAKEUP");
}


