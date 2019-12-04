#include <i2c.h>
#include <lpi2c_driver.h>
#include <mid/kernel/kernel.h>
#include <status.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "gpio.h"
#include "gpio_hal.h"
#include "pin.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
lpi2c_master_state_t i2cMasterState;

#define RTC_WAIT_TIMEOUT   50

/*******************************************************************************
 * Code
 ******************************************************************************/
static void I2C_Gpio_delay(uint32_t time_value)
{
    uint8_t count;

    for (count = 0; count < time_value; count++)
    {
        __asm("NOP");
        __asm("NOP");
        __asm("NOP");
        __asm("NOP");
        __asm("NOP");
        __asm("NOP");
        __asm("NOP");
        __asm("NOP");
        __asm("NOP");
        __asm("NOP");
    }
}

static void I2C_Fail_recover(void)
{
    uint8_t count1;
    pin_I2Cgpio_init();

    for (count1 = 0; count1 < 10; count1++)
    {
        GPIO_HAL_WritePin(I2C_SDA_PORT, I2C_SDA_PORT_PIN, 0U);
        GPIO_HAL_WritePin(I2C_SCL_PORT, I2C_SCL_PORT_PIN, 0U);
        I2C_Gpio_delay(2);
        GPIO_HAL_WritePin(I2C_SDA_PORT, I2C_SDA_PORT_PIN, 1U);
        GPIO_HAL_WritePin(I2C_SCL_PORT, I2C_SCL_PORT_PIN, 1U);
        I2C_Gpio_delay(2);
    }

    pin_I2C_init();
}

static int I2C_WriteRegs(uint32_t instance, uint16_t slave_addr, uint8_t reg_addr, uint8_t *txBuff,
                         uint32_t txSize)
{
    status_t ret = 0;
    uint8_t txbuff[10];

    if (txSize > 8)
    {
        return -1;
    }

    txbuff[0] = reg_addr;
    memcpy(&txbuff[1], txBuff, txSize);
    txSize++;
    ret |= LPI2C_DRV_MasterSlaveAddrSendDataBlocking(instance, slave_addr, txbuff, txSize, true,
            RTC_WAIT_TIMEOUT);

    return ret;
}

static int I2C_ReadRegs(uint32_t instance, uint16_t slave_addr, uint8_t reg_addr, uint8_t *rxBuff,
                        uint32_t rxSize)
{
    status_t ret = 0;
    uint8_t txbuff[10];

    txbuff[0] = reg_addr;
    ret |= LPI2C_DRV_MasterSlaveAddrSendDataBlocking(instance, slave_addr, txbuff, 1, false,
            RTC_WAIT_TIMEOUT);
    ret |= LPI2C_DRV_MasterSlaveAddrReceiveDataBlocking(instance, slave_addr, rxBuff, rxSize, true,
            RTC_WAIT_TIMEOUT);

    return ret;
}

int driverI2CWrite(uint16_t slave_addr, unsigned int addr, unsigned char *buf,
                   unsigned int num)
{
    return I2C_WriteRegs(I2C_INSTANCE, slave_addr, addr, buf, num);
}

int driverI2CRead(uint16_t slave_addr, unsigned int addr, unsigned char *buf,
                  unsigned int num)
{
    return I2C_ReadRegs(I2C_INSTANCE, slave_addr, addr, buf, num);
}

void driverI2CInit(void)
{
    driverI2CDeinit();

    /**recover i2c if it is fail*/
    I2C_Fail_recover();
    /*****/

    lpi2c_master_user_config_t i2cMasterCfg =
    {
        .slaveAddress = 81U,
        .is10bitAddr = false,
        .operatingMode = LPI2C_STANDARD_MODE,
        .baudRate = 50000U,
        .transferType = LPI2C_USING_INTERRUPTS,
        .dmaChannel = 0U,
        .masterCallback = NULL,
        .callbackParam = NULL,
    };
    LPI2C_DRV_MasterInit(I2C_INSTANCE, &i2cMasterCfg, &i2cMasterState);
}

void driverI2CDeinit(void)
{
    LPI2C_DRV_MasterDeinit(I2C_INSTANCE);
}

int shell_i2c_read(unsigned int argc, unsigned char **argv)
{
    unsigned int reg;
    unsigned int dev;
    unsigned int len;
    unsigned char buf[10];
    unsigned int i;
    int ret;

    /* show help */
    if (argc == 2 && argv[0][0] == '!')
    {
        shellPrintf(" %10s - i2cr <dev addr> <reg addr> <lenth>.\r\n", argv[1]);
        return 0;
    }

    sscanf((char const *) argv[0], "%u", &dev);
    sscanf((char const *) argv[1], "%u", &reg);
    sscanf((char const *) argv[2], "%u", &len);

    ret = driverI2CRead((uint16_t)(dev & 0xFF), (reg & 0xFF), buf, len);

    if (!ret)
    {
        shellPrintf(" [0x%x,0x%x,%u]data:", dev, reg, len);

        for (i = 0; i < len; i++)
        {
            shellPrintf(" 0x%x", buf[i]);
        }

        shellPrintf(" \r\n");
    }
    else
    {
        shellPrintf(" read error ret:%d\r\n", ret);
    }

    return 1;
}


