#ifndef _P_WIFI_STA_H_
#define _P_WIFI_STA_H_

#include "common.h"

#include <esp_wifi.h>

#include <nvs_flash.h>
#include <esp_event.h>
#include <esp_wifi.h>
#include <esp_netif_ip_addr.h>
#include "esp_netif.h"
#include <arpa/inet.h>

#define WIFI_FAIL_BIT (1 << 0)
#define WIFI_CONNECTED_BIT (1 << 1)

struct WiFi_Param
{
    char *ssid;
    char *password;
    char *ip;
    char *gateway;
    char *subnet;
    char *dns;
    bool isConnected;
};

struct WiFi_List
{
    char ssid[30];
    uint8_t rssi;
    uint8_t primary;
};

extern struct WiFi_List *wifi_list;
extern struct WiFi_Param wp;

extern EventGroupHandle_t s_wifi_event_group;
void wifi_sta_init(struct WiFi_Param *WiFi_Config, esp_event_handler_t esp_event_callback);
int scan_wifi(struct WiFi_List *list);
void get_sta_wifi_info(void);
#endif