/**
 * @file hc_sr04.c
 * @brief Simple HC-SR04 Ultrasonic Sensor Driver Implementation
 */

#include "hc_sr04.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "HC-SR04";

typedef struct hc_sr04_s {
    gpio_num_t trigger_pin;
    gpio_num_t echo_pin;
    uint32_t timeout_us;
    volatile uint32_t pulse_start;
    volatile uint32_t pulse_end;
    volatile bool pulse_received;
} hc_sr04_t;

static void IRAM_ATTR echo_isr_handler(void* arg) {
    hc_sr04_handle_t sensor = (hc_sr04_handle_t)arg;
    uint32_t level = gpio_get_level(sensor->echo_pin);
    
    if (level == 1) {
        // Rising edge - start timing
        sensor->pulse_start = esp_timer_get_time();
    } else {
        // Falling edge - stop timing
        sensor->pulse_end = esp_timer_get_time();
        sensor->pulse_received = true;
    }
}

hc_sr04_handle_t hc_sr04_init(const hc_sr04_config_t* config) {
    if (config == NULL) return NULL;
    
    hc_sr04_handle_t sensor = malloc(sizeof(hc_sr04_t));
    if (sensor == NULL) return NULL;
    
    // Store configuration
    sensor->trigger_pin = config->trigger_pin;
    sensor->echo_pin = config->echo_pin;
    sensor->timeout_us = config->timeout_us ? config->timeout_us : 30000;
    sensor->pulse_received = false;
    
    // Setup trigger pin as output
    gpio_reset_pin(sensor->trigger_pin);
    gpio_set_direction(sensor->trigger_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(sensor->trigger_pin, 0);
    
    // Setup echo pin as input with interrupt
    gpio_reset_pin(sensor->echo_pin);
    gpio_set_direction(sensor->echo_pin, GPIO_MODE_INPUT);
    
    // Install GPIO ISR service
    gpio_install_isr_service(0);
    
    // Configure interrupt on both edges
    gpio_set_intr_type(sensor->echo_pin, GPIO_INTR_ANYEDGE);
    gpio_isr_handler_add(sensor->echo_pin, echo_isr_handler, sensor);
    
    ESP_LOGI(TAG, "HC-SR04 initialized on trigger GPIO%d, echo GPIO%d", 
             sensor->trigger_pin, sensor->echo_pin);
    
    return sensor;
}

void hc_sr04_deinit(hc_sr04_handle_t handle) {
    if (handle == NULL) return;
    
    gpio_isr_handler_remove(handle->echo_pin);
    free(handle);
}

float hc_sr04_measure_cm(hc_sr04_handle_t handle) {
    if (handle == NULL) return -1.0;
    
    handle->pulse_received = false;
    
    // Send 10us trigger pulse
    gpio_set_level(handle->trigger_pin, 1);
    esp_rom_delay_us(10);
    gpio_set_level(handle->trigger_pin, 0);
    
    // Wait for pulse with timeout
    uint32_t start_time = esp_timer_get_time();
    while (!handle->pulse_received) {
        if ((esp_timer_get_time() - start_time) > handle->timeout_us) {
            ESP_LOGE(TAG, "Measurement timeout");
            return -1.0;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    
    // Calculate distance (sound speed ~340 m/s = 0.034 cm/us)
    uint32_t pulse_duration = handle->pulse_end - handle->pulse_start;
    float distance_cm = (pulse_duration * 0.034) / 2.0;
    
    return distance_cm;
}