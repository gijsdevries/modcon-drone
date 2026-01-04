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

#define dT 0.01
#define BUILTIN_LED (gpio_num_t)2
#define DEBUG

float error, error_sum, error_div, error_prev, desired_distance, actual_distance, pwm, output;
bool operation_state;
float kp, ki, kd;

extern "C" {void app_main(void) {
  desired_distance = 0.5;
  operation_state = false;

  kp = 0;
  ki = 0;
  kd = 0;

  pid_struct pid_struct;
  pid_struct.msg_type = PID_DRONE;

  pwm = BIT_16_MAX * 0.05;
  esp_now_full_init();

  hc_sr04_config_t config = {
    .trigger_pin = GPIO_NUM_4,
    .echo_pin = GPIO_NUM_5,
    .timeout_us = 30000
  };
  hc_sr04_handle_t sensor = hc_sr04_init(&config);

  pwm_init();

  gpio_reset_pin(BUILTIN_LED);
  gpio_set_direction(BUILTIN_LED, GPIO_MODE_OUTPUT);

  setPWM(0);
  vTaskDelay(3000 / portTICK_PERIOD_MS);

  bool led_state = true;

  while (kp == 0 && ki == 0 && kd == 0) {
    gpio_set_level((gpio_num_t)2, led_state);
    led_state = !led_state; 
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
  printf("recieved PID values:\nkp = %f\nki = %f\nkd = %f\n", kp, ki, kd);

  while (1) {
    gpio_set_level((gpio_num_t)2, operation_state);

    if (operation_state == false) {
      setPWM(0);
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    else {
      actual_distance = (float)hc_sr04_measure_cm(sensor); 

      if (actual_distance < 0)
        actual_distance = -1;
      else if (actual_distance < 5 || actual_distance > 200)
        actual_distance = 5;

      error = desired_distance - actual_distance;
      error_sum += error * dT;
      error_div = (error - error_prev) / dT;
      output = error*kp + error*ki + error*kd;
      error_prev = error;

      pwm *= output;

      if (pwm > BIT_16_MAX * 0.10)
        pwm = BIT_16_MAX * 0.10;
      else if (pwm < 1)
        pwm = 1;

      setPWM(pwm);

      vTaskDelay((1000*dT) / portTICK_PERIOD_MS);

#ifdef DEBUG
      static int i = 0;
      i++;
      if (i > 250) {
        pid_struct.error = error;
        pid_struct.error_sum = error_sum;
        pid_struct.error_div = error_div;
        pid_struct.error_prev = error_prev;
        pid_struct.desired_distance = desired_distance;
        pid_struct.actual_distance = actual_distance;
        pid_struct.pwm = pwm;
        pid_struct.output = output;

      printf("                distance: %f\n", actual_distance);

        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &pid_struct, sizeof(pid_struct));

        if (result == ESP_OK) {
          printf("PID debug info send succes");
        }
        else {
          printf("PID debug info send fail");
        }
        i = 0;
      } 
#endif
    }
  }
}}
