#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#define I2C_MASTER_SCL_IO           (gpio_num_t)22              /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           (gpio_num_t)21              /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              I2C_NUM_0                   /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ          400000                      /*!< I2C master clock frequency */
#define I2C_MASTER_TIMEOUT_MS       10000
#define MPU9250_SENSOR_ADDR         0x29                       /*!< Address of sensor */

gpio_num_t LED_BLINK = (gpio_num_t)2;

i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t dev_handle;

uint8_t i2c_distance();
uint8_t i2c_read(uint8_t msg_type);
static void i2c_write(uint8_t msg_type);
static void i2c_master_init(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle);

enum i2c_msg_types {
  /* writing */
  SYSRANGE__START,
  SYSTEM__INTERRUPT_CLEAR,

  /* reading */
  RESULT__INTERRUPT_STATUS_GPIO,
  RESULT__RANGE_VAL,
};

extern "C" {void app_main(void) {
  i2c_master_init(&bus_handle, &dev_handle);

  while(1) {
    uint8_t distance = i2c_distance();
    printf("distance: %d\n", distance);
    vTaskDelay(100 / portTICK_PERIOD_MS); 
  }
}}

uint8_t i2c_distance() {
  i2c_write(SYSRANGE__START);

  uint8_t ITR_STATUS = i2c_read(RESULT__INTERRUPT_STATUS_GPIO);

  while (ITR_STATUS != 4) {
    ITR_STATUS = i2c_read(RESULT__INTERRUPT_STATUS_GPIO);
  }

  uint8_t distance = i2c_read(RESULT__RANGE_VAL);
  i2c_write(SYSTEM__INTERRUPT_CLEAR);
  return distance;
}

static void i2c_master_init(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle)
{
  i2c_master_bus_config_t bus_config = {
    .i2c_port = I2C_MASTER_NUM,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
  };
  ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, bus_handle));

  i2c_device_config_t dev_config = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = MPU9250_SENSOR_ADDR,
    .scl_speed_hz = I2C_MASTER_FREQ_HZ,
  };
  ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handle, &dev_config, dev_handle));
}

uint8_t i2c_read(uint8_t msg_type) {
  uint8_t i2c_recieved = 0;
  switch (msg_type) {
    case RESULT__INTERRUPT_STATUS_GPIO: 
      {
        uint8_t msg[2] = {0x00, 0x4F};
        ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, msg, 2, &i2c_recieved, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
        break;
      }
    case RESULT__RANGE_VAL: 
      {
        uint8_t msg[2] = {0x00, 0x62};
        ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, msg, 2, &i2c_recieved, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
        break;
      }
    default: 
      printf("ERR i2c_read(): invalid argument recieved\n");
  }

  return i2c_recieved;
}

static void i2c_write(uint8_t msg_type) {
  switch (msg_type) {
    case SYSRANGE__START: 
      {
        uint8_t msg[3] = {0x00, 0x18, 0x01};
        ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, msg, 3, 1000));
        break;
      }
    case SYSTEM__INTERRUPT_CLEAR: 
      {
        uint8_t msg[3] = {0x00, 0x15, 0x07};
        ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, msg, 3, 1000));
        break;

      }
    default:
      printf("ERR i2c_write(): invalid argument recieved\n");
  }
}
