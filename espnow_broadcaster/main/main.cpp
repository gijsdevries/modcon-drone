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

//TODO add csv file for logging

/// ------------------------------ ESPNOW ------------------------------ /// 

//drone esp
uint8_t broadcastAddress[] = {0x80, 0xF3, 0xDA, 0x55, 0x9B, 0x00};

//random esp at home
//uint8_t broadcastAddress[] = {0x88, 0x57, 0x21, 0x78, 0xaa, 0x80};

enum MSG_TYPE {
  DISTANCE,
  OPERATION,
  PID_DRONE,
  PID_FACTOR,
};

enum OPERATION_STATE {
  IDLE,
  PWM_CONTROL,
  PID_CONTROL,
};

typedef struct operation_struct {
  uint8_t msg_type;
  uint8_t operation_state;
} operation_struct;

typedef struct struct_message {
  uint8_t msg_type;
  float distance;
} struct_message;

float distance;

typedef struct pid_struct {
  uint8_t msg_type;
  float error; 
  float error_sum;
  float error_div;
  float error_prev;
  float desired_distance;
  float actual_distance;
  float pwm;
  float output;
} pid_struct;

typedef struct pid_factor {
  uint8_t msg_type;
  float kp;
  float ki;
  float kd;
} pid_factor;

struct_message myData;
operation_struct myOpState;
esp_now_peer_info_t peerInfo;
pid_factor my_pid_factor;
  
static uint8_t operation_state;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  printf(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success\n" : "Delivery Fail\n");
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  static esp_now_peer_info_t peerInfo;

  uint8_t msg_type = incomingData[0];

  switch (msg_type) {
    case DISTANCE:
      break;

    case OPERATION:
      break;

    case PID_DRONE:
      static pid_struct rec_pid;
      memcpy(&rec_pid, incomingData, sizeof(rec_pid));

      printf("--- Measurements ---\nerror: %f\nerror_sum: %f\nerror_div: %f\nerror_prev: %f\ndesired_distance: %f\nactual_distance: %f\npwm: %f\noutput: %f\n\n\n", 
          rec_pid.error, rec_pid.error_sum, rec_pid.error_div, rec_pid.error_prev, rec_pid.desired_distance, rec_pid.actual_distance, rec_pid.pwm, rec_pid.output);
      break;

    default:
      break;
  }
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

static void rx_task(void *arg) {
  uint16_t char_distance;

  char* data = (char*) malloc(RX_BUF_SIZE + 1);
  while (1) {
    const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
    if (rxBytes > 0) {

      if (data[0] == 'w') {
        my_pid_factor.kp += 0.05; 
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &my_pid_factor, sizeof(my_pid_factor));
        printf("updated pid factors kp=%f, ki=%f, kd=%f  ", my_pid_factor.kp,my_pid_factor.ki,my_pid_factor.kd);
      }
      else if (data[0] == 's') {
        my_pid_factor.kp -= 0.05; 
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &my_pid_factor, sizeof(my_pid_factor));
        printf("updated pid factors kp=%f, ki=%f, kd=%f  ", my_pid_factor.kp,my_pid_factor.ki,my_pid_factor.kd);
      }
      else if (data[0] == 'e') {
        my_pid_factor.ki += 0.05; 
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &my_pid_factor, sizeof(my_pid_factor));
        printf("updated pid factors kp=%f, ki=%f, kd=%f  ", my_pid_factor.kp,my_pid_factor.ki,my_pid_factor.kd);
      }
      else if (data[0] == 'd') {
        my_pid_factor.ki -= 0.05; 
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &my_pid_factor, sizeof(my_pid_factor));
        printf("updated pid factors kp=%f, ki=%f, kd=%f  ", my_pid_factor.kp,my_pid_factor.ki,my_pid_factor.kd);
      }
      else if (data[0] == 'r') {
        my_pid_factor.kd += 0.05; 
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &my_pid_factor, sizeof(my_pid_factor));
        printf("updated pid factors kp=%f, ki=%f, kd=%f  ", my_pid_factor.kp,my_pid_factor.ki,my_pid_factor.kd);
      }
      else if (data[0] == 'f') {
        my_pid_factor.kd -= 0.05; 
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &my_pid_factor, sizeof(my_pid_factor));
        printf("updated pid factors kp=%f, ki=%f, kd=%f  ", my_pid_factor.kp,my_pid_factor.ki,my_pid_factor.kd);
      }
      else if (data[0] == 'z') {
	myOpState.operation_state = IDLE;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myOpState, sizeof(myOpState));
	printf("operation state now IDLE\n");
      }
      else if (data[0] == 'x') {
        myOpState.operation_state = PWM_CONTROL;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myOpState, sizeof(myOpState));
	printf("operation state now PWM_CONTROL\n");
      }
      else if (data[0] == 'c') {
        myOpState.operation_state = PID_CONTROL;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myOpState, sizeof(myOpState));
	printf("operation state now PID_CONTROL\n");
      }
      else {
        char_distance = atoi(data);

        distance = (float)char_distance;
        snprintf(data, rxBytes, "%.2f", distance);

        myData.distance = distance;

        // Send message via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
        if (result == ESP_OK) {
          printf("Distance send succes: %f    ", myData.distance);
        }
        else {
          printf("Unknown error. Distance was valid: %f\n", myData.distance);
        }

        data[rxBytes] = 0;
      }
    }
  }
  free(data);
}

extern "C" {void app_main(void)
  {
    operation_state = IDLE;
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
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

    // Register peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;

    // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      printf("Failed to add peer\n");
      return;
    }

    myOpState.msg_type = OPERATION;
    myData.msg_type = DISTANCE;

    my_pid_factor.msg_type = PID_FACTOR;
    my_pid_factor.kp = 0.08;
    my_pid_factor.ki = 1.13;
    my_pid_factor.kd = 0.80;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &my_pid_factor, sizeof(my_pid_factor));

    while (result != ESP_OK) {
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &my_pid_factor, sizeof(my_pid_factor));
      printf("failed to send pid factors...\n");
      vTaskDelay(200 / portTICK_PERIOD_MS);
    }

    printf("pid factors send succes\n");

    uart_init();
    //xTaskCreate(button_monitor, "button_monitor", 4096, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(rx_task, "uart_rx_task", 4096, NULL, configMAX_PRIORITIES - 1, NULL);

    gpio_reset_pin((gpio_num_t)2);
    gpio_set_direction((gpio_num_t)2, GPIO_MODE_OUTPUT);

    bool led_state = false;

    while (1)
    {
      switch (myOpState.operation_state) {

	case IDLE: //IDLE
		   //LED blinking fast
	  gpio_set_level((gpio_num_t)2, led_state);
	  led_state = !led_state; 
	  vTaskDelay(100 / portTICK_PERIOD_MS);
	  break;

	case PWM_CONTROL: //PWM RECIEVER
			  //LED ON 
	  gpio_set_level((gpio_num_t)2, 1);
	  break;

	case PID_CONTROL: //PID
			  //LED OFF 
	  gpio_set_level((gpio_num_t)2, 0);
	  break;
      }
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
  }}
