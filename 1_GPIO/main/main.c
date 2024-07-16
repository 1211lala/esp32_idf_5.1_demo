/**
 * 官方wiki     https://docs.espressif.com/projects/esp-idf/zh_CN/release-v5.1/esp32/api-reference/peripherals/gpio.html
 * 更改工程名,主文件名      https://blog.csdn.net/mark_md/article/details/113884641
 * 
 * 
 * 
 * 
*/

#include "common.h"
#include "bspled.h"
#include "bspkey.h"
TaskHandle_t task_led_handle = NULL;
TaskHandle_t task_key_handle = NULL;

void task_led(void *arg)
{
    led_general_init();
    while (1)
    {
        vTaskDelay(100 / portTICK);
    }
}

void task_key(void *arg)
{
    led_general_init();
    while (1)
    {
        uint8_t value = get_key_value(keyPin);

        if(value)
        {
            led_blink();
        }

        vTaskDelay(50/portTICK);
    }
}

void app_main(void)
{
    xTaskCreatePinnedToCore(task_led, "task_led", 1024 * 3, NULL, 2, &task_led_handle, 1);
    xTaskCreatePinnedToCore(task_key, "task_key", 1024 * 3, NULL, 3, &task_key_handle, 1);
    vTaskDelete(NULL);
}
