#ifndef _WIFI_STA_H_
#define _WIFI_STA_H_

#include "datafile.h"
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <esp_event.h>
#include <esp_wifi.h>
#include <esp_netif_ip_addr.h>
#include "esp_netif.h"
#include <arpa/inet.h>


void wifi_connect_start(struct WIFI_STA_DEV *WiFi_Config, esp_event_handler_t esp_event_callback);

int scan_wifi(struct WiFi_List *list);
void get_sta_wifi_info(void);

#endif