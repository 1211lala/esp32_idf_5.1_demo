#include <stdio.h>
#include "driver/gpio.h"
#include "driver/ledc.h"

#define BRIGHTNESS_PIN 21

#define ENABLE_LEVEL 1
// LEDC_LOW_SPEED_MODE
// LEDC_HIGH_SPEED_MODE
// LEDC_SPEED_MODE_MAX
#define LEDC_SPEED_MODE LEDC_LOW_SPEED_MODE
// 0~7
#define BRIGHTNESS_LEDC_CHANNEL LEDC_CHANNEL_0

void display_brightness_init(void)
{
    // Setup LEDC peripheral for PWM backlight control
    const ledc_channel_config_t LCD_backlight_channel = {
        .gpio_num = BRIGHTNESS_PIN,
        .speed_mode = LEDC_SPEED_MODE,
        .channel = BRIGHTNESS_LEDC_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = 1,
        .duty = 0,
        .hpoint = 0};
    const ledc_timer_config_t LCD_backlight_timer = {
        .speed_mode = LEDC_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = 1,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK};

    ledc_timer_config(&LCD_backlight_timer);
    ledc_channel_config(&LCD_backlight_channel);
}

void display_brightness_set(int brightness_percent)
{
    if (brightness_percent > 100)
    {
        brightness_percent = 100;
    }
    if (brightness_percent < 0)
    {
        brightness_percent = 0;
    }

#if ENABLE_LEVEL == 0
    uint32_t duty_cycle = (1023 * (100 - brightness_percent)) / 100; // LEDC resolution set to 10bits, thus: 100% = 1023
#elif ENABLE_LEVEL == 1
    uint32_t duty_cycle = (1023 * brightness_percent) / 100;
#endif
    ledc_set_duty(LEDC_SPEED_MODE, BRIGHTNESS_LEDC_CHANNEL, duty_cycle);
    ledc_update_duty(LEDC_SPEED_MODE, BRIGHTNESS_LEDC_CHANNEL);

}

void display_backlight_off(void)
{
    return display_brightness_set(0);
}

void display_backlight_on(void)
{
    return display_brightness_set(100);
}