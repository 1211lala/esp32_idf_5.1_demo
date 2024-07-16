/*
 * LEDC Chan to Group/Channel/Timer Mapping
 ** ledc: 0  => Group: 0, Channel: 0, Timer: 0
 ** ledc: 1  => Group: 0, Channel: 1, Timer: 0
 ** ledc: 2  => Group: 0, Channel: 2, Timer: 1
 ** ledc: 3  => Group: 0, Channel: 3, Timer: 1
 ** ledc: 4  => Group: 0, Channel: 4, Timer: 2
 ** ledc: 5  => Group: 0, Channel: 5, Timer: 2
 ** ledc: 6  => Group: 0, Channel: 6, Timer: 3
 ** ledc: 7  => Group: 0, Channel: 7, Timer: 3
 
 ** ledc: 8  => Group: 1, Channel: 0, Timer: 0
 ** ledc: 9  => Group: 1, Channel: 1, Timer: 0
 ** ledc: 10 => Group: 1, Channel: 2, Timer: 1
 ** ledc: 11 => Group: 1, Channel: 3, Timer: 1
 ** ledc: 12 => Group: 1, Channel: 4, Timer: 2
 ** ledc: 13 => Group: 1, Channel: 5, Timer: 2
 ** ledc: 14 => Group: 1, Channel: 6, Timer: 3
 ** ledc: 15 => Group: 1, Channel: 7, Timer: 3
 */

/*  PWM的通道，共16个(0-15)，分为高低速两组，高速通道(0-7): 80MHz时钟，
 *                                      低速通道(8-15): 1MHz时钟0-15都可以设置。
 *  初始化ledc定时器(TIM0 --TIM3) ==> 关联PIN  ==> 输出
 *
 */

#include "p_ledc.h"

void ledc_led_init(void)
{
    ledc_timer_config_t ledc_timer = {};
    ledc_timer.speed_mode = LEDC_MODE;
    ledc_timer.duty_resolution = LEDC_TIMER_10_BIT;
    ledc_timer.timer_num = LEDC_TIMER;
    ledc_timer.freq_hz = 5000;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel =
        {
            .gpio_num = LEDC_GPIO,
            .channel = LEDC_CHANNEL,
            .speed_mode = LEDC_MODE,
            .duty = 0,
            .hpoint = 0,
            .timer_sel = LEDC_TIMER};
    ledc_channel_config(&ledc_channel);
}