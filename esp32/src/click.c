#include "driver/ledc.h"
#include "pins.h"
#include "click.h"

#define CLICK_FREQ_HZ   4000
#define CLICK_DUTY       512        /* 50 % of 1024 (10-bit resolution) */
#define CLICK_TIMER      LEDC_TIMER_0
#define CLICK_CHANNEL    LEDC_CHANNEL_0


void click_init(void)
{
    ledc_timer_config_t timer = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .timer_num       = CLICK_TIMER,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz         = CLICK_FREQ_HZ,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t chan = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = CLICK_CHANNEL,
        .timer_sel  = CLICK_TIMER,
        .gpio_num   = PIN_PIEZO,
        .duty       = 0,
        .hpoint     = 0,
    };
    ledc_channel_config(&chan);
}


void click_start(void)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, CLICK_CHANNEL, CLICK_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, CLICK_CHANNEL);
}


void click_stop(void)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, CLICK_CHANNEL, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, CLICK_CHANNEL);
}
