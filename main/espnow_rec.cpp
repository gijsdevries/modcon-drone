#include "espnow_rec.h"

esp_now_peer_info_t peerInfo;

uint8_t broadcastAddress[] = {0x88, 0x57, 0x21, 0x7a, 0xb1, 0x48};

//random esp at home
//uint8_t broadcastAddress[] = {0x80, 0xf3, 0xda, 0x54, 0x18, 0x38};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  uint8_t op_state_test = 0;
  uint8_t msg_type = incomingData[0];

  switch (msg_type) {
    case DISTANCE:
      static distance_struct recDistance;
      memcpy(&recDistance, incomingData, sizeof(recDistance));
      desired_distance = recDistance.distance;
#ifdef ESP_NOW_DEBUG
      printf("recieved desired_distance: %f\n", desired_distance);
#endif
      break;

    case OPERATION:
      operation_state = incomingData[1];
#ifdef ESP_NOW_DEBUG
      printf("recieved operation_state: %d\n", operation_state);
#endif
      break; 
    case PID_FACTOR:
      static pid_factor recPID;
      memcpy(&recPID, incomingData, sizeof(recPID));
      kp = recPID.kp;
      ki = recPID.ki;
      kd = recPID.kd;
#ifdef ESP_NOW_DEBUG
      printf("recieved PID values:\nkp = %f\nki = %f\nkd = %f\n", kp, ki, kd);
#endif
      break;

    default:
#ifdef ESP_NOW_DEBUG
      printf("recieved unknown val: %02X", msg_type);
#endif
      break;
  }
}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  printf(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success\n" : "Delivery Fail\n");
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
}
