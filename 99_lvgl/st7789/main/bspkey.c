#include "bspkey.h"

/******************************************************************************
 * 函数描述: ESP32的GPIO0初始化为输入
 *******************************************************************************/
void key_general_init(void)
{
    gpio_config_t pin_cfg = {};
    pin_cfg.intr_type = GPIO_INTR_DISABLE;
    pin_cfg.mode = GPIO_MODE_INPUT;
    pin_cfg.pull_down_en = GPIO_PULLDOWN_ENABLE;
    pin_cfg.pull_up_en = GPIO_PULLUP_ENABLE;
    pin_cfg.pin_bit_mask = (1ULL << key1Pin) | (1ULL << key2Pin) | (1ULL << key3Pin) | (1ULL << key4Pin) | (1ULL << keyboot);

    gpio_config(&pin_cfg);
}

/******************************************************************************
 * 函数描述: 获取按键值 只支持同时检测一个按键
 * 参   数: 按键引脚
 * 返   回: 0(无按键)/1、2、3、4--对应按键按下
 *******************************************************************************/
static uint8_t key[KEY_NUM] = {key1Pin, key2Pin, key3Pin, key4Pin, keyboot};
int get_key_value(void)
{
    int8_t value = 0;
    static uint8_t flag = 0;
    for (uint8_t i = 0; i < KEY_NUM; i++)
    {
        if (gpio_get_level(key[i]) == KEY_TRIGGER_STATUS)
        {
            value = i + 1;
            break;
        }
    }
    vTaskDelay(20 / portTICK);
    if (flag == 0 && gpio_get_level(key[value - 1]) == KEY_TRIGGER_STATUS)
    {
        flag = 1;
        return value;
    }

    if (value == 0)
    {
        flag = 0;
    }

    return 0;
}