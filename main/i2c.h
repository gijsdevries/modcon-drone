#ifndef I2C_H
#define I2C_H

#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_SCL_IO           (gpio_num_t)22              /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           (gpio_num_t)21              /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              I2C_NUM_0                   /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ          400000                      /*!< I2C master clock frequency */
#define I2C_MASTER_TIMEOUT_MS       10000
#define MPU9250_SENSOR_ADDR         0x29                       /*!< Address of sensor */

extern i2c_master_dev_handle_t dev_handle;
extern i2c_master_bus_handle_t bus_handle;

uint8_t i2c_distance();
uint8_t i2c_read(uint8_t msg_type);
static void i2c_write(uint8_t msg_type);

enum i2c_msg_types {
  /* writing */
  SYSRANGE__START,
  SYSTEM__INTERRUPT_CLEAR,

  /* reading */
  RESULT__INTERRUPT_STATUS_GPIO,
  RESULT__RANGE_VAL,
};

#endif
