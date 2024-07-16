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
#include "http_client.h"
TaskHandle_t wifi_handle = NULL;

void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static int s_retry_num = 0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

        ESP_LOGI("WIFI", "STA-IP: " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI("WIFI", "STA-GATEWAY: " IPSTR, IP2STR(&event->ip_info.gw));
        ESP_LOGI("WIFI", "STA-SUBNET: " IPSTR, IP2STR(&event->ip_info.netmask));

        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < 10)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI("WIFI", "连接失败,开始重新连接...");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGE("WIFI", "连接失败!!!");
    }
}

void task_http(void *arg)
{
    int flag = 0;
    while (1)
    {
        int value = get_key_value(0);
        if (value)
        {
            if (flag == 0)
            {
                http_get_request("http://192.168.31.125:8000");
                flag = 1;
            }
            else if (flag == 1)
            {
                http_post_request("http://192.168.31.125:8000/post");
                flag = 0;
            }
        }
        vTaskDelay(50 / portTICK);
    }
}

void task_led(void *arg)
{
    while (1)
    {
        vTaskDelay(1000 / portTICK);
        led_blink();
    }
}
void app_main()
{
    led_init();
    key_general_init();
    spiffs_mount();
    wifi_sta_init(&wp, wifi_event_handler);

    xTaskCreate(task_led, "task_led", 1024 * 4, NULL, 5, &wifi_handle);
    xTaskCreate(task_http, "task_http", 1024 * 8, NULL, 10, &wifi_handle);
}
