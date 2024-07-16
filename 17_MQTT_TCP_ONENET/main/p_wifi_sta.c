#include "p_wifi_sta.h"

EventGroupHandle_t s_wifi_event_group = NULL;

struct WiFi_Param wp = {
    // .ssid = "Kean",
    // .password = "Kean.2023",
    // .ip = "192.168.8.188",
    // .gateway = "192.168.8.1",
    // .subnet = "255.255.255.0",
    // .dns = "114.114.114.114",
    // .ssid = "Xiaomi_4C",
    // .password = "121314liuAO#",
    // .ip = "192.168.31.188",
    // .gateway = "192.168.31.1",
    // .subnet = "255.255.255.0",
    // .dns = "114.114.114.114",
    .ssid = "艾可舒",
    .password = "ai666888",
    // .ssid = "realme GT Neo5",
    // .password = "12111211",
};

// void wifi_sta_init(struct WiFi_Param *WiFi_Config, esp_event_handler_t esp_event_callback)
// {

//     s_wifi_event_group = xEventGroupCreate();
//     // 1: 初始化 NVS
//     esp_err_t err = nvs_flash_init();
//     if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
//     { // 如果 NVS 分区已满或者版本不匹配，需要擦除 NVS 分区并重新初始化
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         ESP_ERROR_CHECK(nvs_flash_init());
//     }
//     // 2: 初始化网络接口
//     ESP_ERROR_CHECK(esp_netif_init());

//     // 3: 创建默认的事件循环
//     ESP_ERROR_CHECK(esp_event_loop_create_default());

//     // 4: 创建默认的 Wi-Fi Station 网络接口
//     esp_netif_create_default_wifi_sta();

//     // 5: 初始化 Wi-Fi 栈，之后可以进行Wi-Fi 连接、启动 Wi-Fi 等操作。
//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     // 6: 配置WiFi模式
//     esp_wifi_set_mode(WIFI_MODE_STA);

//     // 7: 配置STA参数 如SSID, 密码等参数
//     wifi_config_t cfg_sta = {};
//     strcpy((char *)cfg_sta.sta.ssid, WiFi_Config->ssid);
//     strcpy((char *)cfg_sta.sta.password, WiFi_Config->password);
//     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cfg_sta));

//     /**************************************可选 配置静态IP**********************************************/
//     ESP_ERROR_CHECK(esp_netif_dhcpc_stop(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF")));
//     esp_netif_ip_info_t ip_inifo = {};
//     inet_aton(wp.ip, &ip_inifo.ip);
//     inet_aton(wp.gateway, &ip_inifo.gw);
//     inet_aton(wp.subnet, &ip_inifo.netmask);
//     ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_inifo));
//     /**************************************可选 配置静态IP**********************************************/
//     /**************************************可选 配置DNS服务器**********************************************/
//     esp_netif_dns_info_t dnsInfo = {};
//     inet_aton(wp.dns, &dnsInfo.ip.u_addr.ip4);
//     ESP_ERROR_CHECK(esp_netif_set_dns_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), ESP_NETIF_DNS_MAIN, &dnsInfo));
//     /**************************************可选 配置DNS服务器**********************************************/

//     // 9: 设置WiFi事件
//     esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, esp_event_callback, NULL, NULL);
//     esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, esp_event_callback, NULL, NULL);

//     /*
//     10: 开启WiFi，调用 esp_wifi_start() 函数后，ESP32 将开始扫描 Wi-Fi 网络并尝试连接。
//         在这个过程中，系统会产生多种类型的 Wi-Fi 事件，例如扫描结果事件、连接事件等。
//         这些事件会通过事件循环传递给注册的事件处理函数。
//     */
//     esp_wifi_start();

//     EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
//     if (bits & WIFI_CONNECTED_BIT)
//     {
//         ESP_LOGI("TAG", "connected to ap SSID:%s password:%s", wp.ssid, wp.password);
//     }
//     else if (bits & WIFI_FAIL_BIT)
//     {
//         ESP_LOGI("TAG", "Failed to connect to SSID:%s, password:%s", wp.ssid, wp.password);
//     }
// }

struct WiFi_List *wifi_list;

