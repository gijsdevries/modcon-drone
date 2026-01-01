#ifndef ESPNOW_REC
#define ESPNOW_REC

#include "esp_wifi.h"
#include "nvs_flash.h"
#include <stdio.h>
#include <esp_now.h>
#include <string.h>

#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF   WIFI_IF_STA

enum MSG_TYPE {
  DISTANCE,
  OPERATION,
  PID_DRONE,
};

typedef struct distance_struct {
  uint8_t msg_type;
  float distance;
} distance_struct;

typedef struct operation_struct {
  uint8_t msg_type;
  bool operation_state;
} operation_struct;

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


extern float desired_distance;
extern bool operation_state;
extern uint8_t broadcastAddress[6];

void esp_now_full_init();

#endif
