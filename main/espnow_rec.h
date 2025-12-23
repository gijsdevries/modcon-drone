#ifndef ESPNOW_REC
#define ESPNOW_REC

#include "esp_wifi.h"
#include "nvs_flash.h"
#include <stdio.h>
#include <esp_now.h>
#include <string.h>

#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF   WIFI_IF_STA

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
void example_wifi_init(void);

#endif
