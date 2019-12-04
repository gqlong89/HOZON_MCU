/**
 * @Title: gpio.h
 * @author yuzhimin
 * @date Aug 8, 2017
 * @version V1.0
 */
#ifndef INCLUDE_GPIO_H_
#define INCLUDE_GPIO_H_

#include <port_hal.h>
#include <S32K144.h>

#define GP_4G_ON_OFF                PTC
#define GP_4G_ON_OFF_PIN_OUT        12U
#define GP_4G_RESET                 PTC
#define GP_4G_RESET_PIN_OUT         13U
#define GP_4G_STATUS                PTC
#define GP_4G_STATUS_PIN_IN         10U

#define GP_4G_MRDY_3V3              PTC         // 4G SPI-Mrdy
#define GP_4G_MRDY_PIN_IN           11U
#define GP_4G_SRDY_3V3              PTA         // 4G SPI-Srdy
#define GP_4G_SRDY_3V3_PIN_OUT      7U

#define GP_CAN_STB                  PTE
#define GP_CAN_STB_PIN_OUT          3U
#define GP_CAN4_nINT                PTA
#define GP_CAN4_nINT_PIN_IN         10U
#define GP_CAN4_RESET               PTA
#define GP_CAN4_RESET_PIN_OUT       11U
#define GP_CAN5_nINT                PTE
#define GP_CAN5_nINT_PIN_IN         10U
#define GP_CAN5_RESET               PTE
#define GP_CAN5_RESET_PIN_OUT       11U

#define GP_12V_CRL                  PTB         //  K-Lin
#define GP_12V_CRL_PIN_OUT          8U

#define GP_BAT_K                                PTA         // battery health monitor 
#define GP_BAT_K_PIN_OUT                        16U
#define GP_CPU_CHARGE_ON                        PTD         // battery charge on/off
#define GP_CPU_CHARGE_ON_PIN_OUT                12U
#define GP_CPU_CHARGE_TRICKLE                   PTD         //CPU-charge-trickle
#define GP_CPU_CHARGE_TRICKLE_PIN_OUT           16U
#define GP_CPU_CHARGE_TRICKLE_VARTA             PTD         //CPU-charge-trickle
#define GP_CPU_CHARGE_TRICKLE__VARTA_PIN_OUT    5U


#define GP_EN1                      PTE         // 5V Ctrl
#define GP_EN1_PIN_OUT              14U
#define GP_EN2                      PTD         // 4G Power Ctrl
#define GP_EN2_PIN_OUT              8U
#define GP_EN3                      PTB         // 3.3V_C Ctrl
#define GP_EN3_PIN_OUT              14U
#define GP_EN4                      PTD         // battery on/off
#define GP_EN4_PIN_OUT              10U
#define GP_EN7                      PTC         // Ethernet on/off
#define GP_EN7_PIN_OUT              5U
#define GP_EN8                      PTD         // VBUS on/off
#define GP_EN8_PIN_OUT              4U


#define GP_ACC_WKUP                 PTE  //PTA
#define GP_ACC_WKUP_PIN_IN          12U  //1U
#define GP_ON_WKUP                  PTA
#define GP_ON_WKUP_PIN_IN           1U
#define GP_USB_WKUP                 PTE
#define GP_USB_WKUP_PIN_IN          13U
#define GP_RING_WKUP                PTB
#define GP_RING_WKUP_PIN_IN         9U
#define GP_RTC_WKUP                 PTC
#define GP_RTC_WKUP_PIN_IN          7U
#define GP_S_CHARGE_WKUP            PTE
#define GP_S_CHARGE_WKUP_PIN_IN     2U
#define GP_F_CHARGE_WKUP            PTA
#define GP_F_CHARGE_WKUP_PIN_IN     15U
#define GP_G_WKUP                   PTE
#define GP_G_WKUP_PIN_IN            7U
#define GP_MP_WKUP                  PTB
#define GP_MP_WKUP_PIN_IN           15U


