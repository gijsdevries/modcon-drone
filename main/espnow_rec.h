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
  PID_FACTOR,
};

enum OPERATION_STATE {
  IDLE,
  PWM_CONTROL,
  PID_CONTROL,
};

typedef struct distance_struct {
  uint8_t msg_type;
  float distance;
} distance_struct;

typedef struct operation_struct {
  uint8_t msg_type;
  uint8_t operation_state;
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

typedef struct pid_factor {
  uint8_t msg_type;
  float kp;
  float ki;
  float kd;
} pid_factor;

extern float desired_distance;
extern uint8_t operation_state;
extern uint8_t broadcastAddress[];

extern float kp;
extern float ki;
extern float kd;

void esp_now_full_init();

#define ESP_NOW_DEBUG

#endif
