/**
* @Title: pin.c
* @author yuzhimin
* @date Jan 18, 2018
* @version V1.0
*/

#include <pins_driver.h>
#include <port_hal.h>
#include <stdbool.h>
#include <S32K144.h>


static void porta_init_pins(void)
{
    pin_settings_config_t porta[] =
    {
        {
            .base          = PORTA,
            .pinPortIdx    = 0u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTA,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // PA_EN
        {
            .base          = PORTA,
            .pinPortIdx    = 1u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_INT_FALLING_EDGE,
            .clearIntFlag  = false,
            .gpioBase      = PTA,
            .direction     = GPIO_INPUT_DIRECTION,
        }, // ACC_WKUP
        {
            .base          = PORTA,
            .pinPortIdx    = 2u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT3,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // I2C_SDA
        {
            .base          = PORTA,
            .pinPortIdx    = 3u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT3,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // I2C_SCL
        {
            .base          = PORTA,
            .pinPortIdx    = 4u,
            .pullConfig    = PORT_INTERNAL_PULL_UP_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT7,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // SWDIO
        {
            .base          = PORTA,
            .pinPortIdx    = 5u,
            .pullConfig    = PORT_INTERNAL_PULL_UP_ENABLED,
            .passiveFilter = true,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT7,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // RESET_CPU
        {
            .base          = PORTA,
            .pinPortIdx    = 6u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT5,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // ADC_TEMP
        {
            .base          = PORTA,
            .pinPortIdx    = 7u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTA,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // 4G_SRDY_3V3
        {
            .base          = PORTA,
            .pinPortIdx    = 8u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // TP138

        {
            .base          = PORTA,
            .pinPortIdx    = 9u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // TP355


        {
            .base          = PORTA,
            .pinPortIdx    = 10u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,//PORT_HIGH_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTA,
            .direction     = GPIO_INPUT_DIRECTION,
        }, // CAN4_nINT
        {
            .base          = PORTA,
            .pinPortIdx    = 11u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTA,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // CAN4_RESET
        {
            .base          = PORTA,
            .pinPortIdx    = 12u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT3,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // RD2
        {
            .base          = PORTA,
            .pinPortIdx    = 13u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT3,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // TD2
        {
            .base          = PORTA,
            .pinPortIdx    = 14u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTA,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // Sleep_Ctr
        {
            .base          = PORTA,
            .pinPortIdx    = 15u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTA,
            .direction     = GPIO_INPUT_DIRECTION,
        }, // F_CHARGE_WKUP
        {
            .base          = PORTA,
            .pinPortIdx    = 16u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTA,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // BAT_K

        {
            .base          = PORTA,
            .pinPortIdx    = 17u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTA,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // G_RST
    };
    PINS_DRV_Init(sizeof(porta) / sizeof(porta[0]), porta);
}
static void portb_init_pins(void)
{
    pin_settings_config_t portb[] =
    {
        {
            .base          = PORTB,
            .pinPortIdx    = 0u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT3,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // 4G_CS_3V3

        {
            .base          = PORTB,
            .pinPortIdx    = 1u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT3,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // 4G_MISO_3V3
        {
            .base          = PORTB,
            .pinPortIdx    = 2u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT3,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // 4G_SCK_3V3

        {
            .base          = PORTB,
            .pinPortIdx    = 3u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT3,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // 4G_MOSI_3V3

        {
            .base          = PORTB,
            .pinPortIdx    = 4u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT2,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // SRS_IN_MCU
        {
            .base          = PORTB,
            .pinPortIdx    = 5u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT2,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // SRS_IN_MCU(NC)

        {
            .base          = PORTB,
            .pinPortIdx    = 6u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        },  // PTB6 --- XTAL
        {
            .base          = PORTB,
            .pinPortIdx    = 7u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // PTB7 --- EXTAL
        {
            .base          = PORTB,
            .pinPortIdx    = 8u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTB,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // 12V_CRL
        {
            .base          = PORTB,
            .pinPortIdx    = 9u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTB,
            .direction     = GPIO_INPUT_DIRECTION,
        }, // RING_WKUP
        {
            .base          = PORTB,
            .pinPortIdx    = 10u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTB,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // LS_TEST_EN1

        {
            .base          = PORTB,
            .pinPortIdx    = 11u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTB,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // LS_TEST_EN2

        {
            .base          = PORTB,
            .pinPortIdx    = 12u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // ADC_VMIC
        {
            .base          = PORTB,
            .pinPortIdx    = 13u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // ADC_MIC+
        {
            .base          = PORTB,
            .pinPortIdx    = 14u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTB,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // EN3
        {
            .base          = PORTB,
            .pinPortIdx    = 15u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTB,
            .direction     = GPIO_INPUT_DIRECTION,
        }, // MP_WKUP
        {
            .base          = PORTB,
            .pinPortIdx    = 16u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // ADC_L-
        {
            .base          = PORTB,
            .pinPortIdx    = 17u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTB,
            .direction     = GPIO_INPUT_DIRECTION,
        }, // BTN_CON_MCU
    };
    PINS_DRV_Init(sizeof(portb) / sizeof(portb[0]), portb);
}

static void portc_init_pins(void)
{
    pin_settings_config_t portc[] =
    {
        {
            .base          = PORTC,
            .pinPortIdx    = 0u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // ADC_BAT
        {
            .base          = PORTC,
            .pinPortIdx    = 1u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // Bbat

        {
            .base          = PORTC,
            .pinPortIdx    = 2u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT4,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // UART0_RXD
        {
            .base          = PORTC,
            .pinPortIdx    = 3u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT4,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // UART0_TXD
        {
            .base          = PORTC,
            .pinPortIdx    = 4u,
            .pullConfig    = PORT_INTERNAL_PULL_DOWN_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT7,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // SWDCLK
        {
            .base          = PORTC,
            .pinPortIdx    = 5u,
            .pullConfig    = PORT_INTERNAL_PULL_UP_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTC,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // EN7
        {
            .base          = PORTC,
            .pinPortIdx    = 6u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // ADC_L+

        {
            .base          = PORTC,
            .pinPortIdx    = 7u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTC,
            .direction     = GPIO_INPUT_DIRECTION,
        }, // RTC_WKUP
        {
            .base          = PORTC,
            .pinPortIdx    = 8u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // 4G_TX2_3V3 (not used)

        {
            .base          = PORTC,
            .pinPortIdx    = 9u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // 4G_RX2_3V3 (not used)


        {
            .base          = PORTC,
            .pinPortIdx    = 10u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTC,
            .direction     = GPIO_INPUT_DIRECTION,
        }, // 4G_STATUS
        {
            .base          = PORTC,
            .pinPortIdx    = 11u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // 4G_MRDY_3V3(not used)

        {
            .base          = PORTC,
            .pinPortIdx    = 12u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTC,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // 4G_ON_OFF
        {
            .base          = PORTC,
            .pinPortIdx    = 13u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTC,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // 4G_RESET
        {
            .base          = PORTC,
            .pinPortIdx    = 14u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // ADC_GPS2
        {
            .base          = PORTC,
            .pinPortIdx    = 15u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // ADC_GPS1
        {
            .base          = PORTC,
            .pinPortIdx    = 16u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT3,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // RD3
        {
            .base          = PORTC,
            .pinPortIdx    = 17u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT3,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // TD3
    };
    PINS_DRV_Init(sizeof(portc) / sizeof(portc[0]), portc);
}

static void portd_init_pins(void)
{
    pin_settings_config_t portd[] =
    {
        {
            .base          = PORTD,
            .pinPortIdx    = 0u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT3,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // CAN4_CLK

        {
            .base          = PORTD,
            .pinPortIdx    = 1u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT3,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // CAN4_SIN
        {
            .base          = PORTD,
            .pinPortIdx    = 2u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // Abat
        {
            .base          = PORTD,
            .pinPortIdx    = 3u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTD,
            .direction     = GPIO_INPUT_DIRECTION,
        }, // BTN_MCU
        {
            .base          = PORTD,
            .pinPortIdx    = 4u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTD,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // EN8
        {
            .base          = PORTD,
            .pinPortIdx    = 5u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTD,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // TP324 -> FT CPU-charge-trickle-VARTA
        {
            .base          = PORTD,
            .pinPortIdx    = 6u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // TP270
        {
            .base          = PORTD,
            .pinPortIdx    = 7u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // TP271
        {
            .base          = PORTD,
            .pinPortIdx    = 8u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTD,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // EN2
        {
            .base          = PORTD,
            .pinPortIdx    = 9u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTD,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // Offsef_EN
        {
            .base          = PORTD,
            .pinPortIdx    = 10u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTD,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // EN4
        {
            .base          = PORTD,
            .pinPortIdx    = 11u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTD,
            .direction     = GPIO_OUTPUT_DIRECTION,
        },// MUTE_MCU

        {
            .base          = PORTD,
            .pinPortIdx    = 12u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTD,
            .direction     = GPIO_OUTPUT_DIRECTION,
        },  // CPU-charge-ON

        {
            .base          = PORTD,
            .pinPortIdx    = 13u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTD,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // BCALL_BL_MCU

        {
            .base          = PORTD,
            .pinPortIdx    = 14u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTD,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // ECALL_IL_MCU
        {
            .base          = PORTD,
            .pinPortIdx    = 15u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_INT_RISING_EDGE,
            .clearIntFlag  = false,
            .gpioBase      = PTD,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // ECALL_BL_MCU
        {
            .base          = PORTD,
            .pinPortIdx    = 16u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTD,
            .direction     = GPIO_OUTPUT_DIRECTION,

        },// TP437  //CPU-charge-trickle

        {
            .base          = PORTD,
            .pinPortIdx    = 17u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTD,
            .direction     = GPIO_INPUT_DIRECTION,
        },// RF_WKUP

    };

    PINS_DRV_Init(sizeof(portd) / sizeof(portd[0]), portd);

}

static void porte_init_pins(void)
{
    pin_settings_config_t porte[] =
    {
        {
            .base          = PORTE,
            .pinPortIdx    = 0u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT5,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // CAN4_SOUT
        {
            .base          = PORTE,
            .pinPortIdx    = 1u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT5,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // CAN4_CS



        {
            .base          = PORTE,
            .pinPortIdx    = 2u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTE,
            .direction     = GPIO_INPUT_DIRECTION,
        }, // S_CHARGE_WKUP
        {
            .base          = PORTE,
            .pinPortIdx    = 3u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTE,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // CAN_STB
        {
            .base          = PORTE,
            .pinPortIdx    = 4u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT5,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // RD1


        {
            .base          = PORTE,
            .pinPortIdx    = 5u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT5,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // TD1
        {
            .base          = PORTE,
            .pinPortIdx    = 6u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTE,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // WAKEUP_EC20_3V3
        {
            .base          = PORTE,
            .pinPortIdx    = 7u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTE,
            .direction     = GPIO_INPUT_DIRECTION,
        }, // G_WKUP
        {
            .base          = PORTE,
            .pinPortIdx    = 8u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTE,
            .direction     = GPIO_OUTPUT_DIRECTION,
        },// TPS_PS

        {
            .base          = PORTE,
            .pinPortIdx    = 9u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTE,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // BCALL_IL_MCU
        {
            .base          = PORTE,
            .pinPortIdx    = 10u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        },// TP435

        {
            .base          = PORTE,
            .pinPortIdx    = 11u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        },// TP436

        {
            .base          = PORTE,
            .pinPortIdx    = 12u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTE,
            .direction     = GPIO_INPUT_DIRECTION,
        }, // ON_WKUP
        {
            .base          = PORTE,
            .pinPortIdx    = 13u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTE,
            .direction     = GPIO_INPUT_DIRECTION,
        }, // USB_WKUP
        {
            .base          = PORTE,
            .pinPortIdx    = 14u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTE,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // EN1

        {
            .base          = PORTE,
            .pinPortIdx    = 15u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        },// TP125

        {
            .base          = PORTE,
            .pinPortIdx    = 16u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_PIN_DISABLED,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        },// TP434
    };
    PINS_DRV_Init(sizeof(porte) / sizeof(porte[0]), porte);
}

void pin_init(void)
{
    porta_init_pins();
    portb_init_pins();
    portc_init_pins();
    portd_init_pins();
    porte_init_pins();
}

void pin_I2Cgpio_init(void)
{
    pin_settings_config_t porta[] =
    {

        {
            .base          = PORTA,
            .pinPortIdx    = 2u,
            .pullConfig    = PORT_INTERNAL_PULL_UP_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTA,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // I2C_SDA
        {
            .base          = PORTA,
            .pinPortIdx    = 3u,
            .pullConfig    = PORT_INTERNAL_PULL_UP_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_AS_GPIO,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
            .gpioBase      = PTA,
            .direction     = GPIO_OUTPUT_DIRECTION,
        }, // I2C_SCL
    };

    PINS_DRV_Init(sizeof(porta) / sizeof(porta[0]), porta);
}
void pin_I2C_init(void)
{
    pin_settings_config_t porta[] =
    {
        {
            .base          = PORTA,
            .pinPortIdx    = 2u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT3,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // I2C_SDA
        {
            .base          = PORTA,
            .pinPortIdx    = 3u,
            .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
            .passiveFilter = false,
            .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
            .mux           = PORT_MUX_ALT3,
            .pinLock       = false,
            .intConfig     = PORT_DMA_INT_DISABLED,
            .clearIntFlag  = false,
        }, // I2C_SCL
    };

    PINS_DRV_Init(sizeof(porta) / sizeof(porta[0]), porta);
}


