#ifndef _P_WIFI_AP_H_
#define _P_WIFI_AP_H_

#include "common.h"

#include <nvs_flash.h>
#include <esp_event.h>
#include <esp_wifi.h>
#include <esp_netif_ip_addr.h>
#include "esp_netif.h"
#include <arpa/inet.h>

struct WiFi_Param
{
    char *ssid;
    char *password;
    char *ip;
    char *gateway;
    char *subnet;

};

extern struct WiFi_Param wp;

void wifi_ap_init(struct WiFi_Param *WiFi_Config, esp_event_handler_t esp_event_callback);

#endif