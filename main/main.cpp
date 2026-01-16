#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "hc_sr04.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

/* USER INCLUDES */
#include "i2c.h"
#include "espnow_rec.h"
#include "pwm.h"
#include "uart.h"

#define EXAMPLE_ADC_ATTEN           ADC_ATTEN_DB_12
#define EXAMPLE_ADC1_CHAN0          ADC_CHANNEL_4

extern "C" {void app_main(void) {
  //TODO add adc read tester
  //TODO add mpu read tester

  adc_oneshot_unit_handle_t adc1_handle;
  adc_oneshot_unit_init_cfg_t init_config1 = {
    .unit_id = ADC_UNIT_1,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

  adc_oneshot_chan_cfg_t config = {
    .atten = EXAMPLE_ADC_ATTEN,
    .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN0, &config));


  hc_sr04_config_t config_hc = {
    .trigger_pin = GPIO_NUM_4,
    .echo_pin = GPIO_NUM_5,
    .timeout_us = 30000
  };

  hc_sr04_handle_t sensor = hc_sr04_init(&config_hc);

  pwm_init();

  uint8_t i = 0;
  float actual_distance = 0;
  static int adc_raw[2][10];

  while (1) {

    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN0, &adc_raw[0][0]));
    printf("ADC%d Channel[%d] Raw Data: %d\n", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN0, adc_raw[0][0]);


    i++;

    //actual_distance = hc_sr04_measure_cm(sensor);
    setPWM(i);

    //printf("actual_distance: %f\n", actual_distance);
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
  }
}}
