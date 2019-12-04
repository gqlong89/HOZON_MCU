/*
 * Copyright (c) 2014 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * @file rtc_hal.c
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External could be made static.
 * The function is defined for use by application code.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 10.3, Expression assigned to a narrower or different essential type.
 * The cast is required to perform a conversion between an unsigned integer and an enum type.
 */

#include "rtc_hal.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : RTC_HAL_Init
 * Description   : This function initializes the RTC instance
 * Return          STATUS_SUCCESS if the operation was successful,
 *                 STATUS_ERROR if at least one register is locked.
 * Implements    : RTC_HAL_Init_Activity
 *END**************************************************************************/
status_t RTC_HAL_Init(RTC_Type * const base)
{
    status_t statusCode = STATUS_SUCCESS;

    /* Check if the registers are locked */
    if ((base->LR & 0xFFu) != 0xFFu)
    {
        statusCode =  STATUS_ERROR;
    }
    else
    {
        /* Set all registers to default values, except for RTC IER */
        /* Disable all interrupts */
        base->IER = 0UL;
        /* Clear all flags and disable the counter */
        base->SR  = 0UL;
        /* Set Time Seconds Registers to 1 to avoid triggering Time
         * Invalid Interrupt
         */
        base->TSR = 1UL;
        /* Clear Time Prescaler Register */
        base->TPR = 0UL;
        /* Clear Time Alarm Register */
        base->TAR = 0UL;
        /* Set Configuration Register to reset value */
        base->CR  = 0UL;
        /* Set Lock Register to default value */
        base->LR  = 0xFFUL;

        /* Check if the configuration was successful */
        if (RTC_HAL_GetTimeInvalidFlag(base) == true)
        {
            statusCode = STATUS_ERROR;
        }
    }
    /* Return the exit code */
    return statusCode;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : RTC_HAL_Enable
 * Description   : This function enables the RTC counter
 * Return          STATUS_SUCCESS if the operation was successful, STATUS_ERROR
 *                 if the counter is enabled.
 * Implements    : RTC_HAL_Enable_Activity
 *END**************************************************************************/
status_t RTC_HAL_Enable(RTC_Type * const base)
{
    status_t statusCode = STATUS_SUCCESS;
    bool isCounterEnabled;

    /* Get the flags signaling if the counter is enabled */
    isCounterEnabled = RTC_HAL_GetTimeCounterEnable(base);

    /* Check if the RTC counter is enabled */
    if (isCounterEnabled)
    {
        statusCode = STATUS_ERROR;
    }
    else
    {
        /* Enable oscillator and seconds counter */
        RTC_HAL_SetTimeCounterEnable(base, true);
    }
    /* Return the exit code */
    return statusCode;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : RTC_HAL_Disable
 * Description   : This function disables the RTC counter
 * Return          STATUS_SUCCESS if the operation was successful, STATUS_ERROR
 *                 if the counter was not disabled.
 * Implements    : RTC_HAL_Disable_Activity
 *END**************************************************************************/
status_t RTC_HAL_Disable(RTC_Type * const base)
{
    if (RTC_HAL_GetTimeCounterEnable(base) == true)
    {
        RTC_HAL_SetTimeCounterEnable(base, false);
    }

    /* Read TCE bit to check if the counter is really disabled and return the
     * corresponding result.
     *  -   Error if the timer is still enabled (The register can be locked)
     *  -   Success if the timer is disabled
     */
    return (status_t)(RTC_HAL_GetTimeCounterEnable(base) ? STATUS_ERROR : STATUS_SUCCESS);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : RTC_HAL_SetTimeSecondsRegister
 * Description   : This function along with SetTimePrescalerRegister will help
 *                 you set the starting time at a specified value.
 *                 The write will fail if the Time Counter is enabled and will return
 *                 STATUS_ERROR, otherwise the return will be STATUS_SUCCESS
 * Return        : STATUS_SUCCESS if the write is succeeded or STATUS_ERROR if
 *                 the counter is enabled.
 * Implements    : RTC_HAL_SetTimeSecondsRegister_Activity
 *END**************************************************************************/
status_t RTC_HAL_SetTimeSecondsRegister(RTC_Type * const base, uint32_t seconds)
{
    status_t statusCode = STATUS_SUCCESS;

    if (RTC_HAL_GetTimeCounterEnable(base) == true)
    {
        statusCode = STATUS_ERROR;
    }
    else
    {
        base->TSR  = seconds;
        statusCode = STATUS_SUCCESS;
    }
    /* Return the exit code */
    return statusCode;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : RTC_HAL_SetTimePrescalerRegister
 * Description   : This function along with SetTimeSecondsRegister will help you set
 *                 the starting time at a specified value.
 *                 The write will fail if the Time Counter is enabled and will return
 *                 STATUS_ERROR, otherwise the return will be STATUS_SUCCESS
 * Return        : STATUS_SUCCESS if the write is succeeded or STATUS_ERROR if
 *                 the counter is enabled.
 * Implements    : RTC_HAL_SetTimePrescalerRegister_Activity
 *END**************************************************************************/
status_t RTC_HAL_SetTimePrescalerRegister(RTC_Type * const base, uint16_t value)
{
    status_t statusCode = STATUS_SUCCESS;

    if (RTC_HAL_GetTimeCounterEnable(base) == true)
    {
        statusCode = STATUS_ERROR;
    }
    else
    {
        uint32_t tmp = base->TPR;
        tmp &= ~(RTC_TPR_TPR_MASK);
        tmp |= RTC_TPR_TPR(value);
        base->TPR = tmp;
        statusCode = STATUS_SUCCESS;
    }
    /* Return the exit code */
    return statusCode;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : RTC_HAL_ConfigureRegisterLock
 * Description   : This method will allow you to lock the registers. It will return
 *                 STATUS_SUCCESS if the lock was successful or if the register
 *                 was already locked, STATUS_ERROR if the Lock Register is
 *                 already locked or if the registerToConfig parameter is not a
 *                 valid register.
 * Return        : Status of the operation
 * Implements    : RTC_HAL_ConfigureRegisterLock_Activity
 *END**************************************************************************/
status_t RTC_HAL_ConfigureRegisterLock(RTC_Type * const base, rtc_lock_register_select_t registerToConfig)
{
    status_t statusCode = STATUS_SUCCESS;

    /* Check if the Lock Register is already locked,
     * if true, any other register lock status cannot
     * be modified.
     */
    if (RTC_HAL_GetLockRegisterLock(base) == true)
    {
        statusCode = STATUS_ERROR;
    }
    else
    {
        /* If the Lock Register is not locked we can
         * configure the register lock.
         */
        switch (registerToConfig)
        {
            case RTC_LOCK_REG_LOCK:
                RTC_HAL_LockRegisterLock(base);
                break;
            case RTC_STATUS_REG_LOCK:
                RTC_HAL_StatusRegisterLock(base);
                break;
            case RTC_CTRL_REG_LOCK:
                RTC_HAL_ControlRegisterLock(base);
                break;
            case RTC_TCL_REG_LOCK:
                RTC_HAL_TimeCompensationLock(base);
                break;
            default:
                /* If the register is not recognized, return error */
                statusCode = STATUS_ERROR;
                break;
        }
    }
    /* Return the exit code */
    return statusCode;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : RTC_HAL_IsRegisterLocked
 * Description   : This method will get the register lock status
 * Return        : True if the register is locked, false if not
 * Implements    : RTC_HAL_IsRegisterLocked_Activity
 *END**************************************************************************/
bool RTC_HAL_IsRegisterLocked(const RTC_Type * const base, rtc_lock_register_select_t reg)
{
    bool state = false;

    switch (reg)
    {
        case RTC_LOCK_REG_LOCK:
            state = RTC_HAL_GetLockRegisterLock(base);
            break;
        case RTC_CTRL_REG_LOCK:
            state = RTC_HAL_GetControlRegisterLock(base);
            break;
        case RTC_STATUS_REG_LOCK:
            state = RTC_HAL_GetStatusRegisterLock(base);
            break;
        case RTC_TCL_REG_LOCK:
            state = RTC_HAL_GetTimeCompensationLock(base);
            break;
        default:
            /* This statement should not be reached */
            break;
    }
    /* Return the exit code */
    return state;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : RTC_HAL_ConfigureClockOut
 * Description   : This method will allow you to configure the RTC Clock out pin.
 *                 It will return STATUS_SUCCESS if the configuration was successful
 *                 STATUS_ERROR if the Control Register is locked.
 * Return        : Status of the operation
 * Implements    : RTC_HAL_ConfigureClockOut_Activity
 *END**************************************************************************/
status_t RTC_HAL_ConfigureClockOut(RTC_Type * const base, rtc_clk_out_config_t config)
{
    status_t statusCode = STATUS_SUCCESS;

    /*
     * Check if the Control Register is already locked,
     * if true, clock out configuration cannot be modified.
     */
    if (RTC_HAL_GetControlRegisterLock(base) == true)
    {
        statusCode = STATUS_ERROR;
    }
    else
    {
        switch (config)
        {
            case RTC_CLKOUT_DISABLED:
                /* Disable the clock out pin */
                base->CR &= ~RTC_CR_CPE_MASK;
                break;
            case RTC_CLKOUT_SRC_TSIC:
                /* Select clock out source as Time Seconds Interrupt and enable the pin */
                base->CR &= ~(RTC_CR_CPE_MASK | RTC_CR_CPS_MASK);
                base->CR |= (RTC_CR_CPE(1U) | RTC_CR_CPS(0U));
                break;
            case RTC_CLKOUT_SRC_32KHZ:
                /* Select clock out source as the 32 KHz clock and enable the pin */
                base->CR &= ~(RTC_CR_CPE_MASK | RTC_CR_CPS_MASK);
                base->CR |= (RTC_CR_CPE(1U) | RTC_CR_CPS(1U));
                break;
            default:
                /* This statement should not be reached */
                break;
        }
    }
    /* Return the exit code */
    return statusCode;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
