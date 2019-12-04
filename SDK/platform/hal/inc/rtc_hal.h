/*
 * Copyright (c) 2014 - 2015, Freescale Semiconductor, Inc.
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

#ifndef RTC_HAL_H
#define RTC_HAL_H
#include <stdbool.h>
#include "device_registers.h"
#include "status.h"

/**
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 10.3, Expression assigned to a narrower or different essential type.
 * The cast is required to perform a conversion between an unsigned integer and an enum type.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 10.8, Impermissible cast of composite expression
 * (different essential type categories).
 * This is required by the conversion of a bit/bitfield of a register into signed type.
 */

/*!
 * @addtogroup rtc_hal Real Time Clock HAL
 * @ingroup rtc
 * @brief Real Time Clock Hardware Abstraction Layer
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*!
 * @brief RTC Seconds interrupt configuration
* Implements : rtc_second_int_cfg_t_Class
 */
typedef enum
{
    RTC_INT_1HZ   = 0x00U,     /*!< RTC seconds interrupt occurs at 1 Hz      */
    RTC_INT_2HZ   = 0x01U,     /*!< RTC seconds interrupt occurs at 2 Hz      */
    RTC_INT_4HZ   = 0x02U,     /*!< RTC seconds interrupt occurs at 4 Hz      */
    RTC_INT_8HZ   = 0x03U,     /*!< RTC seconds interrupt occurs at 8 Hz      */
    RTC_INT_16HZ  = 0x04U,     /*!< RTC seconds interrupt occurs at 16 Hz     */
    RTC_INT_32HZ  = 0x05U,     /*!< RTC seconds interrupt occurs at 32 Hz     */
    RTC_INT_64HZ  = 0x06U,     /*!< RTC seconds interrupt occurs at 64 Hz     */
    RTC_INT_128HZ = 0x07U      /*!< RTC seconds interrupt occurs at 128 Hz    */
} rtc_second_int_cfg_t;

/*!
 * @brief RTC CLKOUT pin configuration
 * Implements : rtc_clk_out_config_t_Class
 */
typedef enum
{
    RTC_CLKOUT_DISABLED  = 0x00U,  /*!< Clock out pin is disabled                                    */
    RTC_CLKOUT_SRC_TSIC  = 0x01U,  /*!< Output on RTC_CLKOUT as configured on Time seconds interrupt */
    RTC_CLKOUT_SRC_32KHZ = 0x02U   /*!< Output on RTC_CLKOUT of the 32KHz clock                      */
} rtc_clk_out_config_t;

/*!
 * @brief RTC clock select
 * Implements : rtc_clk_select_t_Class
 */
typedef enum
{
    RTC_CLK_SRC_OSC_32KHZ = 0x00U, /*!< RTC Prescaler increments using 32 KHz crystal  */
    RTC_CLK_SRC_LPO_1KHZ  = 0x01U  /*!< RTC Prescaler increments using 1KHz LPO        */
} rtc_clk_select_t;

/*!
 * @brief RTC register lock
 * Implements : rtc_lock_register_select_t_Class
 */
typedef enum
{
    RTC_LOCK_REG_LOCK   = 0x00U,   /*!< RTC Lock Register lock         */
    RTC_STATUS_REG_LOCK = 0x01U,   /*!< RTC Status Register lock       */
    RTC_CTRL_REG_LOCK   = 0x02U,   /*!< RTC Control Register lock      */
    RTC_TCL_REG_LOCK    = 0x03U    /*!< RTC Time Compensation Reg lock */
} rtc_lock_register_select_t;

/*******************************************************************************
 * Code
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Configuration
 * @{
 */

/*****************************************************************************
 * Methods for RTC Control
 ****************************************************************************/

/*!
 * @brief Initialize RTC instance
 *
 * @param[in] base RTC base pointer
 * @return    STATUS_SUCCESS if the operation was successful,
 *            STATUS_ERROR if at least one register is locked.
 */
status_t RTC_HAL_Init(RTC_Type * const base);

/*!
 * @brief Enable RTC instance counter
 *
 * @param[in] base RTC base pointer
 * @return    STATUS_SUCCESS if the operation was successful, STATUS_ERROR
 *            if the counter is enabled or if the time invalid flag is set.
 */
status_t RTC_HAL_Enable(RTC_Type * const base);

