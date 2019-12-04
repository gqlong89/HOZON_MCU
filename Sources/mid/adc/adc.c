#include <adc.h>
#include <adc_driver.h>
#include <adc_hal.h>
#include <gpio_hal.h>
#include <osif.h>
#include <rtc.h>
#include <stdbool.h>
#include <sys/_stdint.h>
#include <gpio.h>
#include <thres_diag_futian.h>


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define ADC0_INSTANCE   (0UL)
#define ADC0_CHANNEL    (0UL)
#define ADC1_INSTANCE   (1UL)
#define ADC1_CHANNEL    (0UL)

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile unsigned int g_conversionResult;

#define ADC_MAX_BUF        (10)
#define ADC_VIN_TEST_MAX    (3)
#define ADC_INDEX_NO        (3)

#define ADC_SET_TIMEOUT_VAL     500 //5s

static unsigned int ADCValuePtr[ADC_INDEX_NO];
static unsigned int ADC_Index[ADC_INDEX_NO];
static unsigned int ADCBuf[ADC_INDEX_NO][ADC_MAX_BUF];

static unsigned int TestNo0 = 0;        //更改循环策略，将ADC0 ADC1同时进行转化
static unsigned int TestNo1 = 0;

static unsigned int Gps_H = 0;
static unsigned int Gps_L = 0;

static unsigned int ADC_TEMP = 0;

static unsigned int ADC_VMIC = 0;
static unsigned int ADC_MIC_POSITIVE = 0;

static unsigned int ADC_CALL_TYPE = 0;  //SOS BTN

static unsigned int ADC_SPK_P = 0;      //added by Cindy
static unsigned int ADC_SPK_N = 0;
volatile bool spk_test_flag = false;
volatile bool mic_test_flag = true;

volatile bool adcConvDone = true;

static unsigned int adcInitFlag = 0;

adc_chan_config_t adc0_ChnCfg =
{
    .interruptEnable = false,
    .channel = ADC_INPUTCHAN_AD31,
};

adc_chan_config_t adc1_ChnCfg =
{
    .interruptEnable = false,
    .channel = ADC_INPUTCHAN_AD31,
};

/*******************************************************************************
 * Code
 ******************************************************************************/

/* ADC start */
void ADC0Start(unsigned char no)
{
    adc0_ChnCfg.channel = no;
    ADC_DRV_ConfigChan(ADC0_INSTANCE, ADC0_CHANNEL, &adc0_ChnCfg);
}

void ADC1Start(unsigned char no)
{
    adc1_ChnCfg.channel = no;
    ADC_DRV_ConfigChan(ADC1_INSTANCE, ADC1_CHANNEL, &adc1_ChnCfg);
}

/*ADC0 5通道*/
void ADCSet0(void)
{
    switch (TestNo0)
    {
        case 0:
            ADC0Start(2);
            TestNo0++;
            break;

        case 1:
            ADC0Start(8);
            TestNo0++;
            break;

        case 2:
            ADC0Start(9);
            TestNo0++;
            break;

        case 3:
            ADC0Start(12);
            TestNo0++;
            break;

        case 4:
            ADC0Start(13);
            TestNo0 = 0;
            break;

        default:
            break;
    }
}


/*ADC1 JL : 2通道；FT: 6通道*/
void ADCSet1(void)
{
    switch (TestNo1)
    {
        case 0:
            ADC1Start(2);
            TestNo1++;
            break;

        case 1:
            ADC1Start(7);
            #if FUTIAN_DIAG_ENABLE
            {
                TestNo1++;
            }
            #else
            {
                TestNo1 = 0;
            }

            #endif
            break;

            #if FUTIAN_DIAG_ENABLE

        case 2:
            ADC1Start(8);    //MIC+
            TestNo1++;
            break;

        case 3:
            ADC1Start(3);   //SOS
            TestNo1++;
            break;

        case 4:
            if (get_spk_test_flag())
            {
                spk_check_switch(true);
            }

            ADC1Start(4);  //ADC1_4 SPK+ added by Cindy
            TestNo1++;
            break;

        case 5:
            ADC1Start(15);  //ADC1_15 SPK-  added by Cindy
            TestNo1 = 0;
            break;
            #endif

        default:
            break;
    }
}

