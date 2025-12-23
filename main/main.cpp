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

//TODO calibrate these values
#define KP 1
#define KI 0.1
#define KD 0.1
#define dT 0.01

#define BUILTIN_LED (gpio_num_t)2
#define DEBUG

float error, error_sum, error_div, error_prev, desired_distance, actual_distance, pwm;

extern "C" {void app_main(void) {
  desired_distance = 150;
  //  esp_now_full_init();

  hc_sr04_config_t config = {
    .trigger_pin = GPIO_NUM_4,
    .echo_pin = GPIO_NUM_5,
    .timeout_us = 30000
  };
  hc_sr04_handle_t sensor = hc_sr04_init(&config);

  pwm_init();

  gpio_reset_pin(BUILTIN_LED);
  gpio_set_direction(BUILTIN_LED, GPIO_MODE_OUTPUT);

  while (1) {
    actual_distance = hc_sr04_measure_cm(sensor); 

    error = desired_distance - actual_distance;
    actual_distance = hc_sr04_measure_cm(sensor); 
    error_sum += error * dT;
    error_div = (error - error_prev) / dT;
    pwm = error*KP;
    error_prev = error;

    setPWM(pwm);

    vTaskDelay((1000*dT) / portTICK_PERIOD_MS);

#ifdef DEBUG
    static int i = 0;
    i++;
    if (i > 250) {
      printf("--- Measurements ---\nerror: %f   error_sum: %f   error_div: %f   error_prev: %f    desired_distance: %f    actual_distance: %f   pwm: %f\n\n\n", 
      error, error_sum, error_div, error_prev, desired_distance, actual_distance, pwm);
      i = 0;
    } 
#endif
  }
}}