/*!
 * @brief Disable RTC instance counter
 *
 * @param[in] base RTC base pointer
 * @return    STATUS_SUCCESS if the operation was successful, STATUS_ERROR
 *            if the counter was not disabled.
 */
status_t RTC_HAL_Disable(RTC_Type * const base);

/*!
 * @brief This function configures register lock status
 *
 * @param[in] base RTC base pointer
 * @param[in] registerToConfig Register to configure
 * @return    STATUS_SUCCESS if the lock was successful or if the register
 *            was already locked, STATUS_ERROR if the Lock Register is
 *            already locked or if the registerToConfig parameter is not a
 *            valid register.
 */
status_t RTC_HAL_ConfigureRegisterLock(RTC_Type * const base, rtc_lock_register_select_t registerToConfig);

/*!
 * @brief This function gets register lock status
 *
 * @param[in] base RTC base pointer
 * @param[in] reg The register for which to check lock status
 * @return    Return true if the register is locked, false if
 *            it is not locked
 */
bool RTC_HAL_IsRegisterLocked(const RTC_Type * const base, rtc_lock_register_select_t reg);


/*!
 * @brief This function configures the Clock Out pin source
 *
 * @param[in] base RTC base pointer
 * @param[in] config Source for the Clock Out pin
 * @return    Returns the status of the operation, STATUS_SUCCESS
 *            if the configuration was successful, STATUS_ERROR if the Control
 *            Register is locked.
 */
status_t RTC_HAL_ConfigureClockOut(RTC_Type * const base, rtc_clk_out_config_t config);

/*****************************************************************************
 * Methods for RTC Time Seconds register
 ****************************************************************************/

/*!
 * @brief Get Time Seconds Register Value
 *
 * @param[in] base RTC base pointer
 * @return    Number of seconds passed
 * Implements : RTC_HAL_GetTimeSecondsRegister_Activity
 */
static inline uint32_t RTC_HAL_GetTimeSecondsRegister(const RTC_Type * const base)
{
    uint32_t tmp = base->TSR;
    tmp = (tmp & RTC_TSR_TSR_MASK) >> RTC_TSR_TSR_SHIFT;
    return (uint32_t) (tmp);
}

/*!
 * @brief Set Time Seconds Register
 *
 * This function along with SetTimePrescalerRegister will help you set
 * the starting time at a specified value.
 * The write will fail if the Time Counter is enabled and will return
 * STATUS_ERROR, otherwise the return will be STATUS_SUCCESS
 *
 * @param[in] base    RTC base pointer
 * @param[in] seconds number of seconds passed
 * @return    STATUS_SUCCESS if the write is succeeded or STATUS_ERROR if
 *            the counter is enabled.
 */
status_t RTC_HAL_SetTimeSecondsRegister(RTC_Type * const base, uint32_t seconds);

/*****************************************************************************
 * Methods for RTC Time Prescaler register
 ****************************************************************************/

/*!
 * @brief Get Time Prescaler Register
 *
 * @param[in] base RTC base pointer
 * @return    Value stored in the Time Prescaler Register
 * Implements : RTC_HAL_GetTimePrescalerRegister_Activity
 */
static inline uint16_t RTC_HAL_GetTimePrescalerRegister(const RTC_Type * const base)
{
    uint32_t tmp = base->TPR;
    tmp = (tmp & RTC_TPR_TPR_MASK) >> RTC_TPR_TPR_SHIFT;
    return (uint16_t) (tmp);
}

/*!
 * @brief Set Time Prescaler Register.
 *
 * This function along with SetTimeSecondsRegister will help you set
 * the starting time at a specified value.
 * The write will fail if the Time Counter is enabled and will return
 * STATUS_ERROR, otherwise the return will be STATUS_SUCCESS
 *
 * @param[in] base RTC base pointer
 * @param[in] value Number of RTC CLK IN periods
 * @return    STATUS_SUCCESS if the write is succeeded or STATUS_ERROR if
 *            the counter is enabled.
 */
status_t RTC_HAL_SetTimePrescalerRegister(RTC_Type * const base, uint16_t value);


/*****************************************************************************
 * Methods for RTC Time Alarm register
 ****************************************************************************/

/*!
 * @brief Get Time Alarm Register
 *
 * @param[in] base RTC base pointer
 * @return    Value in seconds of the Time Alarm Register
 * Implements : RTC_HAL_GetTimeAlarmRegister_Activity
 */
