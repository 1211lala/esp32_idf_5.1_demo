#ifndef _WIFI_MQTT_H_
#define _WIFI_MQTT_H_

#include "datafile.h"



void mqtt_start_task(void);
void mqtt_task(void *arg);
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
#endif // !_p_MQTT_H_