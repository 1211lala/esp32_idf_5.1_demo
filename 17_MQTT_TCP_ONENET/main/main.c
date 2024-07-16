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
#include "p_mqtt.h"
#include "http_server.h"
#include "sntp.h"

void task_wifi(void *arg);
void task_led(void *arg);

static const char *TAG = "main";

TaskHandle_t wifi_handle;
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
        wp.isConnected = true;
        s_retry_num = 0;
        ESP_LOGI("TEST_ESP32", "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI("TEST_ESP32", "Got GW: " IPSTR, IP2STR(&event->ip_info.gw));
        ESP_LOGI("TEST_ESP32", "Got NT: " IPSTR, IP2STR(&event->ip_info.netmask));

        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        wp.isConnected = false;
        if (s_retry_num < 1000)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGE("TAG", "重新开始连接AP %d/%d", s_retry_num, 1000);
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            ESP_LOGE("TAG", "连接失败");
        }
    }
}

void app_main()
{
    led_general_init();
   
    fs_mount();

    wifi_sta_init(&wp, wifi_event_handler);
    http_server_start(80);
    mqtt_task_start();
    // ntp_init();
    xTaskCreate(task_led, "task_led", 1024 * 4, NULL, 3, NULL);
}

void task_led(void *arg)
{
    while (1)
    {
        // get_time();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}