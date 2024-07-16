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

#include "lwip/sockets.h"


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
};

extern struct WiFi_Param wp;
extern EventGroupHandle_t s_wifi_event_group;
void wifi_sta_init(struct WiFi_Param *WiFi_Config, esp_event_handler_t esp_event_callback);
uint32_t wifi_tcp_server_init(const char *ip, uint16_t port);
#endif