static inline uint32_t RTC_HAL_GetTimeAlarmRegister(const RTC_Type * const base)
{
    uint32_t tmp = base->TAR;
    tmp = (tmp & RTC_TAR_TAR_MASK) >> RTC_TAR_TAR_SHIFT;
    return (uint32_t) (tmp);
}

/*!
 * @brief Set Time Alarm Register
 *
 * @param[in] base    RTC base pointer
 * @param[in] seconds Number of seconds at which the alarm is triggered. The TAR
 *                    value is correct only if the value is greater than current
 *                    time (Time seconds register)
 * Implements : RTC_HAL_SetTimeAlarmRegister_Activity
 */
static inline void RTC_HAL_SetTimeAlarmRegister(RTC_Type * const base, uint32_t seconds)
{
    base->TAR = seconds;
}

/*****************************************************************************
 * Methods for RTC Time Compensation register
 ****************************************************************************/

/*!
 * @brief Get Time Compensation Value and Interval
 *
 * The Time Prescaler register overflows at every 32768 - (compRegister)
 * cycles.
 * For example if the compRegister is -128 TPR overflows at
 * 32768 - (-128) = 32896 cycles.
 * Else if compRegister is 127 TPR overflows at 32641 cycles.
 * This correction is made at the interval configured by Compensation Interval
 *
 * @param[in] base                 RTC base pointer
 * @param[out] compensationValue    Compensation value
 * @param[out] compensationInterval Compensation Interval
 * @return None
 *
 * Implements : RTC_HAL_GetTimeCompensation_Activity
 */
static inline void RTC_HAL_GetTimeCompensation(const RTC_Type * const base,
                                               int8_t  * compensationValue,
                                               uint8_t * compensationInterval)
{
    uint32_t tmp = base->TCR;
    (*compensationValue)    = (int8_t)((tmp & RTC_TCR_TCR_MASK) >> RTC_TCR_TCR_SHIFT);
    (*compensationInterval) = (uint8_t)((tmp & RTC_TCR_CIR_MASK) >> RTC_TCR_CIR_SHIFT);
}

/*!
 * @brief Set Time Compensation
 *
 * Configure the frequency of the Time Seconds counter together with
 * Compensation Interval register.
 *
 * The Time Prescaler register overflows at every 32768 - (compValue)
 * cycles. For example if the compValue is -128 TPR overflows at
 * 32768 - (-128) = 32896 cycles
 *
 * Else if compValue is 127 TPR overflows at 32641 cycles
 *
 * The compensation interval in seconds from 1 to 256 is used to
 * control how frequently the TCR should adjust the number of 32.768 kHz
 * cycles in each second. The value written should be one less than the
 * number of seconds. For example, write zero to configure for a
 * compensation interval of one second. This register is double buffered
 * and writes do not take affect until the end of the current compensation
 * interval.
 *
 * @param[in] base RTC base pointer
 * @param[in] compensationValue - the value which is subtracted from the counter
 *                                valid range -128, +127
 * @param[in] compensationInterval Compensation interval at which the compensation value
 *                                is added to the prescaler register
 * @return    None
 * Implements : RTC_HAL_SetTimeCompensation_Activity
 */
static inline void RTC_HAL_SetTimeCompensation(RTC_Type * const base,
                                               int8_t  compensationValue,
                                               uint8_t compensationInterval)
{
    uint32_t tmp = base->TCR;
    tmp &= ~(RTC_TCR_TCR_MASK | RTC_TCR_CIR_MASK);
    tmp |= RTC_TCR_TCR(compensationValue);
    tmp |= RTC_TCR_CIR(compensationInterval);
    base->TCR = tmp;
}

/*!
 * @brief Get TimeCompensation Value and Interval
 *
 * Returns current value used by the compensation logic for the present
 * second interval. Updated once a second if the CIC equals 0 with the
 * contents of the TCR field. If the CIC does not equal zero then it
 * is loaded with zero.
 *
 * @param[in] base RTC base pointer
 * @param[out] compensationValue -  Current value which is subtracted from the counter
 *                                  valid range -128, +127
 * @param[out] compensationInterval Current Compensation interval at which the compensation value
 *                                  is added to the prescaler register
 * @return    Current value used by the compensation logic for the present second interval
 * Implements : RTC_HAL_GetCurrentTimeCompensation_Activity
 */
