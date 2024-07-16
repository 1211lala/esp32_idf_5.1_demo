#include "sntp.h"

static const char *TAG = "sntp";

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

time_t now;
struct tm timeinfo;

/******************************************************************************
 * 函数描述: 用于使用sntp的初始化，使用前请确定已经连接WIFI
 *******************************************************************************/
void ntp_init(void)
{
    char strftime_buf[64];

    /*1: 先获取当前设备的时间 */
    time(&now);
    localtime_r(&now, &timeinfo);
    ESP_LOGE(TAG, "当前设备时间为: %d", timeinfo.tm_year + 1900);

    /*2: 如果时间不对开始设置时间 */
    if (timeinfo.tm_year < (2021 - 1900))
    {
        ESP_LOGI(TAG, "初始化SNTP");
        esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("ntp.aliyun.com");

        config.sync_cb = time_sync_notification_cb;
        esp_netif_sntp_init(&config);

        int retry = 0;
        while (esp_netif_sntp_sync_wait(500 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT && ++retry < 20)
        {
            ESP_LOGI(TAG, "等待系统时间被设置......(%d/%d)", retry, 20);
            if (retry == 9)
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
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
    if (sntp_get_sync_mode() == SNTP_SYNC_MODE_IMMED)
    {
        ESP_LOGE(TAG, "突变模式");
    }
}
/******************************************************************************
 * 函数描述: 获得时间
 *******************************************************************************/
void get_time(void)
{
    char strftime_buf[64];
    time(&now);
    // setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
    setenv("TZ", "CST-8", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "%s", strftime_buf);
    // ESP_LOGI(TAG, "tm_sec = %d", timeinfo.tm_sec);
    // ESP_LOGI(TAG, "tm_min = %d", timeinfo.tm_min);
    // ESP_LOGI(TAG, "tm_hour = %d", timeinfo.tm_hour);
    // ESP_LOGI(TAG, "tm_mday = %d", timeinfo.tm_mday);
    // ESP_LOGI(TAG, "tm_year = %d", timeinfo.tm_year);
    // ESP_LOGI(TAG, "tm_wday = %d", timeinfo.tm_wday);
    // ESP_LOGI(TAG, "tm_yday = %d", timeinfo.tm_yday);
    // ESP_LOGI(TAG, "tm_isdst = %d", timeinfo.tm_isdst);
}