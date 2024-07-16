#include "spiffs.h"

esp_vfs_spiffs_conf_t conf = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true};

static char *TAG = "spiffs";

void spiffs_mount(void)
{
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "挂载SPIFFS失败");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "无法找到SPIFFS分区");
        }
        else
        {
            ESP_LOGE(TAG, "初始化失败!!!  CODE(%s)", esp_err_to_name(ret));
        }
        return;
    }
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "获取SPIFFS分区信息失败  CODE(%s).", esp_err_to_name(ret));
        // esp_spiffs_format(conf.partition_label);
        return;
    }
    else
    {
        ESP_LOGI(TAG, "分区大小为%dK, used: %dK", total / 1024, used / 1024);
    }
}

void spiffs_scan(void)
{
    char filepath[300];
    DIR *dir = opendir("/spiffs");
    struct dirent *entry;
    if (!dir)
    {
        ESP_LOGE(TAG, "打开/spiffs失败");
        return;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        sprintf(filepath, "/spiffs/%s", (char *)entry->d_name);

        struct stat st;
        if (stat(filepath, &st) == 0)
        {
            ESP_LOGI(TAG, "File: %-20s Size: %ld bytes", entry->d_name, st.st_size);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to get file stats");
        }
    }
    closedir(dir);
}