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
  i2c_master_init();

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK( nvs_flash_erase() );
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK( ret );

  example_wifi_init();
  ESP_ERROR_CHECK( esp_now_init() );

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  while (1) {
    printf("desired distance: %d\n", desired_distance);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}}
