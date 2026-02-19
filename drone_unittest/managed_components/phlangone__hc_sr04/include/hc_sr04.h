/**
 * @file hc_sr04.h
 * @brief Simple HC-SR04 Ultrasonic Sensor Driver for ESP-IDF v5
 */

#ifndef HC_SR04_H
#define HC_SR04_H

#include <stdint.h>
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HC-SR04 sensor configuration structure
 * 
 * @var trigger_pin GPIO pin used for TRIGGER signal output
 * @var echo_pin GPIO pin used for ECHO signal input  
 * @var timeout_us Maximum time to wait for echo pulse in microseconds
 */
typedef struct {
    gpio_num_t trigger_pin;
    gpio_num_t echo_pin;
    uint32_t timeout_us;
} hc_sr04_config_t;

/**
 * @brief Opaque handle to HC-SR04 sensor instance
 */
typedef struct hc_sr04_s* hc_sr04_handle_t;

/**
 * @brief Initialize HC-SR04 ultrasonic sensor
 * 
 * @param config Pointer to sensor configuration structure
 * @return hc_sr04_handle_t Sensor handle on success, NULL on failure
 * 
 * @note This function configures GPIO pins and installs interrupt handlers
 * @warning Configuration structure must remain valid during sensor operation
 */
hc_sr04_handle_t hc_sr04_init(const hc_sr04_config_t* config);

/**
 * @brief Deinitialize HC-SR04 sensor and free resources
 * 
 * @param handle Sensor handle to deinitialize
 * 
 * @note Removes interrupt handlers and frees allocated memory
 * @warning Sensor handle becomes invalid after this call
 */
void hc_sr04_deinit(hc_sr04_handle_t handle);

/**
 * @brief Perform distance measurement in centimeters
 * 
 * @param handle Sensor handle
 * @return float Distance in centimeters, negative value on error
 * 
 * @note Measurement uses sound speed of 340 m/s (0.034 cm/μs)
 * @warning This is a blocking function with timeout
 * 
 * @retval >=0.0 Successfully measured distance in cm
 * @retval -1.0 Sensor handle is NULL
 * @retval -2.0 Measurement timeout occurred
 * @retval -3.0 Echo pulse not detected
 */
float hc_sr04_measure_cm(hc_sr04_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif // HC_SR04_H