#include "datafile.h"
#include "lvgl.h"
#include "lv_demos.h"
#include "wifi_sta.h"
#include "wifi_ap.h"
#include "wifi_mqtt.h"
#include "sd_spi_fat.h"
#include "lvgl_interface.h"
#include "./lvgl_app/lvglstart.h"
#include "sntp.h"
#include "http_server.h"
void wifi_event_callback(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static int retry_num = 0;
    // ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        retry_num = 0;
        wifidev.isConnected = true;

        // if (use_lvgl == true)
        // {
        //     // 为IP地址字符串分配正确大小的缓冲区 (最多15个字符 + 终止符)
        //     char temp[16];
        //     sprintf(temp, "%ld.%ld.%ld.%ld",
        //             (event->ip_info.ip.addr >> 0) & 0xFF,
        //             (event->ip_info.ip.addr >> 8) & 0xFF,
        //             (event->ip_info.ip.addr >> 16) & 0xFF,
        //             (event->ip_info.ip.addr >> 24) & 0xFF);
        //     const char *ssid = "Kean.2023";
        //     lv_label_set_text_fmt(wifi_label, "WIFI: %s\n\n  IP : %s", ssid, temp);
        // }

        xEventGroupSetBits(wifi_group, WIFI_CONNECTED_BIT);
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        wifidev.isConnected = false;
        if (retry_num < 10)
        {
            esp_wifi_connect();
            retry_num++;
            ESP_LOGE("wifi", "重新开始连接AP %d/%d", retry_num, 10);
        }
        else
        {
            // xEventGroupSetBits(wifi_group, WIFI_FAIL_BIT);
            ESP_LOGE("wifi", "连接失败即将进入AP模式");
            strncpy(wifidev.mode, "AP", sizeof(wifidev.mode));
            xTaskNotifyGive(fsHandle);
        }
    }
}

SemaphoreHandle_t fs_semaphore;
TaskHandle_t fsHandle;
void write_read_fs_task(void *arg)
{
    sd_init_mount_fat();

    read_wifi_info_from_fs("/wifi.json", (void *)&wifidev);
    ESP_LOGI("fs", "ssid: %s", wifidev.ssid);
    ESP_LOGI("fs", "pwd: %s", wifidev.password);
    ESP_LOGI("fs", "mode: %s", wifidev.mode);
    xSemaphoreGive(fs_semaphore);
    while (1)
    {
        ulTaskNotifyTake(fsHandle, portMAX_DELAY);
        printf("开始写入wifi数据");

        write_wifi_info_to_fs("/wifi.json", (void *)&wifidev);
        vTaskDelay(1000);
        esp_restart();
    }
}

void app_main(void)
{
    fs_semaphore = xSemaphoreCreateBinary();
    xTaskCreate(write_read_fs_task, "write_read_fs_task", 1024 * 10, NULL, 4, &fsHandle);
    if (xSemaphoreTake(fs_semaphore, portMAX_DELAY) == pdPASS)
    {
        vSemaphoreDelete(fs_semaphore);
    }


    if (strcmp(wifidev.mode, "AP") == 0)
    {
        printf("进入AP模式\r\n");

        wifi_ap_start(&wp, wifi_event_callback);
        wifi_list = malloc(sizeof(struct WiFi_List) * 10);
        scan_wifi(wifi_list);
        http_server_start(80);
    }
    else
    {
        printf("进入STA模式\r\n");
        wifi_connect_start(&wifidev, wifi_event_callback);
        sntp_start_task();

        mqtt_start_task();
        lvgl_start_task();
        http_server_start(80);
    }
}
