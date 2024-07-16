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
#include "p_gpttim.h"


TaskHandle_t task_tim_handle = NULL;

void IRAM_ATTR task_tim0(void *arg)
{
    while (1)
    {
        uint64_t value = 0;
        if (xQueueReceive(tim_queue, &value, portMAX_DELAY))
        {
            /* 如果中断时1ms会触发任务看门狗，因为LOG打印所需要的时间大于1ms*/
            ESP_LOGI("LOG", "tim0 value : %llu", value);

            led_blink();
        }
    }
}

bool IRAM_ATTR gpttim_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

    uint64_t value = 4;
    xQueueSendFromISR(tim_queue, &value, &pxHigherPriorityTaskWoken);
    return pxHigherPriorityTaskWoken;
}

void app_main(void)
{
    led_init();
    tim_queue = xQueueCreate(10, sizeof(uint64_t));
    if (tim_queue == NULL)
    {
        ESP_LOGI("LOG", "队列初始化失败");
    }
    gpttimer_init(gpttim_on_alarm_cb, 1000000, 1000000);

    xTaskCreatePinnedToCore(task_tim0, "task_tim0", 1024 * 4, NULL, 10, &task_tim_handle, 1);
    vTaskDelete(NULL);
}
