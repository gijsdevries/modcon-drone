#include "pwm.h"

#define FIVE_PERCENT_DUTY_CYCLE (8192 * 0.05)
#define CHAR_TO_DUTY_CYCLE_FACTOR 1.59

void setPWM(float motorspeed) {
  if (motorspeed > 255)
    motorspeed = 255;
  else if (motorspeed < 0)
    motorspeed = 0;

  uint16_t duty_cycle = FIVE_PERCENT_DUTY_CYCLE + ((uint8_t)motorspeed * CHAR_TO_DUTY_CYCLE_FACTOR); 

  ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty_cycle));
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

void pwm_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK,
        .deconfigure      = 0
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = LEDC_OUTPUT_IO,
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0,
        .flags          = {
            .output_invert = 0
        }
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}


