/**
* @Title: clock.c
* @author yuzhimin
* @date Jan 26, 2018
* @version V1.0
*/

#include <clock_manager.h>
#include <pcc_hal.h>
#include <scg_hal.h>
#include <sim_hal_S32K144.h>
#include <S32K144_features.h>

#define NUM_OF_PERIPHERAL_CLOCKS    (21U)
#define CLOCK_MGR_CONFIG_CNT        (1U)
#define CLOCK_MGR_CALLBACK_CNT      (0U)

void clock_init(void)
{
    peripheral_clock_config_t perClkCfg[NUM_OF_PERIPHERAL_CLOCKS] =
    {
        {
            .clockName        = PCC_ADC0_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_SIRC,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_ADC1_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_SIRC,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_DMAMUX0_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_OFF,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_FlexCAN0_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_OFF,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_FlexCAN1_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_OFF,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_FlexCAN2_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_OFF,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_FTM0_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_SIRC,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_FTM1_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_SIRC,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_FTM2_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_SIRC,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_FTM3_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_SIRC,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_LPI2C0_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_SIRC,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_EIGTH,
        },
        {
            .clockName        = PCC_LPSPI0_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_SIRC,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_LPSPI1_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_SIRC,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_LPSPI2_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_SIRC,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_LPUART0_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_FIRC,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_PORTA_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_OFF,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_PORTB_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_OFF,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_PORTC_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_OFF,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_PORTD_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_OFF,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_PORTE_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_OFF,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
        {
            .clockName        = PCC_RTC0_CLOCK,
            .clkGate          = true,
            .clkSrc           = CLK_SRC_OFF,
            .frac             = MULTIPLY_BY_ONE,
            .divider          = DIVIDE_BY_ONE,
        },
    };

    clock_manager_user_config_t clkMgrCfg =
    {
        /*! @brief Configuration of SIRC */
        .scgConfig =
        {
            .sircConfig =
            {
                .initialize       = true,                                /*!< Initialize */
                /* SIRCCSR */
                .enableInStop     = false,                               /*!< SIRCSTEN  */
                .enableInLowPower = true,                                /*!< SIRCLPEN  */
                .locked           = false,                               /*!< LK        */
                /* SIRCCFG */
                .range            = SCG_SIRC_RANGE_HIGH,                 /*!< RANGE - High range (8 MHz) */
                /* SIRCDIV */
                .div1             = SCG_ASYNC_CLOCK_DIV_BY_1,            /*!< SIRCDIV1  */
                .div2             = SCG_ASYNC_CLOCK_DIV_BY_1,            /*!< SIRCDIV2  */
            },
            .fircConfig =
            {
                .initialize       = true,                                /*!< Initialize */
                /* FIRCCSR */
                .regulator        = true,                                /*!< FIRCREGOFF */
                .locked           = false,                               /*!< LK         */
                /* FIRCCFG */
                .range            = SCG_FIRC_RANGE_48M,                  /*!< RANGE      */
                /* FIRCDIV */
                .div1             = SCG_ASYNC_CLOCK_DIV_BY_1,            /*!< FIRCDIV1   */
                .div2             = SCG_ASYNC_CLOCK_DIV_BY_1,            /*!< FIRCDIV2   */
            },
            .rtcConfig =
            {
                .initialize       = true,                                /*!< Initialize  */
                .rtcClkInFreq     = 0U,                                  /*!< RTC_CLKIN   */
            },
            .soscConfig =
            {
                .initialize       = true,                                /*!< Initialize */
                .freq             = 8000000U,                            /*!< Frequency  */
                /* SOSCCSR */
                .monitorMode      = SCG_SOSC_MONITOR_DISABLE,            /*!< SOSCCM      */
                .locked           = false,                               /*!< LK          */
                /* SOSCCFG */
                .extRef           = SCG_SOSC_REF_OSC,                    /*!< EREFS       */
                .gain             = SCG_SOSC_GAIN_LOW,                   /*!< HGO         */
                .range            = SCG_SOSC_RANGE_MID,                  /*!< RANGE       */
                /* SOSCDIV */
                .div1             = SCG_ASYNC_CLOCK_DIV_BY_1,            /*!< SOSCDIV1    */
                .div2             = SCG_ASYNC_CLOCK_DIV_BY_1,            /*!< SOSCDIV2    */
            },
            .spllConfig =
            {
                .initialize       = true,                                /*!< Initialize */
                /* SPLLCSR */
                .monitorMode      = SCG_SPLL_MONITOR_DISABLE,            /*!< SPLLCM     */
                .locked           = false,                               /*!< LK         */
                /* SPLLCFG */
                .prediv           = 0U,                                  /*!< PREDIV     */
                .mult             = 24U,                                 /*!< MULT       */
                .src              = 0U,                                  /*!< SOURCE     */
                /* SPLLDIV */
                .div1             = SCG_ASYNC_CLOCK_DIV_BY_1,            /*!< SPLLDIV1   */
                .div2             = SCG_ASYNC_CLOCK_DIV_BY_1,            /*!< SPLLDIV2   */
            },
            .clockOutConfig =
            {
                .initialize       = true,                                /*!< Initialize    */
                .source           = SCG_CLOCKOUT_SRC_FIRC,               /*!< SCG CLKOUTSEL     */
            },
            .clockModeConfig =
            {
                .initialize       = true,                                /*!< Initialize */
                .rccrConfig =              /*!< RCCR - Run Clock Control Register          */
                {
                    .src          = SCG_SYSTEM_CLOCK_SRC_SYS_PLL,        /*!< SCS        */
                    .divCore      = SCG_SYSTEM_CLOCK_DIV_BY_2,           /*!< DIVCORE    */
                    .divBus       = SCG_SYSTEM_CLOCK_DIV_BY_2,           /*!< DIVBUS     */
                    .divSlow      = SCG_SYSTEM_CLOCK_DIV_BY_3,           /*!< DIVSLOW    */
                },
                .vccrConfig =              /*!< VCCR - VLPR Clock Control Register         */
                {
                    .src          = SCG_SYSTEM_CLOCK_SRC_SIRC,           /*!< SCS        */
                    .divCore      = SCG_SYSTEM_CLOCK_DIV_BY_2,           /*!< DIVCORE    */
                    .divBus       = SCG_SYSTEM_CLOCK_DIV_BY_1,           /*!< DIVBUS     */
                    .divSlow      = SCG_SYSTEM_CLOCK_DIV_BY_4,           /*!< DIVSLOW    */
                },
                .hccrConfig =              /*!< HCCR - HSRUN Clock Control Register        */
                {
                    .src          = SCG_SYSTEM_CLOCK_SRC_SYS_PLL,        /*!< SCS        */
                    .divCore      = SCG_SYSTEM_CLOCK_DIV_BY_2,           /*!< DIVCORE    */
                    .divBus       = SCG_SYSTEM_CLOCK_DIV_BY_2,           /*!< DIVBUS     */
                    .divSlow      = SCG_SYSTEM_CLOCK_DIV_BY_4,           /*!< DIVSLOW    */
                },
            },
        },
        .pccConfig =
        {
            .peripheralClocks = perClkCfg,                              /*!< Peripheral clock control configurations  */
            .count = NUM_OF_PERIPHERAL_CLOCKS,                          /*!< Number of the peripheral clock control configurations  */
        },
        .simConfig =
        {
            .clockOutConfig =              /*!< Clock Out configuration.           */
            {
                .initialize       = true,                                /*!< Initialize    */
                .enable           = false,                               /*!< CLKOUTEN      */
                .source           = SIM_CLKOUT_SEL_SYSTEM_SCG_CLKOUT,    /*!< CLKOUTSEL     */
                .divider          = SIM_CLKOUT_DIV_BY_1,                 /*!< CLKOUTDIV     */
            },
            .lpoClockConfig =              /*!< Low Power Clock configuration.     */
            {
                .initialize       = true,                                /*!< Initialize    */
                .enableLpo1k      = true,                                /*!< LPO1KCLKEN    */
                .enableLpo32k     = true,                                /*!< LPO32KCLKEN   */
                .sourceLpoClk     = SIM_LPO_CLK_SEL_LPO_128K,            /*!< LPOCLKSEL     */
                .sourceRtcClk     = SIM_RTCCLK_SEL_LPO_32K,              /*!< RTCCLKSEL     */
            },
            .platGateConfig =              /*!< Platform Gate Clock configuration. */
            {
                .initialize       = true,                                /*!< Initialize    */
                .enableMscm       = true,                                /*!< CGCMSCM       */
                .enableMpu        = true,                                /*!< CGCMPU        */
                .enableDma        = true,                                /*!< CGCDMA        */
                .enableErm        = true,                                /*!< CGCERM        */
                .enableEim        = true,                                /*!< CGCEIM        */
            },
            .tclkConfig =                  /*!< TCLK CLOCK configuration. */
            {
                .initialize       = true,                                /*!< Initialize    */
                .tclkFreq[0]      = 0U,                                  /*!< TCLK0         */
                .tclkFreq[1]      = 0U,                                  /*!< TCLK1         */
                .tclkFreq[2]      = 0U,                                  /*!< TCLK2         */
            },
            .traceClockConfig =            /*!< Debug trace Clock Configuration. */
            {
                .initialize       = true,                                /*!< Initialize    */
                .divEnable        = true,                                /*!< TRACEDIVEN    */
                .source           = CLOCK_TRACE_SRC_CORE_CLK,            /*!< TRACECLK_SEL  */
                .divider          = 0U,                                  /*!< TRACEDIV      */
                .divFraction      = false,                               /*!< TRACEFRAC     */
            },
        },
        .pmcConfig =
        {
            .lpoClockConfig =              /*!< Low Power Clock configuration.     */
            {
                .initialize       = true,                                /*!< Initialize             */
                .enable           = true,                                /*!< Enable/disable LPO     */
                .trimValue        = 0,                                   /*!< Trimming value for LPO */
            },
        },
    };

    clock_manager_user_config_t const *clkMgrCfgsArr[] =
    {
        &clkMgrCfg
    };

    clock_manager_callback_user_config_t *clkMgrCallbacksArr[] = {(void *)0};

    CLOCK_SYS_Init(clkMgrCfgsArr, CLOCK_MGR_CONFIG_CNT, clkMgrCallbacksArr,
                   CLOCK_MGR_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);
}


