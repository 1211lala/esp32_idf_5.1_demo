#include "common.h"
#include "bspled.h"
#include "bspkey.h"
#include "p_exti.h"
#include "p_adc.h"
#include "p_uart.h"
#include "p_gpttim.h"
#include "p_i2c_sht20.h"
#include "p_wifi_sta.h"
#include "spiffs.h"
#include "p_touch.h"

SemaphoreHandle_t xSemaphore = NULL;

static void task_touch(void *arg)
{
    static const char touchNum = 0;
    uint16_t value;
    touch_init(touchNum);
    while (1)
    {
        touch_pad_read_filtered(touchNum, &value);
        ESP_LOGI("task_touch", "touch value: %d", value);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        led_blink();
    }
}
uint32_t pad_intr;
void touch_0_callback(void *arg)
{
    static BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    /* 2的touch引脚 */
    pad_intr = touch_pad_get_status();
    if (pad_intr)
    {
        touch_pad_clear_status();
        xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
    }
}
static void task_touch_isr(void *arg)
{
    static const char touchNum = 4;
    vSemaphoreCreateBinary(xSemaphore);
    touch_init_isr(touchNum, touch_0_callback);
    while (1)
    {
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI("touch", "pad_intr = %ld", pad_intr);
            led_blink();
        }
    }
}

void app_main(void)
{
    led_init();
    // xTaskCreate(&task_touch, "task_touch", 1024 * 4, NULL, 5, NULL);
    xTaskCreate(&task_touch_isr, "task_touch_isr", 1024 * 4, NULL, 5, NULL);
}