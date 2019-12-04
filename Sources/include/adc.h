#ifndef _ADC_H_
#define _ADC_H_

#include <stdbool.h>
#include <thres_diag_futian.h>

unsigned int driverInitADC(void);
void ADCSet(void);
void ADCPolling(void);
unsigned int adc_Gps_H(void);
unsigned int adc_Gps_L(void);
unsigned int adc_Temperature(void);
unsigned int adc_Abat(void);
unsigned int adc_Bbat(void);
unsigned int adc_ADC_BAT(void);
unsigned int adc_VMIC(void);


unsigned int adc_MIC_POSITIVE(void);
unsigned int adc_MIC_NEGATIVE(void);
unsigned int adc_get_call_type(void);
unsigned int adc_spk_positive(void);
unsigned int adc_spk_negative(void);
void spk_check_switch(bool enable);
void set_spk_test_flag(bool enable);
bool get_spk_test_flag(void);
void set_spk_mic_test(void);
bool get_mic_test_flag(void);
void set_mic_test_flag(bool enable);

void spk_close(void);
void adc_Abat_init(void);

#endif

