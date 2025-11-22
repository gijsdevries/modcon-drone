#include "driver/i2c_master.h"
#include "i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
