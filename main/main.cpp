#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

/* USER INCLUDES */
#include "i2c.h"

extern "C" {void app_main(void) {
  i2c_master_init();

  while(1) {
    uint8_t distance = i2c_distance(); //push in main?
    printf("distance: %d\n", distance);
    vTaskDelay(100 / portTICK_PERIOD_MS); 
  }
}}


