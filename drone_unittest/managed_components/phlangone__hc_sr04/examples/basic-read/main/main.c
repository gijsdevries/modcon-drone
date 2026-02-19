/**
 * @file main.c
 * @brief Basic HC-SR04 Example - Simple blocking measurements
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "hc_sr04.h"

static const char* TAG = "BASIC_EXAMPLE";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting HC-SR04 Basic Example");
    
    // Sensor configuration
    hc_sr04_config_t config = {
        .trigger_pin = GPIO_NUM_4,
        .echo_pin = GPIO_NUM_5,
        .timeout_us = 30000
    };
    
    // Initialize sensor
    hc_sr04_handle_t sensor = hc_sr04_init(&config);
    if (sensor == NULL) {
        ESP_LOGE(TAG, "Failed to initialize sensor");
        return;
    }
    
    ESP_LOGI(TAG, "Sensor initialized successfully");
    
    // Main measurement loop
    while (1) 
    {
        float distance = hc_sr04_measure_cm(sensor);
        
        if (distance > 0) {
            ESP_LOGI(TAG, "Distance: %.2f cm", distance);
        } else {
            ESP_LOGE(TAG, "Measurement failed: %.2f", distance);
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
    // Cleanup (never reached in this example)
    hc_sr04_deinit(sensor);
}