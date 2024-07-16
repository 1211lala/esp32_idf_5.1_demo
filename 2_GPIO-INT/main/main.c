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
#include "p_exti.h"

TaskHandle_t task_led_handle = NULL;

QueueHandle_t isr_Quene = NULL;

void task_led(void *arg)
{
    int gpio_pin = 0;
    led_general_init();
    while (1)
    {
        if (xQueueReceive(isr_Quene, &gpio_pin, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI("INT", "GPIO[%d]INT", gpio_pin);
            led_blink();
        }
        
    }
}

void IRAM_ATTR gpio_0_isr_callback(void *arg)
{
    // uint8_t gpio_pin = *((uint8_t*)arg); //这种写法错误,不知道为什么
    uint8_t gpio_pin = (uint8_t)arg;
    xQueueSendFromISR(isr_Quene, &gpio_pin, NULL);
}

void app_main(void)
{
    gpio_install_isr_service(0);
    gpio_0_isr_init(gpio_0_isr_callback, (void*)4);

    isr_Quene = xQueueCreate(5, sizeof(uint8_t));
    if (isr_Quene == NULL)
    {
        ESP_LOGI("Warn", "队列创造失败");
    }
    xTaskCreatePinnedToCore(task_led, "task_led", 1024 * 3, (void *)0, 2, &task_led_handle, 1);
    vTaskDelete(NULL);
}
