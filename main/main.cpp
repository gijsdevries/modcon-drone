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
#include "espnow_rec.h"

uint8_t desired_distance;

extern "C" {void app_main(void) {
  esp_now_full_init();

  while (1) {
    printf("desired distance: %d\n", desired_distance);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}}
