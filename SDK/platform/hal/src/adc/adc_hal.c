/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
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
 * @file adc_hal.c
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External could be made static.
 * Function is defined for usage by application code.
 *
 */

#include "adc_hal.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_HAL_Init
 * Description   : This function initializes the ADC instance to a known
 * state (the register are written with their reset values from the Reference
 * Manual).
 *
 * Implements : ADC_HAL_Init_Activity
 *END**************************************************************************/
void ADC_HAL_Init(ADC_Type * const baseAddr)
{
    baseAddr->SC1[0U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->SC1[1U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->SC1[2U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->SC1[3U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->SC1[4U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->SC1[5U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->SC1[6U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->SC1[7U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->SC1[8U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->SC1[9U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->SC1[10U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->SC1[11U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->SC1[12U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->SC1[13U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->SC1[14U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->SC1[15U] = ADC_SC1_ADCH(ADC_INPUTCHAN_DISABLED) | ADC_SC1_AIEN(0x00U);
    baseAddr->CFG1 = ADC_CFG1_ADICLK(ADC_CLK_ALT_1) | ADC_CFG1_MODE(ADC_RESOLUTION_8BIT) | ADC_CFG1_ADIV(ADC_CLK_DIVIDE_1);
    baseAddr->CFG2 = ADC_CFG2_SMPLTS(0x0CU);
    baseAddr->CV[0U] = ADC_CV_CV(0U);
    baseAddr->CV[1U] = ADC_CV_CV(0U);
    baseAddr->SC2 = ADC_SC2_REFSEL(ADC_VOLTAGEREF_VREF) | ADC_SC2_DMAEN(0x00U) | ADC_SC2_ACREN(0x00U) | ADC_SC2_ACFGT(0x00U) | ADC_SC2_ACFE(0x00U) |
                    ADC_SC2_ADTRG(0x00U);
    baseAddr->SC3 = ADC_SC3_AVGS(ADC_AVERAGE_4) | ADC_SC3_AVGE(0x00U) | ADC_SC3_ADCO(0x00U) | ADC_SC3_CAL(0x00U);
    baseAddr->USR_OFS = ADC_USR_OFS_USR_OFS(0U);
    baseAddr->UG = ADC_UG_UG(4U);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
