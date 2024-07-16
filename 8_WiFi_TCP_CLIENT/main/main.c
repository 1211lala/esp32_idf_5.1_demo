#include "common.h"
#include "bspled.h"
#include "bspkey.h"
#include "p_exti.h"
#include "p_adc.h"
#include "p_uart.h"
#include "p_gpttim.h"
#include "p_i2c_sht20.h"
#include "p_wifi_sta.h"

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

void task_tcp(void *arg)
{

    int len = 0;
    char *rx_buffer = (char *)malloc(100);
    // while (1)
    // {
    //     uint32_t sockfd = wifi_tcp_client_init("192.168.8.100", 8080);
    //     len = send(sockfd, "123456789", strlen("123456789") - 1, 0);

    //     vTaskDelay(100 / portTICK);
    //     close(sockfd);
    // }

    uint32_t sockfd = wifi_tcp_client_init("192.168.8.100", 8080);
    while (1)
    {

        len = recv(sockfd, rx_buffer, 100, MSG_DONTWAIT);
        if (len > 0)
        {
            rx_buffer[len] = '\0';
            len = send(sockfd, rx_buffer, strlen(rx_buffer) - 1, 0);
            ESP_LOGI("LOG", "%s", rx_buffer);
        }
        vTaskDelay(50 / portTICK);
    }
    close(sockfd);
}

void app_main()
{
    led_init();
    wifi_sta_init(&wp, wifi_event_handler);
    xTaskCreate(task_tcp, "task_tcp", 1024 * 4, NULL, 5, &wifi_handle);
}
