/**
 * 免费的Mqtt服务器地址 "mqtt://mqtt.eclipseprojects.io"
 *
 * mqtx软件下载连接 https://www.emqx.io/zh/downloads?os=Windows
 *
 *
 *
 */

#include "wifi_mqtt.h"

static const char *TAG = "mqtt";

struct MQTT_DEV mqttdev = {
    .isConnected = false,
};

void mqtt_start_task(void)
{
    xTaskCreate(mqtt_task, "mqtt_task", 1024 * 4, NULL, MQTT_TASK_PRIORITY, &mqttdev.taskHandle);
}

void mqtt_task(void *arg)
{
    while (1)
    {
    mqtt_connect_server:
        while (!wifidev.isConnected)
        {
            ESP_LOGE(TAG, "设备未连接WIFI");
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        vTaskDelay(pdMS_TO_TICKS(500));

        mqttdev.config.broker.address.uri = mqttUri;
        /* 设置遗嘱信息 */
        mqttdev.config.session.last_will.topic = mqttLastWill;
        mqttdev.config.session.last_will.msg = MQTT_HOST_OFFLINE;
        mqttdev.config.session.last_will.msg_len = strlen(MQTT_HOST_OFFLINE);
        mqttdev.config.session.last_will.retain = true;
        mqttdev.config.session.last_will.qos = 1;
        /* 设置主机监听时间 */
        mqttdev.config.session.keepalive = 20;

#if 0
        /* 设置登录密码 */
        mqttdev.config.credentials.username = "user-1";
        mqttdev.config.credentials.authentication.password = "user-1";
#endif

        mqttdev.client = esp_mqtt_client_init(&mqttdev.config);
        esp_mqtt_client_register_event(mqttdev.client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
        esp_mqtt_client_start(mqttdev.client);

        ESP_LOGI(TAG, "正在连接......");
        ulTaskNotifyTake(0x00000000, portMAX_DELAY);
        ESP_LOGI(TAG, "连接成功");

        while (1)
        {
            if (false == mqttdev.isConnected)
            {
                esp_mqtt_client_destroy(mqttdev.client);
                goto mqtt_connect_server;
            }
            vTaskDelay(pdMS_TO_TICKS(250));
            // esp_mqtt_client_publish(mqttdev.client, mqttFanCtrl, "mqttFanCtrl", 0, 1, 0);
            // vTaskDelay(pdMS_TO_TICKS(250));
            // esp_mqtt_client_publish(mqttdev.client, mqttCurtainCtrl, "mqttCurtainCtrl", 0, 1, 0);
            // vTaskDelay(pdMS_TO_TICKS(250));
            // esp_mqtt_client_publish(mqttdev.client, mqttCurtainCtrl, "mqttCurtainCtrl", 0, 1, 0);
            // vTaskDelay(pdMS_TO_TICKS(250));
        }
    }
}

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch (event_id)
    {
    case MQTT_EVENT_CONNECTED: // 连接成功
        mqttdev.isConnected = true;
        xTaskNotifyGive(mqttdev.taskHandle);

        esp_mqtt_client_subscribe(mqttdev.client, mqttRoomLedCtrl, 0);
        // esp_mqtt_client_subscribe(mqttdev.client, mqttFanRead, 0);
        // esp_mqtt_client_subscribe(mqttdev.client, mqttCurtainRead, 0);
        // esp_mqtt_client_subscribe(mqttdev.client, mqttWcLedRead, 0);

        esp_mqtt_client_publish(mqttdev.client, mqttLastWill, MQTT_HOST_ONLINE, 0, 1, 1);
        break;
    case MQTT_EVENT_DISCONNECTED: // 断开连接成功
        mqttdev.isConnected = false;
        ESP_LOGE("mqtt", "断开与服务器的连接");
        break;
    case MQTT_EVENT_SUBSCRIBED: // 订阅成功
        break;
    case MQTT_EVENT_UNSUBSCRIBED: // 订阅成功
        break;
    case MQTT_EVENT_PUBLISHED: // 发布成功
        break;
    case MQTT_EVENT_DATA:
        char topic[100];
        char data[100];
        memcpy(topic, event->topic, event->topic_len);
        memcpy(data, event->data, event->data_len);
        topic[event->topic_len] = '\0';
        data[event->data_len] = '\0';
        printf("topic: %s  data: %s\r\n", topic, data);
        if (use_lvgl == true)
        {
            if (0 == strcmp(topic, mqttRoomLedCtrl))
            {
                if (strcmp(data, "on") == 0)
                {
                    lv_label_set_text_fmt(label00, "ON");
                    lv_obj_add_state(switch0, LV_STATE_CHECKED);
                }
                if (strcmp(data, "off") == 0)
                {
                    lv_label_set_text_fmt(label00, "OFF");
                    lv_obj_clear_state(switch0, LV_STATE_CHECKED);
                }
            }
            else if (0 == strcmp(topic, mqttFanRead))
            {
                if (strcmp(data, "on") == 0)
                {
                    lv_label_set_text_fmt(label11, "ON");
                    lv_obj_add_state(switch1, LV_STATE_CHECKED);
                }
                if (strcmp(data, "off") == 0)
                {
                    lv_label_set_text_fmt(label11, "OFF");
                    lv_obj_clear_state(switch1, LV_STATE_CHECKED);
                }
            }
            else if (0 == strcmp(topic, mqttCurtainRead))
            {
                if (strcmp(data, "on") == 0)
                {
                    lv_label_set_text_fmt(label22, "ON");
                    lv_obj_add_state(switch2, LV_STATE_CHECKED);
                }
                if (strcmp(data, "off") == 0)
                {
                    lv_label_set_text_fmt(label22, "OFF");
                    lv_obj_clear_state(switch2, LV_STATE_CHECKED);
                }
            }
            else if (0 == strcmp(topic, mqttWcLedRead))
            {
                if (strcmp(data, "on") == 0)
                {
                    lv_label_set_text_fmt(label33, "ON");
                    lv_obj_add_state(switch3, LV_STATE_CHECKED);
                }
                if (strcmp(data, "off") == 0)
                {
                    lv_label_set_text_fmt(label33, "OFF");
                    lv_obj_clear_state(switch3, LV_STATE_CHECKED);
                }
            }
        }
        break;
    case MQTT_EVENT_ERROR: // 出现错误
        mqttdev.isConnected = false;
        ESP_LOGE("mqtt", "出现错误");
        break;
    }
}
