/**
 * 官方wiki     https://docs.espressif.com/projects/esp-idf/zh_CN/release-v5.1/esp32/api-reference/peripherals/gpio.html
 * 更改工程名,主文件名      https://blog.csdn.net/mark_md/article/details/113884641
 */

#include "common.h"
#include "bspled.h"
#include "bspkey.h"
#include "p_exti.h"
#include "p_adc.h"


TaskHandle_t task_adc_handle = NULL;
TaskHandle_t task_led_handle = NULL;

void task_adc(void *arg)
{
    onehost_adc_unit1_init();
    while (1)
    {
        get_adc1_value();
        vTaskDelay(400 / portTICK);
    }
}

void task_led(void *arg)
{
    led_general_init();
    while (1)
    {
        led_blink();
        vTaskDelay(1000 / portTICK);
    }
}

void app_main(void)
{
    xTaskCreatePinnedToCore(task_adc, "task_adc", 1024 * 4, NULL, 2, &task_adc_handle, 1);
    xTaskCreatePinnedToCore(task_led, "task_led", 1024 * 4, NULL, 2, &task_led_handle, 1);
    vTaskDelete(NULL);
}
