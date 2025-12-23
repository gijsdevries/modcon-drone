#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_now.h>
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

/* USER INCLUDES */
#include "i2c.h"
#include "hc_sr04.h"


extern "C" {void app_main(void) {
  hc_sr04_config_t config = {
    .trigger_pin = GPIO_NUM_4,
    .echo_pin = GPIO_NUM_5,
    .timeout_us = 30000
  };
  hc_sr04_handle_t sensor = hc_sr04_init(&config);


  while(1) {
    float distance = hc_sr04_measure_cm(sensor);
    printf("distance: %f\n", distance);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}}

/*

/* USER INCLUDES */
#include "i2c.h"
#include "pwm.h"

extern "C" {void app_main(void) {
  i2c_master_init();
  i2c_write(SYSTEM__INTERRUPT_CONFIG_GPIO); 
  example_ledc_init();

  while(1) {
    uint8_t distance = i2c_distance(); 
    setPWM(distance);
    printf("distance: %d\n", distance);
    vTaskDelay(100 / portTICK_PERIOD_MS); 
*/
