#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "hc_sr04.h"
#include "hal/adc_types.h"

/* USER INCLUDES */
#include "i2c.h"
#include "espnow_rec.h"
#include "pwm.h"
#include "uart.h"

extern "C" {void app_main(void) {
  //TODO add adc read tester
  //TODO add mpu read tester
  hc_sr04_config_t config = {
    .trigger_pin = GPIO_NUM_4,
    .echo_pin = GPIO_NUM_5,
    .timeout_us = 30000
  };

  hc_sr04_handle_t sensor = hc_sr04_init(&config);

  pwm_init();

  uint8_t i = 0;
  float actual_distance = 0;

  while (1) {
    i++;
    
    actual_distance = hc_sr04_measure_cm(sensor);
    setPWM(i);

    printf("actual_distance: %f\n", actual_distance);
    vTaskDelay(portTICK_PERIOD_MS / 100);
  }
}}