static inline void RTC_HAL_GetCurrentTimeCompensation(const RTC_Type * const base,
                                                      int8_t  * compensationValue,
                                                      uint8_t * compensationInterval)
{
    uint32_t tmp = base->TCR;
    (*compensationValue)    = (int8_t)((tmp & RTC_TCR_TCV_MASK) >> RTC_TCR_TCV_SHIFT);
    (*compensationInterval) = (uint8_t)((tmp & RTC_TCR_CIC_MASK) >> RTC_TCR_CIC_SHIFT);
}

/*****************************************************************************
 * Methods for RTC Control register
 ****************************************************************************/

/*!
 * @brief Select clock source for RTC prescaler
 *
 * When set, the RTC prescaler increments using the LPO 1kHz
 * clock and not the RTC 32kHz crystal clock. The LPO increments
 * the prescaler from bit TPR[5] (TPR[4:0] are ignored),
 * supporting close to 1 second increment of the seconds register.
 *
 * @param[in] base RTC base pointer
 * @param[in] clk_select clock source
 * Implements : RTC_HAL_SetLPOSelect_Activity
 */
static inline void RTC_HAL_SetLPOSelect(RTC_Type * const base, rtc_clk_select_t clk_select)
{
    uint32_t tmp = base->CR;
    tmp &= ~(RTC_CR_LPOS_MASK);
    tmp |= RTC_CR_LPOS(clk_select);
    base->CR = tmp;
}

/*!
 * @brief Get the selected clock source for RTC prescaler
 *
 * When set, the RTC prescaler increments using the LPO 1kHz
 * clock and not the RTC 32kHz crystal clock. The LPO increments
 * the prescaler from bit TPR[5] (TPR[4:0] are ignored),
 * supporting close to 1 second increment of the seconds register.
 *
 * @param[in] base RTC base pointer
 * @return    Selected clock source for RTC
 * Implements : RTC_HAL_GetLPOSelect_Activity
 */
static inline rtc_clk_select_t RTC_HAL_GetLPOSelect(const RTC_Type * const base)
{
    uint32_t tmp = base->CR;
    tmp = (tmp & RTC_CR_LPOS_MASK) >> RTC_CR_LPOS_SHIFT;
    return (rtc_clk_select_t)((tmp == 0u) ? RTC_CLK_SRC_OSC_32KHZ : RTC_CLK_SRC_LPO_1KHZ);
}

/*!
 * @brief Set Update Mode of the registers when locked
 *
 * @param[in] base RTC base pointer
 * @param[in] updateEnable value to be written in the register field
 *          - true to enable writing of the registers when locked
 *          - false to disable writing of the registers when locked
 * Implements : RTC_HAL_SetUpdateMode_Activity
 */
static inline void RTC_HAL_SetUpdateMode(RTC_Type * const base, bool updateEnable)
{
    uint32_t tmp = base->CR;
    tmp &= ~(RTC_CR_UM_MASK);
    tmp |= RTC_CR_UM((updateEnable ? 1UL : 0UL));
    base->CR = tmp;
}

/*!
 * @brief Get the Update Mode of the registers when locked
 *
 * @param[in] base RTC base pointer
 * @return    Update mode value
 *       -    true if writing of the registers when locked is enabled
 *       -    false if writing of the registers when locked is disabled
 * Implements : RTC_HAL_GetUpdateMode_Activity
 */
static inline bool RTC_HAL_GetUpdateMode(const RTC_Type * const base)
{
    uint32_t tmp = base->CR;
    tmp = (tmp & RTC_CR_UM_MASK) >> RTC_CR_UM_SHIFT;
    return ((tmp == 1U) ? true : false);
}

/*!
 * @brief Set Non-Supervisor access mode
 *
 * @param[in] base RTC base pointer
 * @param[in] enable supervisor access
 *          - if true Non-supervisor mode write accesses are supported.
 *          - if false Non-supervisor mode write accesses are not supported and
 *            generate a bus error.
 * Implements : RTC_HAL_SetNonSupervisorAccess_Activity
 */
