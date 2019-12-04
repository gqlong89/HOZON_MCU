#include <ftm_driver.h>
#include <ftm_hal.h>
#include <srs.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/_stdint.h>

#define SRS_FTM_INSTANCE          (0U)

ftm_state_t ftm0StateStruct;
static uint32_t inputCaptureMeas = 0;
static uint32_t frequency;

void SRS_CRTL(void)
{
    frequency = FTM_DRV_GetFrequency(SRS_FTM_INSTANCE);

    /* Get values */
    inputCaptureMeas = FTM_DRV_GetInputCaptureMeasurement(SRS_FTM_INSTANCE, 4U);

    FTM_DRV_ClearInputCaptureMeasurement(SRS_FTM_INSTANCE, 4U);
}

void srs_init(void)
{
    ftm_user_config_t  srs_config =
    {
        {
            false,   /* Software trigger state */
            false,  /* Hardware trigger 1 state */
            false,  /* Hardware trigger 2 state */
            false,  /* Hardware trigger 3 state */
            false, /* Max loading point state */
            false, /* Min loading point state */
            FTM_SYSTEM_CLOCK, /* Update mode for INVCTRL register */
            FTM_SYSTEM_CLOCK, /* Update mode for SWOCTRL register */
            FTM_SYSTEM_CLOCK, /* Update mode for OUTMASK register */
            FTM_SYSTEM_CLOCK, /* Update mode for CNTIN register */
            false, /* Automatic clear of the trigger*/
            FTM_UPDATE_NOW, /* Synchronization point */
        },
        FTM_MODE_INPUT_CAPTURE, /*!< Mode of operation for FTM */
        FTM_CLOCK_DIVID_BY_32, /* FTM clock prescaler */
        FTM_CLOCK_SOURCE_EXTERNALCLK,   /* FTM clock source */
        FTM_BDM_MODE_11, /* FTM debug mode */
        false, /* Interrupt state */
        false /* Initialization trigger */
    };

    /* Callbacks declaration */
    /* Channels configuration structure for SRSFTimer input capture */
    ftm_input_ch_param_t srs_InputCaptureChnCfg[1] =
    {
        {
            4U, /* Channel id */
            FTM_SIGNAL_MEASUREMENT, /* Input capture operation mode */
            FTM_RISING_EDGE, /* Edge alignment mode */
            FTM_PERIOD_ON_MEASUREMENT, /* Signal measurement operation type */
            0U, /* Filter value */
            false, /* Filter state (enabled/disabled) */
            true,
            NULL,
            NULL
        }
    };
    /* Input capture configuration for SRSFTimer */
    ftm_input_param_t srs_InputCaptureConfig =
    {
        1U,                                 /* Number of channels */
        65535U,                             /* Max count value */
        srs_InputCaptureChnCfg /* Channels configuration */
    };

    /* Initialize FTM instances, PWM and Input capture
     *  -   See ftm component for more details
     */
    FTM_DRV_Init(SRS_FTM_INSTANCE, &srs_config, &ftm0StateStruct);

    /* Setup input capture for FTM 1 channel 0  - PTB2 */
    FTM_DRV_InitInputCapture(SRS_FTM_INSTANCE, &srs_InputCaptureConfig);
}

unsigned int kernelGetSRSStatus(void)
{
    uint16_t srs_pulse_widths = 0;

    srs_pulse_widths = (inputCaptureMeas * 1000) / frequency;

    if ((srs_pulse_widths >= 16) && (srs_pulse_widths <= 64))
    {
        return SRS_SIGNAL_NORMAL;
    }
    else if ((srs_pulse_widths >= 176) && (srs_pulse_widths <= 224))
    {
        return SRS_SIGNAL_CRASH;
    }
    else
    {
        return SRS_SIGNAL_UNKNOWN;
    }
}

