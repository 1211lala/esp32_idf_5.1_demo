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

    uint8_t flag = 0;
    int connect_sock = 0;

    char *rx_buffer = (char *)malloc(100);

    uint32_t sockfd = wifi_tcp_server_init("192.168.8.100", 8080);

    struct sockaddr_in6 source_addr;
    socklen_t addr_len = sizeof(source_addr);

    while (1)
    {
        if (flag == 0)
        {
            flag = 1;
            connect_sock = accept(sockfd, (struct sockaddr *)&source_addr, &addr_len);
            if (connect_sock < 0)
            {
                ESP_LOGE("TCP_Server", "建立连接失败");
                close(sockfd);
            }
        }
        else
        {
            memset(rx_buffer, 0, 100);
            int len = recv(connect_sock, rx_buffer, 100, 0);
            if (len < 0)
            {
                ESP_LOGE("TCP_Server", "有错误发生在接收期间！！！ CODE(%d)", errno);
            }
            else if (len == 0)
            {
                ESP_LOGW("TCP_Server", "连接断开");
                close(connect_sock);
                flag = 0;
            }
            else
            {
                ESP_LOGI("TCP_Server", "收到 %d bytes: %s", len, rx_buffer);
                send(connect_sock, rx_buffer, len, 0);
            }
        }
    }
    close(connect_sock);
    close(sockfd);
}

void app_main()
{
    led_init();
    wifi_sta_init(&wp, wifi_event_handler);
    xTaskCreate(task_tcp, "task_tcp", 1024 * 4, NULL, 5, &wifi_handle);
    while (1)
    {
        led_blink();
        vTaskDelay(100);
    }
}
