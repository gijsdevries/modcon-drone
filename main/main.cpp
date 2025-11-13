#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

gpio_num_t LED_BLINK = (gpio_num_t)2;

extern "C" {void app_main(void)
{
    // Configure the GPIO pin 
    gpio_reset_pin(LED_BLINK); 
    gpio_set_direction(LED_BLINK, GPIO_MODE_OUTPUT);

    // Blink loop
    while (1) {
        // Turn LED ON
        printf("LED ON\n");
        gpio_set_level(LED_BLINK, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay 1 second

        // Turn LED OFF
        printf("LED OFF\n");
        gpio_set_level(LED_BLINK, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay 1 second
    }
}}
