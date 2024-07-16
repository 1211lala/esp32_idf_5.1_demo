#include "p_exti.h"

/******************************************************************************
 * 函数描述: 将pin0设置为中断
 * 参    数: 中断的回调函数,函数类型  void (*gpio_isr_t)(void *arg);
 *******************************************************************************/
void gpio_0_isr_init(gpio_isr_t isr_handler, void *arg)
{
    gpio_config_t pin_cfg = {};

    pin_cfg.intr_type = GPIO_INTR_ANYEDGE;
    pin_cfg.mode = GPIO_MODE_INPUT;
    pin_cfg.pin_bit_mask = 1ULL << 0;
    pin_cfg.pull_down_en = 0;
    pin_cfg.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&pin_cfg);

    gpio_isr_handler_add(0, isr_handler, arg);
}