/*

https://blog.csdn.net/m0_50064262/article/details/115407884


## ESP32定时器资源
    ESP32 内置4 个64-bit 通用定时器,分为两组，每组两个
    每个定时器包含一个16-bit 预分频器和一个64-bit 可自动重新加载向上／向下计数器。

##  回调配置法(好用)
　　ESP32 硬件定时器在默认情况下有一个 isr 程序，称作 “总 ISR 程序” 。它为我们执行我们配置的定时器的回调。
    我们写的回调函数，配置后将属于这个ISR程序的一部分。因此这个回调函数又称为ISR 回调（ISR callback）
        调用函数timer_isr_callback_add()添加回调
        调用函数timer_isr_callback_remove()移除回调


## 自定义配置isr
    如果使用此函数重新注册 ISR，则需要编写完整的 ISR。例如需要清除中断标志位，配置自旋锁等。
        调用 timer_isr_register(0, 0, timerIsr, &config, ESP_INTR_FLAG_IRAM, NULL) 注册中断函数
        示例中断函数

        void IRAM_ATTR timerIsr(void *arg)
        {
            // 获取自旋锁
            timer_spinlock_take(0); 

            //代码区1
            // 清除中断标志位
            timer_group_clr_intr_status_in_isr(0, 0);
            // 重新使能alarm事件
            timer_group_enable_alarm_in_isr(0, 0);

            //代码区2
            timer_spinlock_give(0);
        }
*/

#include "p_tim.h"

QueueHandle_t tim_queue = NULL;

/******************************************************************************
 * 函数描述: 硬件定时器0的初始化，设置定时器计数频率为1M，可自动重加载，向上计数,初始哈时不开启定时器，需调用 timer_start(0, 0) 开启
 * 参  数1: 定时中断回调函数 bool (*timer_isr_t)(void *);
 * 参  数2: 定时器回调函数的参数
 * 参  数3: 定时器向上计数终值
 *******************************************************************************/
void tim0_init(timer_isr_t timer_callback, void *arg, uint32_t value)
{
    uint32_t apb_freq = rtc_clk_apb_freq_get();

    uint32_t divider = apb_freq / 1000000;

    timer_config_t config = {};
    config.alarm_en = TIMER_ALARM_EN;
    config.auto_reload = TIMER_AUTORELOAD_EN;
    config.clk_src = TIMER_SRC_CLK_DEFAULT;
    config.counter_dir = TIMER_COUNT_UP;
    config.counter_en = TIMER_PAUSE;
    config.divider = divider;
    // config.intr_type =
    timer_init(0, 0, &config);

    timer_set_counter_value(0, 0, 1);
    timer_set_alarm_value(0, 0, value);
    timer_enable_intr(0, 0);

    timer_isr_callback_add(0, 0, timer_callback, arg, ESP_INTR_FLAG_IRAM);
}
