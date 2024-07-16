#include "bspled.h"

static const char *TAG = "BSPLED";

/******************************************************************************
 * 函数描述: ESP32的GPIO2初始化为输出的方式一
 *******************************************************************************/
void led_init(void)
{
    ESP_LOGI(TAG, "%d被设置为LED引脚", ledPin);
    gpio_reset_pin(ledPin);
    gpio_set_direction(ledPin, GPIO_MODE_OUTPUT);
}

/******************************************************************************
 * 函数描述: ESP32的GPIO2初始化为输出的方式二
 *******************************************************************************/
void led_general_init(void)
{
    gpio_config_t pin_cfg = {};

    pin_cfg.intr_type = GPIO_INTR_DISABLE;
    pin_cfg.mode = GPIO_MODE_OUTPUT;
    pin_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pin_cfg.pull_up_en = GPIO_PULLUP_ENABLE;
    pin_cfg.pin_bit_mask = (1ULL << ledPin);

    gpio_config(&pin_cfg);
}

/******************************************************************************
 * 函数描述: 板载的LED引脚blink,并打印状态,高电平点亮
 *******************************************************************************/
void led_blink(void)
{
    static uint8_t led_state = 0;

    led_state = !led_state;
    gpio_set_level(ledPin, led_state);
}
