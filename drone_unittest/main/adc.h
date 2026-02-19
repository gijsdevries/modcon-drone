#ifndef ADC_H 
#define ADC_H

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define EXAMPLE_ADC_ATTEN           ADC_ATTEN_DB_12
#define EXAMPLE_ADC1_CHAN0          ADC_CHANNEL_4

void adc_init(void);
int read_adc();

#endif
