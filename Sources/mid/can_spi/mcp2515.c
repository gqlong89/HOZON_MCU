/**
 * @Title: mcp2515.c
 * @author yuzhimin
 * @date Jul 21, 2017
 * @version V1.0
 */

#include <lpspi_hal.h>
#include <lpspi_master_driver.h>
#include <lpspi_shared_function.h>
#include <mid/can_spi/can_spi.h>
#include <mid/can_spi/mcp2515.h>
#include <stddef.h>

/*! @brief State structure for LPSPI1 */
static lpspi_state_t lpspiCan1State;

/*! @brief State structure for LPSPI2 */
static lpspi_state_t lpspiCan2State;

static uint8_t can_spi_master_transmit(uint32_t instance, uint8_t *tx, uint8_t *rx, uint8_t len)
{
    return ((uint8_t)LPSPI_DRV_MasterTransferBlocking(instance, tx, rx, len, SPI_TIMEOUT));
    //    return (LPSPI_DRV_MasterTransfer( instance, tx, rx, len ));
}

void mcp2515_init(uint32_t instance)
{
    lpspi_master_config_t lpspican_MasterConfig =
    {
        .bitsPerSec = 8000000U,
        .whichPcs = LPSPI_PCS0,
        .pcsPolarity = LPSPI_ACTIVE_LOW,
        .isPcsContinuous = true,
        .bitcount = 8U,
        .lpspiSrcClk = 8000000U,
        .clkPhase = LPSPI_CLOCK_PHASE_1ST_EDGE,
        .clkPolarity = LPSPI_SCK_ACTIVE_HIGH,
        .lsbFirst = false,
        .transferType = LPSPI_USING_INTERRUPTS,
        .rxDMAChannel = 255,
        .txDMAChannel = 255,
    };

    if (LPSPICAN1 == instance)
    {
        LPSPI_DRV_MasterInit(LPSPICAN1, &lpspiCan1State, &lpspican_MasterConfig);
        LPSPI_HAL_Disable(g_lpspiBase[instance]);
        LPSPI_HAL_SetPinConfigMode(g_lpspiBase[instance], LPSPI_SDI_OUT_SDO_IN,
                                   LPSPI_DATA_OUT_RETAINED, true);                         //设置管脚方向
        LPSPI_DRV_MasterSetDelay(LPSPICAN1, 1, 1,
                                 1);              //设置时间间隔  cs 与 data ， data 与 data
        LPSPI_HAL_Enable(g_lpspiBase[instance]);

    }
    else if (LPSPICAN2 == instance)
    {
        LPSPI_DRV_MasterInit(LPSPICAN2, &lpspiCan2State, &lpspican_MasterConfig);
        LPSPI_HAL_Disable(g_lpspiBase[instance]);
        LPSPI_HAL_SetPinConfigMode(g_lpspiBase[instance], LPSPI_SDI_OUT_SDO_IN,
                                   LPSPI_DATA_OUT_RETAINED, true);
        LPSPI_DRV_MasterSetDelay(LPSPICAN2, 1, 1, 1);
        LPSPI_HAL_Enable(g_lpspiBase[instance]);
    }
    else
    {

    }

    return;
}

uint8_t mcp2515_write_register(uint32_t instance, uint8_t addr, uint8_t data)
{
    uint8_t cmd[3];
    cmd[0] = SPI_WRITE;
    cmd[1] = addr;
    cmd[2] = data;
    return can_spi_master_transmit(instance, cmd, NULL, 3);
}

void mcp2515_write_registers(uint32_t instance, uint8_t addr, uint8_t *ptx, uint8_t len)
{
    ptx[0] = SPI_WRITE;
    ptx[1] = addr;
    can_spi_master_transmit(instance, ptx, NULL, len);
}

uint8_t mcp2515_read_register(uint32_t instance, uint8_t addr)
{
    uint8_t data[3] =
    {0, 0, 0 };
    uint8_t cmd[3];
    cmd[0] = SPI_READ;
    cmd[1] = addr;
    can_spi_master_transmit(instance, cmd, data, 3);
    return data[2];
}

void mcp2515_read_registers(uint32_t instance, uint8_t addr, uint8_t *prx, uint8_t len)
{
    uint8_t cmd[15];

    if (len > 15)
    {
        return;
    }

    cmd[0] = SPI_READ;
    cmd[1] = addr;
    can_spi_master_transmit(instance, cmd, prx, len);
    return;
}

void mcp2515_bit_modify(uint32_t instance, uint8_t addr, uint8_t mask, uint8_t data)
{
    uint8_t cmd[4];
    cmd[0] = SPI_BIT_MODIFY;
    cmd[1] = addr;
    cmd[2] = mask;
    cmd[3] = data;
    can_spi_master_transmit(instance, cmd, NULL, 4);
}

void mcp2515_reset(uint32_t instance)
{
    uint8_t cmd = SPI_RESET;
    can_spi_master_transmit(instance, &cmd, NULL, 1);
}

/*prx->首字节无效*/
bool mcp2515_read_buff(uint32_t instance, uint8_t *prx, uint8_t addr, uint8_t len)
{
    uint8_t cmd[14];

    if (SEND_LEN_DEF < len)
    {
        return false;
    }

    cmd[0] = SPI_READ_RX | ((addr & 0x03) << 1);
    can_spi_master_transmit(instance, cmd, prx, len);
    return true;
}

/*ptx->首字节预留*/
bool mcp2515_write_buff(uint32_t instance, uint8_t *ptx, uint8_t addr, uint8_t len)
{
    if (SEND_LEN_DEF < len)
    {
        return false;
    }

    *ptx = SPI_WRITE_TX | (addr & 0x07);
    can_spi_master_transmit(instance, ptx, NULL, len);
    return true;
}

/*SPI_RTS_0,SPI_RTS_1,SPI_RTS_2*/
void Send_start(uint32_t instance, uint8_t num)
{
    uint8_t cmd[1];
    cmd[0] = SPI_RTS | num;
    can_spi_master_transmit(instance, cmd, NULL, 1);
}

