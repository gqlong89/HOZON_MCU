#include <interrupt_manager.h>
#include <mid/kernel/kernel.h>
#include <stdbool.h>
#include <system_S32K144.h>
#include <S32K144.h>
#include <watchdog1.h>
#include <wdog_driver.h>
#include <wdog_hal.h>

#define WDG_INSTANCE            (0U)
#define WDG_TIME_OUT            (5*500)            /* 5S = (WDG_TIME_OUT/(LPO_CLOCK/256))S = ((WDG_TIME_OUT/500) */

static void WDOG_ISR(void)
{

}

void driverWDGInit(void)
{
    wdog_user_config_t wdog_config =
    {
        .clkSource = WDOG_LPO_CLOCK,
        .opMode = {
            false, /* Wait Mode*/
            false, /* Stop Mode*/
            false /* Debug Mode*/
        },
        .updateEnable = true,
        .intEnable = true,
        .winEnable = false,
        .windowValue = 0,
        .timeoutValue = 2500,
        .prescalerEnable = true
    };

    INT_SYS_InstallHandler(WDOG_EWM_IRQn, WDOG_ISR, (isr_t *) 0);
    INT_SYS_EnableIRQ(WDOG_EWM_IRQn);

    wdog_config = watchdog1_Config0;
    wdog_config.timeoutValue = WDG_TIME_OUT;
    WDOG_DRV_Init(WDG_INSTANCE, &wdog_config);
}

void driverWDGFeed(void)
{
    WDOG_DRV_Trigger(WDG_INSTANCE);
}

void driverWDGEnable(void)
{
    WDOG_HAL_Enable(WDG_INSTANCE);
}

void driverWDGDisable(void)
{
    WDOG_HAL_Disable(WDG_INSTANCE);
}

void driverReset(void)
{
    SystemSoftwareReset();
}

/*get the reset status and printf the result.
 * try by Cindy 2018/1/30;
 * */
uint32_t getResetStatus(void)
{
    uint32_t status = 0;
    status =  RCM->SRS;

    if (status == 0)
    {
        return status;
    }

    if (status & RCM_SRS_LVD_MASK)
    {
        printf(" Reset caused: LVD\r\n");
    }

    if (status & RCM_SRS_LOC_MASK)
    {
        printf(" Reset caused: LOC");
    }

    if (status & RCM_SRS_LOL_MASK)
    {
        printf(" Reset caused: LOL\r\n");
    }

    if (status & RCM_SRS_WDOG_MASK)
    {
        printf(" Reset caused: WDOG\r\n");
    }

    if (status & RCM_SRS_PIN_MASK)
    {
        printf(" Reset caused: PIN\r\n");
    }

    if (status & RCM_SRS_POR_MASK)
    {
        printf(" Reset caused: POR\r\n");
    }

    if (status & RCM_SRS_JTAG_MASK)
    {
        printf("Reset caused: JTAG\r\n");
    }

    if (status & RCM_SRS_LOCKUP_MASK)
    {
        printf(" Reset caused: LOCKUP\r\n");
    }

    if (status & RCM_SRS_SW_MASK)
    {
        printf(" Reset caused: Software\r\n");
    }

    if (status & RCM_SRS_MDM_AP_MASK)
    {
        printf(" Reset caused: MDM-AP\r\n");
    }

    if (status & RCM_SRS_SACKERR_MASK)
    {
        printf(" Reset caused: SACKERR\r\n");
    }

    return status;
}
