#ifndef _p_MQTT_H_
#define _p_MQTT_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_partition.h"
#include "nvs_flash.h"
#include "esp_event.h"

#include "esp_netif.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include "esp_ota_ops.h"
#include <sys/param.h>

#define mqttUri "mqtt://mqtts.heclouds.com"

#define mqttRoomLedCtrl "/liuao/switch/roomLed/write"
#define mqttFanCtrl "/liuao/switch/fan/write"
#define mqttCurtainCtrl "/liuao/switch/curtain/write"
#define mqttWcLedCtrl "/liuao/switch/WcLed/write"

#define mqttRoomLedRead "/liuao/switch/roomLed/read"
#define mqttFanRead "/liuao/switch/fan/read"
#define mqttCurtainRead "/liuao/switch/curtain/read"
#define mqttWcLedRead "/liuao/switch/WcLed/read"

#define mqttLoop "/liuao/loop"
#define mqttLastWill "/liuao/lastwill"

struct mqtt_dev
{
    TaskHandle_t taskHandle;
    esp_mqtt_client_handle_t client;
    esp_mqtt_client_config_t config;
    bool isConnected;
};
extern struct mqtt_dev mqttdev;

void mqtt_task_start(void);
void mqtt_task(void *arg);
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
#endif // !_p_MQTT_H_