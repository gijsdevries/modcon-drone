#include "espnow_rec.h"

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  static struct_message recDistance;
  memcpy(&recDistance, incomingData, sizeof(recDistance));
  desired_distance = recDistance.distance;
}

void example_wifi_init(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(ESPNOW_WIFI_MODE) );
    ESP_ERROR_CHECK( esp_wifi_start());
}

void esp_now_full_init() {
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK( nvs_flash_erase() );
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK( ret );

  example_wifi_init();
  ESP_ERROR_CHECK( esp_now_init() );

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}
