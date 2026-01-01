#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "stdio.h"
#include "string.h"
#include <esp_now.h>
#include "nvs_flash.h"

#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF   WIFI_IF_STA

/// ------------------------------ ESPNOW ------------------------------ /// 

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x80, 0xF3, 0xDA, 0x55, 0x9B, 0x00};


enum MSG_TYPE {
  DISTANCE,
  OPERATION,
};

typedef struct struct_message {
  int msg_type;
  float distance;
} struct_message;

float distance;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  printf(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success\n" : "Delivery Fail\n");
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

/// ------------------------------ UART ------------------------------ /// 

static const int RX_BUF_SIZE = 1024;
#define RXD_PIN (gpio_num_t)3

void uart_init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, -1, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

static void button_monitor(void *arg) {
  while (1) {
    gpio_set_level((gpio_num_t)2, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay 1 second
    gpio_set_level((gpio_num_t)2, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay 1 second
  }
}

static void rx_task(void *arg) {
  char char_distance;
  char* data = (char*) malloc(RX_BUF_SIZE + 1);
  while (1) {
    const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
    if (rxBytes > 0) {
      char_distance = atoi(data);

      distance = (float)char_distance;
      snprintf(data, rxBytes, "%.2f", distance);

      gpio_set_level((gpio_num_t)2, 1);
      myData.distance = distance;

      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
      if (result == ESP_OK) {
        printf("Distance send succes: %f    ", myData.distance);
      }
      else {
        printf("Unknown error. Distance was valid: %f\n", myData.distance);
      }

      vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay 1 second
      gpio_set_level((gpio_num_t)2, 0);
      data[rxBytes] = 0;
    }
  }
  free(data);
}

extern "C" {void app_main(void)
  {
    uint8_t mac_addr[6]; // Buffer for the MAC address

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK( nvs_flash_erase() );
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    example_wifi_init();
    ESP_ERROR_CHECK( esp_now_init() );

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));

    // Register peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;

    // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      printf("Failed to add peer\n");
      return;
    }

    uart_init();
    xTaskCreate(button_monitor, "button_monitor", 4096, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(rx_task, "uart_rx_task", 4096, NULL, configMAX_PRIORITIES - 1, NULL);

    gpio_reset_pin((gpio_num_t)2);
    gpio_set_direction((gpio_num_t)2, GPIO_MODE_OUTPUT);

    while (1) {
      myData.msg_type = DISTANCE;
      myData.distance = 15;
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
      if (result == ESP_OK) {
        printf("Distance send succes: %f    ", myData.distance);
      }
      else {
        printf("Unknown error. Distance was valid: %f\n", myData.distance);
      }

      vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay 1 second
    }
  }}