static inline void RTC_HAL_SetNonSupervisorAccess(RTC_Type * const base, bool enable)
{
    uint32_t tmp = base->CR;
    tmp &= ~(RTC_CR_SUP_MASK);
    tmp |= RTC_CR_SUP(enable ? 1UL : 0UL);
    base->CR = tmp;
}

/*!
 * @brief Get Non-Supervisor access mode
 *
 * @param[in] base RTC base pointer
 * @return    supervisor access mode
 *      -     true if Non-supervisor mode write accesses are supported.
 *      -     false if Non-supervisor mode write accesses are not supported.
 * Implements : RTC_HAL_GetNonSupervisorAccess_Activity
 */
static inline bool RTC_HAL_GetNonSupervisorAccess(const RTC_Type * const base)
{
    uint32_t tmp = base->CR;
    tmp = (tmp & RTC_CR_SUP_MASK) >> RTC_CR_SUP_SHIFT;
    return ((tmp == 1U) ? true : false);
}

/*!
 * @brief Trigger a software reset
 *
 * @param[in] base RTC base pointer
 * Implements : RTC_HAL_SetSoftwareReset_Activity
 */
static inline void RTC_HAL_SetSoftwareReset(RTC_Type * const base)
{
    uint32_t tmp = base->CR;
    tmp &= ~(RTC_CR_SWR_MASK);
    tmp |= RTC_CR_SWR(1);
    base->CR = tmp;
}

/*!
 * @brief Clear Software reset flag
 *
 * @param[in] base RTC base pointer
 * Implements : RTC_HAL_ClearSoftwareReset_Activity
 */
static inline void RTC_HAL_ClearSoftwareReset(RTC_Type * const base)
{
    uint32_t tmp = base->CR;
    tmp &= ~(RTC_CR_SWR_MASK);
    base->CR = tmp;
}

/*****************************************************************************
 * Methods for RTC Status register
 ****************************************************************************/

/*!
 * @brief Enable or disable the Time counter
 *
 * When time counter is disabled the TSR register and TPR register are
 * writable, but do not increment.
 * When time counter is enabled the TSR register and TPR register are
 * not writable, but increment.
 *
 * @param[in] base RTC base pointer
 * @param[in] enable :
 *            - true to enable the counter
 *            - false to disable the counter
 * Implements : RTC_HAL_SetTimeCounterEnable_Activity
 */
static inline void RTC_HAL_SetTimeCounterEnable(RTC_Type * const base, bool enable)
{
    uint32_t tmp = base->SR;
    tmp &= ~(RTC_SR_TCE_MASK);
    tmp |= RTC_SR_TCE(enable ? 1UL : 0UL);
    base->SR = tmp;
}

/*!
 * @brief Get the Time Counter Enable value
 *
 * @param[in] base RTC base pointer
 * @return    State of the counter enable bit
 *      -     true if the counter is enabled
 *      -     false if the counter is disabled
 * Implements : RTC_HAL_GetTimeCounterEnable_Activity
 */
static inline bool RTC_HAL_GetTimeCounterEnable(const RTC_Type * const base)
{
    uint32_t tmp = base->SR;
    tmp = (tmp & RTC_SR_TCE_MASK) >> RTC_SR_TCE_SHIFT;
    return ((tmp == 1U) ? true : false);
}

/*!
 * @brief Get the Time alarm flag
 *
 * The alarm flag is cleared after a write in Time Alarm Register
 *
 * @param[in] base RTC base pointer
 * @return :  State of the alarm flag
 *      -     true if an alarm occurred
 *      -     false if an alarm was not occurred
 * Implements : RTC_HAL_GetTimeAlarmFlag_Activity
 */
static inline bool RTC_HAL_GetTimeAlarmFlag(const RTC_Type * const base)
{
    uint32_t tmp = base->SR;
    tmp = (tmp & RTC_SR_TAF_MASK) >> RTC_SR_TAF_SHIFT;
    return ((tmp == 1U) ? true : false);
}

/*!
 * @brief Get Time Overflow Flag
 *
 * The TOF is set when Time Seconds Register overflows. Disable the
 * counter and write TSR to clear this bit
 *
 * @param[in] base RTC base pointer
 * @return    State of the Time overflow flag
 *      -     true if an overflow has occurred
 *      -     false if an overflow has not occurred
 * Implements : RTC_HAL_GetTimeOverflowFlag_Activity
 */
