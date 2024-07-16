#include "bspkey.h"

/******************************************************************************
 * 函数描述: ESP32的GPIO0初始化为输入
 *******************************************************************************/
void key_general_init(void)
{
    gpio_config_t pin_cfg = {};
    pin_cfg.intr_type = GPIO_INTR_DISABLE;
    pin_cfg.mode = GPIO_MODE_INPUT;
    pin_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pin_cfg.pull_up_en = GPIO_PULLUP_ENABLE;
    pin_cfg.pin_bit_mask = (1ULL << keyPin);

    gpio_config(&pin_cfg);
}

/******************************************************************************
 * 函数描述: 获取按键值
 * 参   数: 按键引脚
 * 返   回: true(按下) flase(松开)
 *******************************************************************************/
uint8_t get_key_value(uint8_t pin)
{
    static uint8_t flag = 1;

    if(gpio_get_level(pin) != false)
    {
        flag = 1;
    }
    if(flag == 1 && gpio_get_level(pin) == false)
    {
        flag = 0;
        return true;
    }
    return false;
}