#define GP_K1                       PTD         // IO1
#define GP_K1_PIN_OUT               13U
#define GP_K2                       PTD         // IO2
#define GP_K2_PIN_OUT               15U

/*
#define GP_K3                       PTD         // I_Indicator  IN78200048P0.2 此引脚被充电trickle使用
#define GP_K3_PIN_OUT               16U
*/

#define GP_K4                       PTE         // B_Indicator
#define GP_K4_PIN_OUT               9U
#define GP_K5                       PTD         // E_Indicator
#define GP_K5_PIN_OUT               14U

#define GP_Offsef_EN                PTD         // MIC Offset on/off
#define GP_Offsef_EN_PIN_OUT        9U
#define GP_PA_EN                    PTA         // power amplifier on/off
#define GP_PA_EN_PIN_OUT            0U
#define GP_LS_TEST_EN1              PTB         //SPK+ TEST_EN1 Cindy
#define GP_LS_TEST_EN1_PIN_OUT      10U
#define GP_LS_TEST_EN2              PTB         //SPK- TEST_EN2 Cindy
#define GP_LS_TEST_EN2_PIN_OUT      11U

#if 0
    #define GP_SOS_ONOFF1               PTB         // SOS+
    #define GP_SOS_ONOFF1_PIN_IN        11U
    #define GP_SOS_ONOFF2               PTB         // SOS-
    #define GP_SOS_ONOFF2_PIN_IN        10U
#endif

#define GP_SLEEP_CTR                PTA         // SLEEP CONTROL
#define GP_SLEEP_CTR_PIN_OUT        14U

#define GP_ACC_G_RST                PTA         // ACC RESET
#define GP_ACC_G_RST_PIN_OUT        17U

#define GP_ECALL                    PTD
#define GP_ECALL_PIN_IN             5U
#define GP_ICALL                    PTD
#define GP_ICALL_PIN_IN             6U
#define GP_BCALL                    PTD
#define GP_BCALL_PIN_IN             7U

#define GP_WAKEUP_EC20              PTE
#define GP_WAKEUP_EC20_PIN_OUT      6U

/*can flex rx port*/
#define RD1_PORT                    PTE
#define RD1_PORT_PIN                4U
#define RD2_PORT                    PTA
#define RD2_PORT_PIN                12U
#define RD3_PORT                    PTC
#define RD3_PORT_PIN                16U

/*i2c pin*/
#define I2C_SDA_PORT                    PTA
#define I2C_SDA_PORT_PIN                2U
#define I2C_SCL_PORT                    PTA
#define I2C_SCL_PORT_PIN                3U


#if 0
    #define GP_TPS_PS                   PTE
    #define GP_TPS_PS_PIN_OUT           8U
#endif

typedef enum
{
    PORT_NUM_A = 0,
    PORT_NUM_B,
    PORT_NUM_C,
    PORT_NUM_D,
    PORT_NUM_E,
    PORT_NUM_MAX,
} PORT_NUM_TYPE;

typedef enum
{
    GPIO_IN_CONF = 0,
    GPIO_OUT_LOW_CONF,
    GPIO_OUT_HI_CONF,
} gpio_io_cfg_t;

typedef struct
{
    GPIO_Type *base;
    unsigned int pin;
    gpio_io_cfg_t io_cfg;
} GPIO_IO_CFG;

typedef void (*port_handler)(unsigned int pin);

typedef struct
{
    PORT_NUM_TYPE port_num;
    port_interrupt_config_t port_cfg;
    unsigned int port_pin;
    port_handler handler;
} GPIO_INT_CFG;

void gpio_io_init(GPIO_IO_CFG *cfg);
unsigned int GPIO_ReadPinInput(GPIO_Type *base, unsigned int pin);
void gpio_isr_init(GPIO_INT_CFG *cfg);
void gpio_isr_deinit(PORT_NUM_TYPE port_num, unsigned int pin);
void gpio_isr_clear(void);

#endif /* INCLUDE_GPIO_H_ */

