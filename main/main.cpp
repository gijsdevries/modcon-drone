/* USER INCLUDES */
#include "i2c.h"
#include "pwm.h"

extern "C" {void app_main(void) {
  i2c_master_init();
  example_ledc_init();

  while(1) {

    uint8_t distance = i2c_distance(); 
    setPWM(distance);
    printf("distance: %d\n", distance);
    vTaskDelay(100 / portTICK_PERIOD_MS); 
  }
}}
