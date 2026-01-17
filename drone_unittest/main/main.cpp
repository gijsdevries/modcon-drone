#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "hc_sr04.h"

/* USER INCLUDES */
#include "i2c.h"
#include "espnow_rec.h"
#include "pwm.h"
#include "adc.h"
#include "uart.h"

extern "C" {void app_main(void) {
  //TODO add mpu read tester

  adc_init();
  
  hc_sr04_config_t config_hc = {
    .trigger_pin = GPIO_NUM_4,
    .echo_pin = GPIO_NUM_5,
    .timeout_us = 30000
  };

  hc_sr04_handle_t sensor = hc_sr04_init(&config_hc);

  pwm_init();

  setPWM(0);
  vTaskDelay(5000 / portTICK_PERIOD_MS); 

  uint8_t i = 75;
  float actual_distance = 0;

  int adc;

  while (1) {
    adc = read_adc();
    actual_distance = (float)hc_sr04_measure_cm(sensor);
    setPWM(i);

    printf("adc: %d   actual_distance: %f   pwm: %d\n", adc, actual_distance, i);

    vTaskDelay(100 / portTICK_PERIOD_MS); 
  }
}}