static inline bool RTC_HAL_GetTimeOverflowFlag(const RTC_Type * const base)
{
    uint32_t tmp = base->SR;
    tmp = (tmp & RTC_SR_TOF_MASK) >> RTC_SR_TOF_SHIFT;
    return ((tmp == 1U) ? true : false);
}

/*!
 * @brief Get Time Invalid flag
 *
 * The time invalid flag is set on POR or software reset. The TSR and
 * TPR do not increment and read as zero when this bit is set. This bit
 * is cleared by writing the TSR register when the time counter is
 * disabled.
 *
 * @param[in] base RTC base pointer
 * @return    State of the time invalid flag
 *      -     true if TIF is set
 *      -     false if TIF is clear
 * Implements : RTC_HAL_GetTimeInvalidFlag_Activity
 */
static inline bool RTC_HAL_GetTimeInvalidFlag(const RTC_Type * const base)
{
    uint32_t tmp = base->SR;
    tmp = (tmp & RTC_SR_TIF_MASK) >> RTC_SR_TIF_SHIFT;
    return ((tmp == 1U) ? true : false);
}

/*****************************************************************************
 * Methods for RTC Lock register
 ****************************************************************************/

/*!
 * @brief Lock the Lock Register
 *
 * This method locks the Lock Register. If the register is locked, it can
 * be unlocked only with power-on reset(POR) or a software reset.
 *
 * @param[in] base RTC base pointer
 * Implements : RTC_HAL_LockRegisterLock_Activity
 */
static inline void RTC_HAL_LockRegisterLock(RTC_Type * const base)
{
    uint32_t tmp = base->LR;
    tmp &= ~(RTC_LR_LRL_MASK);
    base->LR = tmp;
}

/*!
 * @brief Get the Lock Register Lock state
 *
 * @param[in] base RTC base pointer
 * @return    State of the lock register lock
 *      -     true if register is locked
 *      -     false if the register is not locked
 * Implements : RTC_HAL_GetLockRegisterLock_Activity
 */
static inline bool RTC_HAL_GetLockRegisterLock(const RTC_Type * const base)
{
    uint32_t tmp = base->LR;
    tmp = (tmp & RTC_LR_LRL_MASK) >> RTC_LR_LRL_SHIFT;
    return ((tmp == 1U) ? false : true);
}

/*!
 * @brief Lock the Status Register
 *
 * This method locks the Status Register. If the register is locked, it can
 * be unlocked only with power-on reset(POR) or a software reset.
 *
 * @param[in] base RTC base pointer
 * Implements : RTC_HAL_StatusRegisterLock_Activity
 */
static inline void RTC_HAL_StatusRegisterLock(RTC_Type * const base)
{
    uint32_t tmp = base->LR;
    tmp &= ~(RTC_LR_SRL_MASK);
    base->LR = tmp;
}

/*!
 * @brief Get the Status Register Lock state
 *
 * @param[in] base RTC base pointer
 * @return    State of the status register lock
 *      -     true if register is locked
 *      -     false if the register is not locked
 * Implements : RTC_HAL_GetStatusRegisterLock_Activity
 */
static inline bool RTC_HAL_GetStatusRegisterLock(const RTC_Type * const base)
{
    uint32_t tmp = base->LR;
    tmp = (tmp & RTC_LR_SRL_MASK) >> RTC_LR_SRL_SHIFT;
    return ((tmp == 1U) ? false : true);
}

/*!
 * @brief Get the Control Register Lock state
 *
 * @param[in] base RTC base pointer
 * @return    State of the Control register lock
 *      -     true if register is locked
 *      -     false if the register is not locked
 * Implements : RTC_HAL_GetControlRegisterLock_Activity
 */
static inline bool RTC_HAL_GetControlRegisterLock(const RTC_Type * const base)
{
    uint32_t tmp = base->LR;
    tmp = (tmp & RTC_LR_CRL_MASK) >> RTC_LR_CRL_SHIFT;
    return ((tmp == 1U) ? false : true);
}

/*!
 * @brief Lock the Control Register
 *
 * This method locks the Control Register. If the register is locked,
 * it can be unlocked only with power-on reset(POR).
 *
 * @param[in] base RTC base pointer
 * Implements : RTC_HAL_ControlRegisterLock_Activity
 */