void ADCPolling(void)
{
    unsigned int i;
    unsigned int tmpVal = 0;
    static uint64_t tempUptime = 0;
    static unsigned int count[ADC_INDEX_NO] = { 0 };
    unsigned int adc_completeflag[2] = { 0 };


    if (0 == adcInitFlag)
    {
        return;
    }

    if (0 == tempUptime)
    {
        tempUptime = driverGetUptime();
        ADCSet0();
        ADCSet1();
    }

    adc_completeflag[0] = 0;

    if (0U != ADC_DRV_GetConvCompleteFlag(ADC0_INSTANCE, ADC0_CHANNEL))
    {
        ADC_DRV_GetChanResult(ADC0_INSTANCE, ADC0_CHANNEL, (uint16_t *) &g_conversionResult);

        adc_completeflag[0] = 1;

        //printk( PRINTK_LEVEL_LOG,"ADC:(%d),channelNumber: (%d),ADC Value: (%d)", ADC0_INSTANCE,adc0_ChnConfig0.channel, g_conversionResult);

        switch (adc0_ChnCfg.channel)
        {
            case 2:
                ADC_TEMP = g_conversionResult;
                break;

            case 8:
                ADCValuePtr[1] = g_conversionResult;
                ADCBuf[1][ADC_Index[1]] = g_conversionResult;

                ADC_Index[1] = (ADC_Index[1] + 1) % ADC_MAX_BUF;

                tmpVal = 0;

                if (count[1] > ADC_MAX_BUF)
                {
                    for (i = 0; i < ADC_MAX_BUF; i++)
                    {
                        tmpVal += ADCBuf[1][i];
                    }

                    ADCValuePtr[1] = tmpVal / ADC_MAX_BUF;
                }
                else
                {
                    count[1]++;
                }

                break;

            case 9:
                ADCValuePtr[2] = g_conversionResult;
                ADCBuf[2][ADC_Index[2]] = g_conversionResult;

                ADC_Index[2] = (ADC_Index[2] + 1) % ADC_MAX_BUF;

                tmpVal = 0;

                if (count[2] > 10)
                {
                    for (i = 0; i < ADC_MAX_BUF; i++)
                    {
                        tmpVal += ADCBuf[2][i];
                    }

                    ADCValuePtr[2] = tmpVal / ADC_MAX_BUF;
                }
                else
                {
                    count[2]++;
                }

                break;

            case 12:
                Gps_L = g_conversionResult;
                break;

            case 13:
                Gps_H = g_conversionResult;
                break;

            default:
                break;
        }

        //        printf("adc0_ChnCfg.channel = %d, g_conversionResult = %d\r\n", adc0_ChnCfg.channel,  g_conversionResult);
    }

    adc_completeflag[1] = 0;

    if (0U != ADC_DRV_GetConvCompleteFlag(ADC1_INSTANCE, ADC1_CHANNEL))
    {
        ADC_DRV_GetChanResult(ADC1_INSTANCE, ADC1_CHANNEL, (uint16_t *) &g_conversionResult);
        adc_completeflag[1] = 1;

        //printk( PRINTK_LEVEL_LOG,"ADC:(%d),channelNumber: (%d),ADC Value: (%d)", ADC1_INSTANCE,adc1_ChnConfig0.channel, g_conversionResult);

        switch (adc1_ChnCfg.channel)
        {
            case 2:
                ADCValuePtr[0] = g_conversionResult;


                ADCBuf[0][ADC_Index[0]] = g_conversionResult;

                ADC_Index[0] = (ADC_Index[0] + 1) % ADC_VIN_TEST_MAX;

                tmpVal = 0;

                if (count[0] > ADC_VIN_TEST_MAX)
                {
                    for (i = 0; i < ADC_VIN_TEST_MAX; i++)
                    {
                        tmpVal += ADCBuf[0][i];
                    }

                    ADCValuePtr[0] = tmpVal / ADC_VIN_TEST_MAX;
                }
                else
                {
                    count[0]++;
                }

                //              printf("ADCValuePtr[0] = %d\r\n", ADCValuePtr[0]);

                break;

            case 7:
                ADC_VMIC = g_conversionResult;
                break;

                #if FUTIAN_DIAG_ENABLE

            case 8:
                if (get_mic_test_flag())
                {
                    ADC_MIC_POSITIVE = g_conversionResult;
                }

                break;

            case 3:
                ADC_CALL_TYPE = g_conversionResult;
                break;

            case 4:
                if (get_spk_test_flag())
                {
                    ADC_SPK_P = g_conversionResult;     //added by Cindy SPK+
                }

                break;

            case 15:
                if (get_spk_test_flag())
                {
                    ADC_SPK_N = g_conversionResult;     //added by Cindy SPK-
                }

                break;
                #endif

            default:
                break;
        }

        //        printf("adc1_ChnCfg.channel = %d, g_conversionResult = %d\r\n", adc1_ChnCfg.channel,  g_conversionResult);
    }

    if (1U == adc_completeflag[0])
    {
        ADCSet0();
        tempUptime = driverGetUptime();
    }
    else if ((driverGetUptime() - tempUptime) > ADC_SET_TIMEOUT_VAL)
    {
        ADCSet0();
        tempUptime = driverGetUptime();
    }

    if (1U == adc_completeflag[1])
    {
        ADCSet1();
        tempUptime = driverGetUptime();
    }
    else if ((driverGetUptime() - tempUptime) > ADC_SET_TIMEOUT_VAL)
    {
        ADCSet1();
        tempUptime = driverGetUptime();
    }
}

