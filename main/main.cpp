#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "hc_sr04.h"
#include "esp_timer.h"

/* USER INCLUDES */
#include "i2c.h"
#include "espnow_rec.h"
#include "pwm.h"
#include "adc.h"
#include "uart.h"

#define DEBUG_PRINT_INTERVAL 1000 //sending debug info in ms

#define MAX_PWM 180
#define MIN_PWM 80

#define PWM_SLOPE 1

#define BUILTIN_LED (gpio_num_t)2
#define DEBUG

//TODO read out mpu and send with espnow

//global variables
float error, error_sum, error_div, error_prev, desired_distance, actual_distance, actual_dis_prev, pwm, pwm_prev, output;
float kp, ki, kd;
uint8_t operation_state;

extern "C" {void app_main(void) {
  actual_dis_prev = 0;
  bool led_state = false;

  operation_state = IDLE;

  int debug_counter = 0;
  int64_t time = 0; //time since running in ms
  int64_t time_prev = 0;
  int64_t dtime = 0;

  desired_distance = 30.0;

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

  time = esp_timer_get_time() / 1000; //display in ms

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

	pwm_prev = pwm;
	pwm = desired_distance;

	if ((pwm - pwm_prev) > PWM_SLOPE)
	{
	  pwm = pwm_prev + PWM_SLOPE;
	  vTaskDelay((100) / portTICK_PERIOD_MS);
	} 

	if (pwm > MAX_PWM)
	  pwm = MAX_PWM;
	else if (pwm < MIN_PWM)
	  pwm = MIN_PWM;

	setPWM(pwm);

#ifdef DEBUG
	time = esp_timer_get_time() / 1000; //display in ms
	if (time > DEBUG_PRINT_INTERVAL * debug_counter)
	{
	  debug_counter++;
	  pid_struct.time = time;
	  pid_struct.error = error;
	  pid_struct.error_sum = error_sum;
	  pid_struct.error_div = error_div;
	  pid_struct.error_prev = error_prev;
	  pid_struct.desired_distance = desired_distance;
	  pid_struct.actual_distance = actual_distance;
	  pid_struct.pwm = pwm;
	  pid_struct.output = output;

	  esp_now_send(broadcastAddress, (uint8_t *) &pid_struct, sizeof(pid_struct));
	  printf("pwm debug info send succes ");
	}
#endif
	vTaskDelay(10 / portTICK_PERIOD_MS);
	break;

      case PID_CONTROL: //PID LED OFF
	gpio_set_level((gpio_num_t)2, 0);
	time_prev = time;
	time = esp_timer_get_time() / 1000; //display in ms
	dtime = time - time_prev;
	//dtime /= 1000.0f;

	if (dtime <= 0)
	  dtime = 0.01;


	//printf("time: %lld, time_prev: %lld, dtime: %lld\n", time, time_prev, dtime);
	actual_distance = hc_sr04_measure_cm(sensor);

	if (actual_distance < 0) {
	  actual_distance = actual_dis_prev;
	}
	else {
	  actual_dis_prev = actual_distance;
	}

	error = desired_distance - actual_distance;
	error_sum += error * dtime;
	error_div = (error - error_prev) / dtime;
	output = error * kp + (error_sum * ki) / 1000 + (error_div * kd) / 1000;
	error_prev = error;

	pwm_prev = pwm;
	pwm = output;

	/*
	if ((pwm - pwm_prev) > PWM_SLOPE)
	{
	  pwm = pwm_prev + PWM_SLOPE;
	  vTaskDelay((100) / portTICK_PERIOD_MS);
	} 
	else if ((pwm - pwm_prev) < -PWM_SLOPE)
	{
	  pwm = pwm_prev - PWM_SLOPE;
	  vTaskDelay((100) / portTICK_PERIOD_MS);
	}
	*/

	if (pwm > MAX_PWM)
	  pwm = MAX_PWM;
	else if (pwm < MIN_PWM)
	  pwm = MIN_PWM;

	setPWM(pwm);

#ifdef DEBUG
	if (time > DEBUG_PRINT_INTERVAL * debug_counter)
	{
	  debug_counter++;
	  pid_struct.time = time;
	  pid_struct.error = error;
	  pid_struct.error_sum = error_sum;
	  pid_struct.error_div = error_div;
	  pid_struct.error_prev = error_prev;
	  pid_struct.desired_distance = desired_distance;
	  pid_struct.actual_distance = actual_distance;
	  pid_struct.pwm = pwm;
	  pid_struct.output = output;

	  esp_now_send(broadcastAddress, (uint8_t *) &pid_struct, sizeof(pid_struct));
	  printf("PID debug info send succes ");
	  vTaskDelay(10 / portTICK_PERIOD_MS);

	}
#endif
	break;
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  } 
}}
