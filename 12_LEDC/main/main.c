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
#include "p_ledc.h"
TaskHandle_t ledc_handle;

void task_ledc(void *arg)
{
    while (1)
    {
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 1000);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        vTaskDelay(500 / portTICK);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        vTaskDelay(500 / portTICK);
    }
}

void app_main()
{
    spiffs_mount();

    ledc_led_init();
    xTaskCreate(task_ledc, "task_ledc", 1024 * 4, NULL, 5, &ledc_handle);
}
