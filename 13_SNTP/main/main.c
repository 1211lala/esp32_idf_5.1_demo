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
#include "sntp.h"


#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"

#include "nvs_flash.h"
#include "esp_netif_sntp.h"
#include "lwip/ip_addr.h"
#include "esp_sntp.h"

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

        ESP_LOGI("TEST_ESP32", "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI("TEST_ESP32", "Got GW: " IPSTR, IP2STR(&event->ip_info.gw));
        ESP_LOGI("TEST_ESP32", "Got NT: " IPSTR, IP2STR(&event->ip_info.netmask));

        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < 10)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI("TAG", "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI("TAG", "connect to the AP fail");
    }
}

void task_wifi(void *arg)
{
    while (1)
    {
        get_time();
        vTaskDelay(1000 / portTICK);
        led_blink();
    }
}





void app_main()
{

    led_init();
    spiffs_mount();
    
    wifi_sta_init(&wp, wifi_event_handler);
    ntp_init();

    xTaskCreate(task_wifi, "task_wifi", 1024 * 4, NULL, 5, &wifi_handle);
}
