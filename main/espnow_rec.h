#ifndef ESPNOW_REC
#define ESPNOW_REC

#include "esp_wifi.h"
#include "nvs_flash.h"
#include <stdio.h>
#include <esp_now.h>
#include <string.h>

#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF   WIFI_IF_STA

typedef struct struct_message {
  int distance;
} struct_message;

extern uint8_t desired_distance;

void esp_now_full_init();

#endif