static inline void RTC_HAL_ControlRegisterLock(RTC_Type * const base)
{
    uint32_t tmp = base->LR;
    tmp &= ~(RTC_LR_CRL_MASK);
    base->LR = tmp;
}

/*!
 * @brief Get the TimeCompensation Register Lock state
 *
 * @param[in] base RTC base pointer
 * @return    State of the Time Compensation register lock
 *      -     true if register is locked
 *      -     false if the register is not locked
 * Implements : RTC_HAL_GetTimeCompensationLock_Activity
 */
static inline bool RTC_HAL_GetTimeCompensationLock(const RTC_Type * const base)
{
    uint32_t tmp = base->LR;
    tmp = (tmp & RTC_LR_TCL_MASK) >> RTC_LR_TCL_SHIFT;
    return ((tmp == 1U) ? false : true);
}

/*!
 * @brief Lock the TimeCompensation Register
 *
 * This method locks the TimeCompensation Register. If the register
 * is locked, it can be unlocked only with power-on reset(POR) or a
 * software reset.
 *
 * @param[in] base RTC base pointer
 * Implements : RTC_HAL_TimeCompensationLock_Activity
 */
static inline void RTC_HAL_TimeCompensationLock(RTC_Type * const base)
{
    uint32_t tmp = base->LR;
    tmp &= ~(RTC_LR_TCL_MASK);
    base->LR = tmp;
}

/*****************************************************************************
 * Methods for RTC Interrupt Enable register
 ****************************************************************************/

/*!
 * @brief Configure Time Seconds interrupt
 *
 * @param[in] base RTC base pointer
 * @param[in] intCfg Select at which frequency the interrupt
 *                   will occur.
 * Implements : RTC_HAL_SetTimeSecondsIntConf_Activity
 */
static inline void RTC_HAL_SetTimeSecondsIntConf(RTC_Type * const base, rtc_second_int_cfg_t intCfg)
{
    uint32_t tmp = base->IER;
    tmp &= ~(RTC_IER_TSIC_MASK);
    tmp |= RTC_IER_TSIC((uint8_t)intCfg);
    base->IER = tmp;
}

/*!
 * @brief Get Time Seconds interrupt configuration
 *
 * @param[in] base RTC base pointer
 * @return    Time Seconds interrupt configuration
 * Implements : RTC_HAL_GetTimeSecondsIntConf_Activity
 */
static inline rtc_second_int_cfg_t RTC_HAL_GetTimeSecondsIntConf(const RTC_Type * const base)
{
    rtc_second_int_cfg_t returnValue = RTC_INT_1HZ;
    uint32_t tmp = base->IER;
    tmp = (tmp & RTC_IER_TSIC_MASK) >> RTC_IER_TSIC_SHIFT;

    switch (tmp)
    {
        case 0x00U:
            returnValue = RTC_INT_1HZ;
            break;
        case 0x01U:
            returnValue = RTC_INT_2HZ;
            break;
        case 0x02U:
            returnValue = RTC_INT_4HZ;
            break;
        case 0x03U:
            returnValue = RTC_INT_8HZ;
            break;
        case 0x04U:
            returnValue = RTC_INT_16HZ;
            break;
        case 0x05U:
            returnValue = RTC_INT_32HZ;
            break;
        case 0x06U:
            returnValue = RTC_INT_64HZ;
            break;
        case 0x07U:
            returnValue = RTC_INT_128HZ;
            break;
        default:
            /* This case should not be reached */
            break;
    }

    return returnValue;
}

/*!
 * @brief Enable TimeSeconds interrupt
 *
 * @param[in] base RTC base pointer
 * @param[in] enable Write:
 *      -     true to enable the interrupt
 *      -     false to disable it
 * Implements : RTC_HAL_SetTimeSecondsIntEnable_Activity
 */
static inline void RTC_HAL_SetTimeSecondsIntEnable(RTC_Type * const base, bool enable)
{
    uint32_t tmp = base->IER;
    tmp &= ~(RTC_IER_TSIE_MASK);
    tmp |= RTC_IER_TSIE(enable ? 1UL : 0UL);
    base->IER = tmp;
}

/*!
 * @brief Get the TimeSeconds interrupt enable status
 *
 * @param[in] base RTC base pointer
 * @return    The TimeSeconds interrupt enablement
 *      -     true if interrupt is enabled
 *      -     false if interrupt is disabled
 * Implements : RTC_HAL_GetTimeSecondsIntEnable_Activity
 */
