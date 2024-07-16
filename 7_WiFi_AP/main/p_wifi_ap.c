#include "p_wifi_ap.h"

struct WiFi_Param wp = {
    .ssid = "ESP32 - WiFi",
    .password = "123456789",
    .ip = "192.168.1.88",
    .gateway = "192.168.1.1",
    .subnet = "255.255.255.0"};

void wifi_ap_init(struct WiFi_Param *WiFi_Config, esp_event_handler_t esp_event_callback)
{
    // 1 : 初始化 NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    { // 如果 NVS 分区已满或者版本不匹配，需要擦除 NVS 分区并重新初始化
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    // 2: 初始化网络接口
    ESP_ERROR_CHECK(esp_netif_init());

    // 3: 创建默认的事件循环
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 4 : 创建默认的 Wi-Fi AP 网络接口
    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
    /*********************************设置固定IP可选******************************************/
    esp_netif_ip_info_t info_t;
    memset(&info_t, 0, sizeof(esp_netif_ip_info_t));
    if (ap_netif)
    {
        ESP_ERROR_CHECK(esp_netif_dhcps_stop(ap_netif));
        info_t.ip.addr = esp_ip4addr_aton(WiFi_Config->ip);
        info_t.netmask.addr = esp_ip4addr_aton(WiFi_Config->subnet);
        info_t.gw.addr = esp_ip4addr_aton(WiFi_Config->gateway);
        esp_netif_set_ip_info(ap_netif, &info_t);
        ESP_ERROR_CHECK(esp_netif_dhcps_start(ap_netif));
    }
    /*********************************设置固定IP可选******************************************/
    // 5: 初始化 Wi-Fi 栈，之后可以进行Wi-Fi 连接、启动 Wi-Fi 等操作。
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 6: 配置WiFi模式
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    // 7: 配置WiFi参数
    wifi_config_t ap_cfg = {
        .ap = {
            .ssid = "esp32- widfi",
            .ssid_len = strlen("esp32- widfi"),
            .channel = 2,
            .password = "111222333",
            .max_connection = 3,
            .authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg));

    // 9: 设置WiFi事件
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, esp_event_callback, NULL, NULL);
    /*
    10: 开启WiFi，调用 esp_wifi_start() 函数后，ESP32 将开始扫描 Wi-Fi 网络并尝试连接。
        在这个过程中，系统会产生多种类型的 Wi-Fi 事件，例如扫描结果事件、连接事件等。
        这些事件会通过事件循环传递给注册的事件处理函数。
    */
    ESP_ERROR_CHECK(esp_wifi_start());
}