void wifi_sta_init(struct WiFi_Param *WiFi_Config, esp_event_handler_t esp_event_callback)
{

    s_wifi_event_group = xEventGroupCreate();
    // 1: 初始化 NVS
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

    // 4: 创建默认的 Wi-Fi Station 网络接口
    esp_netif_create_default_wifi_sta();

    // 5: 初始化 Wi-Fi 栈，之后可以进行Wi-Fi 连接、启动 Wi-Fi 等操作。
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 6: 配置WiFi模式
    esp_wifi_set_mode(WIFI_MODE_STA);
    /*
    10: 开启WiFi，调用 esp_wifi_start() 函数后，ESP32 将开始扫描 Wi-Fi 网络并尝试连接。
        在这个过程中，系统会产生多种类型的 Wi-Fi 事件，例如扫描结果事件、连接事件等。
        这些事件会通过事件循环传递给注册的事件处理函数。
    */
    esp_wifi_start();

    wifi_list = malloc(sizeof(struct WiFi_List) * 10);
    scan_wifi(wifi_list);

    // 7: 配置STA参数 如SSID, 密码等参数
    wifi_config_t cfg_sta = {};
    strcpy((char *)cfg_sta.sta.ssid, WiFi_Config->ssid);
    strcpy((char *)cfg_sta.sta.password, WiFi_Config->password);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cfg_sta));

    /**************************************可选 配置静态IP**********************************************/
    // ESP_ERROR_CHECK(esp_netif_dhcpc_stop(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF")));
    // esp_netif_ip_info_t ip_inifo = {};
    // inet_aton(wp.ip, &ip_inifo.ip);
    // inet_aton(wp.gateway, &ip_inifo.gw);
    // inet_aton(wp.subnet, &ip_inifo.netmask);
    // ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_inifo));
    /**************************************可选 配置静态IP**********************************************/
    /**************************************可选 配置DNS服务器**********************************************/
    // esp_netif_dns_info_t dnsInfo = {};
    // inet_aton(wp.dns, &dnsInfo.ip.u_addr.ip4);
    // ESP_ERROR_CHECK(esp_netif_set_dns_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), ESP_NETIF_DNS_MAIN, &dnsInfo));
    /**************************************可选 配置DNS服务器**********************************************/

    // 9: 设置WiFi事件
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, esp_event_callback, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, esp_event_callback, NULL, NULL);

    /* 开始连接wifi */
    esp_wifi_connect();
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI("TAG", "connected to ap SSID:%s password:%s", wp.ssid, wp.password);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI("TAG", "Failed to connect to SSID:%s, password:%s", wp.ssid, wp.password);
    }

    get_sta_wifi_info();
}

int scan_wifi(struct WiFi_List *list)
{
    uint16_t ap_count = 0;
    uint16_t number = 10;
    wifi_ap_record_t ap_info[10];

    memset(ap_info, 0, sizeof(ap_info));

    esp_wifi_scan_start(NULL, true);
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI("scan_wifi", "Total APs scanned = %u", ap_count);

    for (int i = 0; (i < 10) && (i < ap_count); i++)
    {
        memcpy(list[i].ssid, ap_info[i].ssid, strlen((char *)ap_info[i].ssid));

        list[i].ssid[strlen((char *)ap_info[i].ssid)] = '\0';

        ESP_LOGI("scan_wifi", "SSID \t\t%s", ap_info[i].ssid);
        ESP_LOGI("scan_wifi", "RSSI \t\t%d", ap_info[i].rssi);
        ESP_LOGI("scan_wifi", "Channel \t\t%d\n", ap_info[i].primary);
    }
    return ap_count;
}

void get_sta_wifi_info(void)
{
    wifi_config_t config;
    esp_wifi_get_config(WIFI_IF_STA, &config);
    printf("ssid: %s    password: %s\r\n", (char *)config.sta.ssid, (char *)config.sta.password);

    esp_netif_ip_info_t ip_info;
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_get_ip_info(netif, &ip_info);
    printf("ip: " IPSTR " gateway: " IPSTR "  subnet: " IPSTR "\r\n", IP2STR(&ip_info.ip), IP2STR(&ip_info.gw), IP2STR(&ip_info.netmask));

    esp_netif_dns_info_t dsn_info;
    esp_netif_get_dns_info(netif, ESP_NETIF_DNS_MAIN, &dsn_info);
    printf("dns: " IPSTR "\r\n", IP2STR(&dsn_info.ip.u_addr.ip4));
}