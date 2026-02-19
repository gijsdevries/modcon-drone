#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "hc_sr04.h"

/* USER INCLUDES */
#include "i2c.h"
#include "espnow_rec.h"
#include "pwm.h"
#include "adc.h"
#include "uart.h"

#define dT 0.01
#define DEBUG_PRINT_INTERVAL 250
#define MIN_RANGE 0.05
#define MAX_RANGE 2

#define MAX_PWM 140
#define MIN_PWM 80

#define BUILTIN_LED (gpio_num_t)2
#define DEBUG

//TODO read out adc and send with espnow
//TODO read out mpu and send with espnow
//TODO add csv logging file for data

float error, error_sum, error_div, error_prev, desired_distance, actual_distance, pwm, output;
uint8_t operation_state;
bool led_state;
float kp, ki, kd;

extern "C" {void app_main(void) {
  desired_distance = 0.3;
  operation_state = IDLE;

  led_state = false;

  kp = 0.10;
  ki = 0.10;
  kd = 0.10;

  //used for sending pid data to controller
  pid_struct pid_struct;
  pid_struct.msg_type = PID_DRONE;

  //init
  esp_now_full_init();

  hc_sr04_config_t config = {
    .trigger_pin = GPIO_NUM_4,
    .echo_pin = GPIO_NUM_5,
    .timeout_us = 30000
  };
  hc_sr04_handle_t sensor = hc_sr04_init(&config);

  pwm_init();

  gpio_reset_pin(BUILTIN_LED);
  gpio_set_direction(BUILTIN_LED, GPIO_MODE_OUTPUT);

  //init the motor
  setPWM(0);
  vTaskDelay(3000 / portTICK_PERIOD_MS);

  while (1) {
    switch (operation_state) {

      case IDLE: //IDLE
	//LED blinking fast
	setPWM(0);
	gpio_set_level((gpio_num_t)2, led_state);
	led_state = !led_state; 
	vTaskDelay(100 / portTICK_PERIOD_MS);
	break;

      case PWM_CONTROL: //PWM RECIEVER
	//LED ON 
	gpio_set_level((gpio_num_t)2, 1);
	setPWM(desired_distance);
	vTaskDelay((10) / portTICK_PERIOD_MS);
	break;

      case PID_CONTROL: //PID
	//LED OFF 
	gpio_set_level((gpio_num_t)2, 0);
	actual_distance = hc_sr04_measure_cm(sensor);

	if (actual_distance < 0) {
	  actual_distance = -1;
	}
	else {
	  actual_distance /= 100; //cm to meter

	  error = desired_distance - actual_distance;
	  error_sum += error * dT;
	  error_div = (error - error_prev) / dT;
	  output = error * kp + error_sum * ki + error_div * kd;
	  error_prev = error;

	  pwm = output;

	  if (pwm > MAX_PWM)
	    pwm = MAX_PWM;
	  else if (pwm < MIN_PWM)
	    pwm = MIN_PWM;

	  setPWM(pwm);
	}
#ifdef DEBUG
	static int i = 0;
	i++;
	if (i > DEBUG_PRINT_INTERVAL) {
	  pid_struct.error = error;
	  pid_struct.error_sum = error_sum;
	  pid_struct.error_div = error_div;
	  pid_struct.error_prev = error_prev;
	  pid_struct.desired_distance = desired_distance;
	  pid_struct.actual_distance = actual_distance;
	  pid_struct.pwm = pwm;
	  pid_struct.output = output;

	  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &pid_struct, sizeof(pid_struct));

	  if (result == ESP_OK) {
	    printf("PID debug info send succes");
	  }
	  else {
	    printf("PID debug info send fail");
	  }
	  i = 0;
	} 
	vTaskDelay(dT*1000 / portTICK_PERIOD_MS);
#endif	
	break;
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}}
