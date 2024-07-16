#include "common.h"
#include "bspled.h"
#include "bspkey.h"
#include "p_exti.h"
#include "p_adc.h"
#include "p_uart.h"
#include "p_gpttim.h"
#include "p_i2c_sht20.h"
#include "p_wifi_ap.h"
#include "spiffs.h"s

TaskHandle_t wifi_handle;

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_AP_START)
    {
        ESP_LOGI("TAG", "AP started");
    }
    else if (event_id == WIFI_EVENT_AP_STOP)
    {
        ESP_LOGI("TAG", "AP stopped");
    }
}

void task_wifi(void *arg)
{
    while (1)
    {
        led_blink();
        vTaskDelay(1000 / portTICK);
    }
}

void app_main()
{
    led_general_init();
    spiffs_mount();
    // wifi_ap_init(&wp, wifi_event_handler);
    xTaskCreate(task_wifi, "task_wifi", 1024 * 4, NULL, 5, &wifi_handle);
}
