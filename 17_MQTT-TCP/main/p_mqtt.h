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

#define mqttUri "mqtt://mqtt.eclipseprojects.io"
#define mqttTopic_1 "/liuao/sensor/temp/room_1"
#define mqttTopic_2 "/liuao/sensor/temp/room_2"
#define mqttTopic_3 "/liuao/sensor/temp/room_3"
#define mqttTopic_4 "/liuao/sensor/temp/room_4"
struct mqtt_dev
{
    TaskHandle_t taskHandle;
    esp_mqtt_client_handle_t client;
};
extern struct mqtt_dev mqtt_dev;

void mqtt_init(void);
void mqtt_task(void *arg);
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
#endif // !_p_MQTT_H_