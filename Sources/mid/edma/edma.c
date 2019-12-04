#include <edma.h>
#include <edma_driver.h>
#include <edma_hal.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/_stdint.h>
#include <S32K144_features.h>

edma_state_t edmaController_State;

edma_chn_state_t dmaChn0_Uart0Tx_State;
edma_chn_state_t dmaChn1_Spi0Rx_State;
edma_chn_state_t dmaChn2_Spi0Tx_State;


edma_chn_state_t *const edmaChnStatArray[] =
{
    &dmaChn0_Uart0Tx_State,
    &dmaChn1_Spi0Rx_State,
    &dmaChn2_Spi0Tx_State,
};

edma_channel_config_t dmaChn0_Uart0Tx_Config =
{
    .priority = EDMA_CHN_DEFAULT_PRIORITY,
    .channel = EDMA_CHN_LPUART0_TX,
    .source = EDMA_REQ_LPUART0_TX,
    .callback = NULL,
    .callbackParam = NULL
};


edma_channel_config_t dmaChn1_Spi0Rx_Config =
{
    .priority = EDMA_CHN_DEFAULT_PRIORITY,
    .channel = EDMA_CHN_LPSPI0_RX,
    .source = EDMA_REQ_LPSPI0_RX,
    .callback = NULL,
    .callbackParam = NULL
};

edma_channel_config_t dmaChn2_Spi0Tx_Config =
{
    .priority = EDMA_CHN_DEFAULT_PRIORITY,
    .channel = EDMA_CHN_LPSPI0_TX,
    .source = EDMA_REQ_LPSPI0_TX,
    .callback = NULL,
    .callbackParam = NULL
};


const edma_channel_config_t *const edmaChnCfgArray[] =
{
    &dmaChn0_Uart0Tx_Config,
    &dmaChn1_Spi0Rx_Config,
    &dmaChn2_Spi0Tx_Config,
};

edma_user_config_t edmaController_InitConfig =
{
    .chnArbitration = EDMA_ARBITRATION_FIXED_PRIORITY,
    .notHaltOnError = false,
};

status_t edma_init(void)
{
    /* Initialize eDMA module & channels */
    return EDMA_DRV_Init(&edmaController_State, &edmaController_InitConfig, edmaChnStatArray,
                         edmaChnCfgArray, EDMA_CHN_COUNT);
}

status_t edma_deinit(void)
{
    return EDMA_DRV_Deinit();
}

status_t edma_chn_realese(uint8_t channel)
{
    return EDMA_DRV_ReleaseChannel(channel);
}

status_t edma_chn_reinit(uint8_t channel)
{
    return EDMA_DRV_ChannelInit(edmaChnStatArray[channel], edmaChnCfgArray[channel]);
}

