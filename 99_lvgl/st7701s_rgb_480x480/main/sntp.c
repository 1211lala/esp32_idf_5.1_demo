#include "sntp.h"

static const char *TAG = "sntp";

struct SNTP_DEV sntpdev = {
    .isOK = false,
};

void time_sync_notification_cb(struct timeval *tv)
{
    sntpdev.isOK = true;
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

void sntp_start_task(void)
{
    xTaskCreate(sntp_task, "sntp_task", 1024 * 4, NULL, SNTP_TASK_PRIORITY, &mqttdev.taskHandle);
}

void sntp_task(void *arg)
{
    while (!wifidev.isConnected)
    {
        ESP_LOGE(TAG, "设备未连接WIFI");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelay(pdMS_TO_TICKS(500));
    sntp_check_time(&sntpdev);
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        sntp_get_time(&sntpdev);
    }
}
/******************************************************************************
 * 函数描述: 用于使用sntp的初始化，使用前请确定已经连接WIFI
 *******************************************************************************/
void sntp_check_time(struct SNTP_DEV *sntp)
{
    /*1: 先获取当前设备的时间 */
    time(&sntp->nowTime);
    localtime_r(&sntp->nowTime, &sntp->tmInfo);
    ESP_LOGE(TAG, "当前设备时间为: %d", sntp->tmInfo.tm_year + 1900);

    /*2: 如果时间不对开始设置时间 */
    if (sntp->tmInfo.tm_year < (2021 - 1900))
    {
        ESP_LOGI(TAG, "开始同步SNTP时间");
        esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("ntp.aliyun.com");

        config.sync_cb = time_sync_notification_cb;
        esp_netif_sntp_init(&config);

        int retry = 0;
        while (esp_netif_sntp_sync_wait(500 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT && ++retry < 100)
        {
            ESP_LOGI(TAG, "等待SNTP时间被设置......(%d/%d)", retry, 100);
            if (retry == 100)
            {
                ESP_LOGE(TAG, "设置失败,即将重启");
                esp_deep_sleep(1000000LL);
            }
        }
    }
    /*可选 3: 判断SNTP模式*/
    if (sntp_get_sync_mode() == SNTP_SYNC_MODE_SMOOTH)
    {
        struct timeval outdelta;
        while (sntp_get_sync_status() == SNTP_SYNC_STATUS_IN_PROGRESS)
        {
            ESP_LOGE(TAG, "渐变模式");
            adjtime(NULL, &outdelta);
            ESP_LOGI(TAG, "Waiting for adjusting time ... outdelta = %jd sec: %li ms: %li us", (intmax_t)outdelta.tv_sec, outdelta.tv_usec / 1000, outdelta.tv_usec % 1000);
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
    if (sntp_get_sync_mode() == SNTP_SYNC_MODE_IMMED)
    {
        ESP_LOGI(TAG, "突变模式");
    }
}
/******************************************************************************
 * 函数描述: 获得时间
 *******************************************************************************/
void sntp_get_time(struct SNTP_DEV *sntp)
{
    time(&sntp->nowTime);
    // setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
    setenv("TZ", "CST-8", 1);
    tzset();
    localtime_r(&sntp->nowTime, &sntp->tmInfo);
    // char strftime_buf[64];
    // strftime(strftime_buf, sizeof(strftime_buf), "%c", &sntp->tmInfo);
    // ESP_LOGI(TAG, "%s", strftime_buf);
    // ESP_LOGI(TAG, "tm_sec = %d", timeinfo.tm_sec);
    // ESP_LOGI(TAG, "tm_min = %d", timeinfo.tm_min);
    // ESP_LOGI(TAG, "tm_hour = %d", timeinfo.tm_hour);
    // ESP_LOGI(TAG, "tm_mday = %d", timeinfo.tm_mday);
    // ESP_LOGI(TAG, "tm_year = %d", timeinfo.tm_year);
    // ESP_LOGI(TAG, "tm_wday = %d", timeinfo.tm_wday);
    // ESP_LOGI(TAG, "tm_yday = %d", timeinfo.tm_yday);
    // ESP_LOGI(TAG, "tm_isdst = %d", timeinfo.tm_isdst);
}