unsigned int driverInitADC(void)
{
    /*! adc0 configuration structure */
    const adc_converter_config_t adcConvCfg =
    {
        .clockDivide = ADC_CLK_DIVIDE_1,
        .sampleTime = 12U,
        .resolution = ADC_RESOLUTION_12BIT,
        .inputClock = ADC_CLK_ALT_1,
        .trigger = ADC_TRIGGER_SOFTWARE,
        .dmaEnable = false,
        .voltageRef = ADC_VOLTAGEREF_VREF,
        .continuousConvEnable = false,
    };


    ADC_DRV_ConfigConverter(ADC0_INSTANCE, &adcConvCfg);
    ADC_DRV_AutoCalibration(ADC0_INSTANCE);
    ADC_DRV_ConfigConverter(ADC1_INSTANCE, &adcConvCfg);
    ADC_DRV_AutoCalibration(ADC1_INSTANCE);

    adcInitFlag = 1;

    return 0;
}

unsigned int adc_Gps_H(void)
{
    return Gps_H;
}

unsigned int adc_Gps_L(void)
{
    return Gps_L;
}

unsigned int adc_Temperature(void)
{
    return ADC_TEMP;
}

unsigned int adc_Abat(void)
{
    return ADCValuePtr[0];
}

unsigned int adc_Bbat(void)
{
    return ADCValuePtr[2];
}

unsigned int adc_ADC_BAT(void)
{
    return ADCValuePtr[1];
}

unsigned int adc_VMIC(void)
{
    return ADC_VMIC;
}

unsigned int adc_MIC_POSITIVE(void)
{
    return ADC_MIC_POSITIVE;
}

unsigned int adc_MIC_NEGATIVE(void)
{
    return 600;
}

unsigned int adc_get_call_type(void)
{
    return ADC_CALL_TYPE;
}

unsigned int adc_spk_positive(void)
{
    return ADC_SPK_P;
}

unsigned int adc_spk_negative(void)
{
    return ADC_SPK_N;
}

void spk_check_switch(bool enable)
{
    if (enable)
    {
        GPIO_HAL_WritePin(GP_PA_EN, GP_PA_EN_PIN_OUT, 0U);
        GPIO_HAL_WritePin(GP_LS_TEST_EN1, GP_LS_TEST_EN1_PIN_OUT, 0U);
        GPIO_HAL_WritePin(GP_LS_TEST_EN2, GP_LS_TEST_EN2_PIN_OUT, 1U);
        set_spk_test_flag(ENABLE);
    }
    else
    {
        GPIO_HAL_WritePin(GP_PA_EN, GP_PA_EN_PIN_OUT, 1U);
        GPIO_HAL_WritePin(GP_LS_TEST_EN1, GP_LS_TEST_EN1_PIN_OUT, 1U);
        GPIO_HAL_WritePin(GP_LS_TEST_EN2, GP_LS_TEST_EN2_PIN_OUT, 0U);
        set_spk_test_flag(DISABLE);
    }
}

void spk_close(void)
{
    GPIO_HAL_WritePin(GP_PA_EN, GP_PA_EN_PIN_OUT, 0U);
    GPIO_HAL_WritePin(GP_LS_TEST_EN1, GP_LS_TEST_EN1_PIN_OUT, 1U);
    GPIO_HAL_WritePin(GP_LS_TEST_EN2, GP_LS_TEST_EN2_PIN_OUT, 0U);
}

void set_spk_test_flag(bool enable)
{
    spk_test_flag = enable;
}

bool get_spk_test_flag(void)
{
    return spk_test_flag;
}

void set_spk_mic_test(void)
{
    #ifdef FUTIAN_DIAG_ENABLE
    //    set_spk_test_flag(true);
    set_spk_test_flag(false);
    set_mic_test_flag(true);
    TestNo1 = 0;
    #endif
}

bool get_mic_test_flag(void)
{
    return mic_test_flag;
}

void set_mic_test_flag(bool enable)
{
    mic_test_flag = enable;
}

void adc_Abat_init(void)
{
    unsigned int  g_conversionResult1 = 0;

    static uint16_t abat_count = 0;

    driverInitADC();

    ADC1Start(2);

    while (1)
    {
        if (0U != ADC_DRV_GetConvCompleteFlag(ADC1_INSTANCE, ADC1_CHANNEL))
        {
            abat_count = 0;
            ADC_DRV_GetChanResult(ADC1_INSTANCE, ADC1_CHANNEL, (uint16_t *) &g_conversionResult1);
            abat_count++;
            break;
        }

        abat_count++;

        if (abat_count > 30)
        {
            abat_count = 0;
            break;
        }
    }

    ADCValuePtr[0] = g_conversionResult1;

}
