#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "nvs_flash.h"

#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF   ESP_IF_WIFI_STA

#define RXD_PIN (gpio_num_t)3
#define TXD_PIN (gpio_num_t)1
#define RX_BUF_SIZE 1024

void uart_init() {
  const uart_config_t uart_config = {
    .baud_rate = 115200, 
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
  };

  uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
  uart_param_config(UART_NUM_0, &uart_config);
  uart_set_pin(UART_NUM_0, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

static void example_wifi_init(void)
{
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(ESPNOW_WIFI_MODE) );
  ESP_ERROR_CHECK( esp_wifi_start());
}

extern "C" {void app_main(void) {

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK( nvs_flash_erase() );
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK( ret );

  example_wifi_init();
  esp_now_init(); 

  uart_init();

  //  uint8_t mac_adr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  //  uint8_t distance = 12;
  uint8_t buf_rx[1024];
  uint8_t buf_tx[1024] = "hoi";

  while (1) {
    ESP_ERROR_CHECK(uart_write_bytes(UART_NUM_0, buf_tx, 1024));
//    printf("buf_tx: %s\n", buf_tx);
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay 1 second

    ESP_ERROR_CHECK(uart_read_bytes(UART_NUM_0, buf_rx, 1024, portTICK_PERIOD_MS / 1000));
    printf("buf_rx: %s\n", buf_rx);
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay 1 second
  }
}}
