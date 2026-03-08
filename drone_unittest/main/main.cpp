/********************************************************************************************
 * Project: MPU6050 ESP32 Sensor Interface
 * Author: Muhammad Idrees
 * 
 * Description:
 * This file is part of a comprehensive library for interfacing the MPU6050 sensor with the
 * ESP32 using the ESP-IDF framework. The main.c file is responsible for initializing the 
 * sensor, reading data, and processing it to calculate roll, pitch, and yaw angles.
 * 
 * Author's Background:
 * Name: Muhammad Idrees
 * Degree: Bachelor's in Electrical and Electronics Engineering
 * Institution: Institute of Space Technology, Islamabad
 * 
 * License:
 * This code is provided for educational purposes and is written entirely by the author.
 * You are free to use, modify, and distribute this code, but please retain this header
 * information and provide appropriate credit to the original author.
 * 
 * Key Features:
 * - Initialization of ESP32 I2C interface and MPU6050 sensor.
 * - Calibration routines for bias correction.
 * - Reading and converting accelerometer and gyroscope data.
 * - Calculating roll, pitch, and yaw using both complementary filters and quaternions.
 * 
 * Date: [28/7/2024]
 ********************************************************************************************/

extern "C" {
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "hc_sr04.h"
#include "mpu6050.h"
#include "roll_pitch.h"
#include "quaternions.h"

//USER INCLUDES
#include "pwm.h"
#include "adc.h"
#include "i2c.h"

void app_main(void) {

    esp_err_t ret;
    int16_t accel_x, accel_y, accel_z;
    int16_t gyro_x, gyro_y, gyro_z;
    float accel_x_g, accel_y_g, accel_z_g;
    float gyro_x_dps, gyro_y_dps, gyro_z_dps;
    float accel_bias[3] = {0.00f, 0.00f, 0.00f};
    float gyro_bias[3] = {0.00f, 0.00f, 0.00f};

    // Initialize I2C
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = { .clk_speed = I2C_MASTER_FREQ_HZ }
    };
    ret = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (ret != ESP_OK) {
        ESP_LOGE("I2C", "I2C param config failed");
        return;
    }
    ret = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, ESP_INTR_FLAG_DEFAULT);
    if (ret != ESP_OK) {
        ESP_LOGE("I2C", "I2C driver install failed");
        return;
    }

    // Initialize MPU6050
    ret = mpu6050_init(I2C_MASTER_NUM);
    if (ret != ESP_OK) {
        ESP_LOGE("MPU6050", "Initialization failed");
        return;
    }

    // Calibrate the MPU6050
    mpu6050_calibrate(I2C_MASTER_NUM, accel_bias, gyro_bias);
    ESP_LOGI("MPU6050", "Calibration complete");

    // Initialize roll and pitch calculations
    roll_pitch_init();

    // Initialize Quaternion
    Quaternion q;
    quaternion_init(&q);

    while (1) {
        // Read raw data
        ret = mpu6050_read_raw_data(I2C_MASTER_NUM, &accel_x, &accel_y, &accel_z, &gyro_x, &gyro_y, &gyro_z);
        if (ret != ESP_OK) {
            ESP_LOGE("MPU6050", "Read failed");
            return;
        }

        // Convert raw data to physical values
        mpu6050_convert_accel(accel_x, accel_y, accel_z, &accel_x_g, &accel_y_g, &accel_z_g);
        mpu6050_convert_gyro(gyro_x, gyro_y, gyro_z, &gyro_x_dps, &gyro_y_dps, &gyro_z_dps);

        // Update roll and pitch
        roll_pitch_update(accel_x_g, accel_y_g, accel_z_g, gyro_x_dps, gyro_y_dps, gyro_z_dps);

        // Update Quaternion
        quaternion_update(&q, gyro_x_dps, gyro_y_dps, gyro_z_dps, accel_x_g, accel_y_g, accel_z_g, 0.01f); // 10 ms time step

        // Print results
        printf("Accel: X=%0.2f m/s^2 Accel: Y=%0.2f m/s^2 Accel: Z=%0.2f m/s^2\n", accel_x_g, accel_y_g, accel_z_g);

        printf("Gyro: X=%0.2f deg/s, Y=%0.2f deg/s, Z=%0.2f deg/s\n", gyro_x_dps, gyro_y_dps, gyro_z_dps);
        printf("Roll: %0.2f degrees\n", roll_get());
        printf("Pitch: %0.2f degrees\n", pitch_get());

        // Print Quaternion-based Roll, Pitch, Yaw
        printf("Quaternion Roll: %0.2f degrees\n", quaternion_get_roll(&q));
        printf("Quaternion Pitch: %0.2f degrees\n", quaternion_get_pitch(&q));
        printf("Quaternion Yaw: %0.2f degrees\n\n", quaternion_get_yaw(&q));

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

}}


/*
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "hc_sr04.h"
#include "mpu6050.h"

//USER INCLUDES
#include "pwm.h"
#include "adc.h"
#include "i2c.h"

extern "C" {void app_main(void) {
  while (1) 
  {
    printf("hello world\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}}

extern "C" {void app_main(void) {
  //TODO add mpu read tester

  adc_init();
  
  hc_sr04_config_t config_hc = {
    .trigger_pin = GPIO_NUM_4,
    .echo_pin = GPIO_NUM_5,
    .timeout_us = 30000
  };

  hc_sr04_handle_t sensor = hc_sr04_init(&config_hc);

  pwm_init();

  setPWM(0);
  vTaskDelay(5000 / portTICK_PERIOD_MS); 

  uint8_t motor_pwm = 75;
  float actual_distance = 0;
  int adc;

  while (1) {
    motor_pwm++;
    if (motor_pwm > 140)
      motor_pwm = 75;

    adc = read_adc();
    actual_distance = (float)hc_sr04_measure_cm(sensor);
    setPWM(motor_pwm);

    printf("adc: %d   actual_distance: %f   pwm: %d\n", adc, actual_distance, motor_pwm);

    vTaskDelay(100 / portTICK_PERIOD_MS); 
  }
}}
*/


