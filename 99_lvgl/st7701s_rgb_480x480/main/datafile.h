#ifndef _DATAFILE_H
#define _DATAFILE_H

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"

/*******************************************WIFI********************************************************/
struct WIFI_STA_DEV
{
    char ssid[33];
    char password[33];
    char ip[16];
    char gateway[16];
    char subnet[16];
    char dns[16];
    char mode[5];
    bool isConnected;
};
extern struct WIFI_STA_DEV wifidev;

struct WiFi_List
{
    char ssid[33];
    uint8_t rssi;
    uint8_t primary;
};
extern struct WiFi_List *wifi_list;

extern EventGroupHandle_t wifi_group;
#define WIFI_FAIL_BIT (1 << 0)
#define WIFI_CONNECTED_BIT (1 << 1)
#define WIFI_IN_AP 0
#define WIFI_IN_STA 1
/*******************************************MQTT********************************************************/
#include "mqtt_client.h"
struct MQTT_DEV
{
    TaskHandle_t taskHandle;
    esp_mqtt_client_handle_t client;
    esp_mqtt_client_config_t config;
    bool isConnected;
};
extern struct MQTT_DEV mqttdev;

#define MQTT_TASK_PRIORITY 6

#define mqttUri "mqtt://mqtt.eclipseprojects.io"
// #define mqttUri "mqtt://192.168.10.40"

#define MQTT_HOST_ONLINE "esp32_01_online"
#define MQTT_HOST_OFFLINE "esp32_01_offline"

#define mqttRoomLedCtrl "/liuao/switch/roomLed/write"
#define mqttFanCtrl "/liuao/switch/fan/write"
#define mqttCurtainCtrl "/liuao/switch/curtain/write"
#define mqttWcLedCtrl "/liuao/switch/WcLed/write"

#define mqttRoomLedRead "/liuao/switch/roomLed/read"
#define mqttFanRead "/liuao/switch/fan/read"
#define mqttCurtainRead "/liuao/switch/curtain/read"
#define mqttWcLedRead "/liuao/switch/WcLed/read"

#define mqttLoop "/liuao/loop"
#define mqttLastWill "liuao/lastwill"
/*******************************************SNTP********************************************************/
#include "esp_netif_sntp.h"
#include "esp_sntp.h"

#define SNTP_TASK_PRIORITY 4
struct SNTP_DEV
{
    TaskHandle_t taskHandle;
    time_t nowTime;
    struct tm tmInfo;
    bool isOK;
};
extern struct SNTP_DEV sntpdev;

/*******************************************LVGL********************************************************/
#include "lvgl.h"

#define LVGL_TASK_PRIORITY 5
extern uint8_t use_lvgl;

extern lv_obj_t *switch0;
extern lv_obj_t *switch1;
extern lv_obj_t *switch2;
extern lv_obj_t *switch3;
extern lv_obj_t *label00;
extern lv_obj_t *label11;
extern lv_obj_t *label22;
extern lv_obj_t *label33;


/***********************************************FS***************************************************** */
extern SemaphoreHandle_t fs_semaphore;
extern TaskHandle_t fsHandle;
#endif