static inline bool RTC_HAL_GetTimeSecondsIntEnable(const RTC_Type * const base)
{
    uint32_t tmp = base->IER;
    tmp = (tmp & RTC_IER_TSIE_MASK) >> RTC_IER_TSIE_SHIFT;
    return ((tmp == 1U) ? true : false);
}

/*!
 * @brief Enable TimeAlarm interrupt
 *
 * @param[in] base RTC base pointer
 * @param[in] enable Write
 *      -     true to enable the interrupt
 *      -     false to disable it
 * Implements : RTC_HAL_SetTimeAlarmIntEnable_Activity
 */
static inline void RTC_HAL_SetTimeAlarmIntEnable(RTC_Type * const base, bool enable)
{
    uint32_t tmp = base->IER;
    tmp &= ~(RTC_IER_TAIE_MASK);
    tmp |= RTC_IER_TAIE(enable ? 1UL : 0UL);
    base->IER = tmp;
}

/*!
 * @brief Get the TimeAlarm interrupt enable status
 *
 * @param[in] base RTC base pointer
 * @return    The TimeAlarm interrupt enablement
 *      -     true if interrupt is enabled
 *      -     false if interrupt is disabled
 * Implements : RTC_HAL_GetTimeAlarmIntEnable_Activity
 */
static inline bool RTC_HAL_GetTimeAlarmIntEnable(const RTC_Type * const base)
{
    uint32_t tmp = base->IER;
    tmp = (tmp & RTC_IER_TAIE_MASK) >> RTC_IER_TAIE_SHIFT;
    return ((tmp == 1U) ? true : false);
}

/*!
 * @brief Enable TimeOverflow interrupt
 *
 * @param[in] base RTC base pointer
 * @param[in] enable Write
 *          - true to enable the interrupt
 *          - false to disable it
 * Implements : RTC_HAL_SetTimeOverflowIntEnable_Activity
 */
static inline void RTC_HAL_SetTimeOverflowIntEnable(RTC_Type * const base, bool enable)
{
    uint32_t tmp = base->IER;
    tmp &= ~(RTC_IER_TOIE_MASK);
    tmp |= RTC_IER_TOIE(enable ? 1UL : 0UL);
    base->IER = tmp;
}

/*!
 * @brief Get the TimeAlarm interrupt enable status
 *
 * @param[in] base RTC base pointer
 * @return     The TimeOverflow interrupt enablement
 *           - true if interrupt is enabled
 *           - false if interrupt is disabled
 * Implements : RTC_HAL_GetTimeOverflowIntEnable_Activity
 */
static inline bool RTC_HAL_GetTimeOverflowIntEnable(const RTC_Type * const base)
{
    uint32_t tmp = base->IER;
    tmp = (tmp & RTC_IER_TOIE_MASK) >> RTC_IER_TOIE_SHIFT;
    return ((tmp == 1U) ? true : false);
}

/*!
 * @brief Enable TimeInvalid interrupt
 *
 * @param[in] base RTC base pointer
 * @param[in] enable Write
 *          - true to enable the interrupt
 *          - false to disable it
 * Implements : RTC_HAL_SetTimeInvalidIntEnable_Activity
 */
static inline void RTC_HAL_SetTimeInvalidIntEnable(RTC_Type * const base, bool enable)
{
    uint32_t tmp = base->IER;
    tmp &= ~(RTC_IER_TIIE_MASK);
    tmp |= RTC_IER_TIIE(enable ? 1UL : 0UL);
    base->IER = tmp;
}

/*!
 * @brief Get the TimeInvalid interrupt enable status
 *
 * @param[in] base RTC base pointer
 * @return    The TimeInvalid interrupt enablement
 *          - true if interrupt is enabled
 *          - false if interrupt is disabled
 * Implements : RTC_HAL_GetTimeInvalidIntEnable_Activity
 */
static inline bool RTC_HAL_GetTimeInvalidIntEnable(const RTC_Type * const base)
{
    uint32_t tmp = base->IER;
    tmp = (tmp & RTC_IER_TIIE_MASK) >> RTC_IER_TIIE_SHIFT;
    return ((tmp == 1U) ? true : false);
}

/*! @}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* RTC_HAL_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
