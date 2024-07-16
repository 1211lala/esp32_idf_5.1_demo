#include "spiffs.h"

static const char *TAG = "spiffs";

esp_vfs_spiffs_conf_t conf = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true};

void fs_mount(void)
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
        ESP_LOGI(TAG, "SPIFFS分区大小为%dK, 已使用: %dK", total / 1024, used / 1024);
    }
    fs_scan();
}

void fs_scan(void)
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

/******************************************************************************
 * 函数描述: 检测文件是否存在
 * 参  数1: 文件路径
 * 返  回:  文件的大小/ (-1 不存在)
 * st_size：文件的大小（以字节为单位）
 * st_mode：文件的类型和权限。
 * st_uid：文件的所有者的用户 ID。
 * st_atime：文件的最后访问时间。
 * st_mtime：文件的最后修改时间。
 *******************************************************************************/
int isExist(const char *path)
{
    struct stat st;
    if (stat(path, &st) != F_OK)
    {
        ESP_LOGE(TAG, "%s不存在", path);
        return -1;
    }
    return st.st_size;
}
/******************************************************************************
 * 函数描述: 读取文件内容z
 * 参  数1: 文件路径
 * 参  数2: 缓存buffer
 * 参  数3: buffer大小
 * 返  回:  实际读取的字节数(字节)
 *******************************************************************************/
int fs_read(const char *path, char *buffer, uint32_t size)
{
    int len = 0;
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        ESP_LOGE(TAG, "%s打开失败", path);
        fclose(file);
        return -1;
    }
    else
    {
        len = fread(buffer, 1, size, file);
    }
    fclose(file);
    return len;
}