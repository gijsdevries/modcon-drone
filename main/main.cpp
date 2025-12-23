#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_now.h>
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "hc_sr04.h"

/* USER INCLUDES */
#include "i2c.h"
#include "espnow_rec.h"
#include "pwm.h"

uint8_t desired_distance;
uint8_t actual_distance;
uint8_t pwm;

#define DEBUG

extern "C" {void app_main(void) {
  esp_now_full_init();

  hc_sr04_config_t config = {
    .trigger_pin = GPIO_NUM_4,
    .echo_pin = GPIO_NUM_5,
    .timeout_us = 30000
  };
  hc_sr04_handle_t sensor = hc_sr04_init(&config);

  pwm_init();

  while (1) {
    actual_distance = (uint8_t)hc_sr04_measure_cm(sensor); 

    if (desired_distance < actual_distance && pwm > 1) {
      pwm--;
    }
    else if (desired_distance > actual_distance && pwm < 255) {
      pwm++;
    }
    setPWM(pwm);

#ifdef DEBUG
    static int i = 0;
    i++;
    if (i > 50) {
      printf("desired distance: %d    actual distance: %d   pwm: %d\n", desired_distance, actual_distance, pwm);
      i = 0;
    } 
#endif

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}}
