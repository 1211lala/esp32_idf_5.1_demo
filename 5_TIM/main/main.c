/**
 * 官方wiki     https://docs.espressif.com/projects/esp-idf/zh_CN/release-v5.1/esp32/api-reference/peripherals/gpio.html
 * 更改工程名,主文件名      https://blog.csdn.net/mark_md/article/details/113884641
 */

#include "common.h"
#include "bspled.h"
#include "bspkey.h"
#include "p_exti.h"
#include "p_adc.h"
#include "p_uart.h"
#include "p_tim.h"

/*

我的理解
    freertos在执行任务A时被中断打断，在中断中又通过队列启动了B任务，
    如果 pxHigherPriorityTaskWoken 为 true 则直接进入任务B，
    如果 pxHigherPriorityTaskWoken 为 false 则是先进入任务A，
    再判断是否有高于A的任务，进行任务切换


bool timer_callback(void *arg)
{
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

    uint64_t value = timer_group_get_counter_value_in_isr(0, 0);
    xQueueSendFromISR(tim_queue, &value, &pxHigherPriorityTaskWoken);
    return pxHigherPriorityTaskWoken;
}
返回true、pdTRUE等定义为 1 的宏，或直接返回1，意味着ISR程序结束时发生一次上下文切换
返回false、pdFALSE等定义为 0 的宏，或直接返回0，意味着ISR程序结束后不发生上下文切换

它的第三个参数名为pxHigherPriorityTaskWoken，传入一个BaseType_t类型的指针。
作用是如果宏定义函数xQueueSendFromISR()造成了更高优先级任务进入就绪状态，则将这个指针指向的变量赋值为pdTRUE，否则则不发生改变。
你可以选择根据这个变量决定有没有必要发生一次上下文切换。
对于此callback函数，直接返回这个值即可。如果你在任何情况下，ISR程序结束时都不想发生上下文切换，
则一直返回 false 或者FreeRTOS风格的 pdFALSE即可
*/

TaskHandle_t task_tim_handle = NULL;

bool timer_callback(void *arg)
{
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

    uint64_t value = timer_group_get_counter_value_in_isr(0, 0);
    xQueueSendFromISR(tim_queue, &value, &pxHigherPriorityTaskWoken);
    return pxHigherPriorityTaskWoken;
}

void IRAM_ATTR task_tim0(void *arg)
{
    while (1)
    {
        uint64_t value = 0;
        if (xQueueReceive(tim_queue, &value, portMAX_DELAY))
        {
            /* 如果中断时1ms会触发任务看门狗，因为LOG打印所需要的时间大于1ms*/
            // ESP_LOGI("LOG", "tim0 value : %llu", value);

            led_blink();
        }
    }
}

void app_main(void)
{
    led_general_init();
    tim0_init(timer_callback, NULL, 2000);
    tim_queue = xQueueCreate(10, sizeof(uint64_t));
    if (tim_queue == NULL)
    {
        ESP_LOGI("LOG", "队列初始化失败");
    }
    xTaskCreatePinnedToCore(task_tim0, "task_tim0", 1024 * 4, NULL, 10, &task_tim_handle, 1);
    timer_start(0, 0);
    vTaskDelete(NULL);
}
