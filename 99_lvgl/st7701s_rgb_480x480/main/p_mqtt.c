#include "p_mqtt.h"
#include "p_wifi_sta.h"

static const char *TAG = "mqtt";

#define MQTT_TASK_PRIORITY 6
#define MQTT_HOST_ONLINE "esp32_01_online"
#define MQTT_HOST_OFFLINE "esp32_01_offline"

struct mqtt_dev mqttdev = {
    .isConnected = false,
};

void mqtt_task_start(void)
{
    xTaskCreate(mqtt_task, "mqtt_task", 1024 * 4, NULL, MQTT_TASK_PRIORITY, &mqttdev.taskHandle);
}
uint32_t cnt = 0;
uint32_t error = 0;
void mqtt_task(void *arg)
{

    while (1)
    {
    mqtt_connect_server:

        while (!wp.isConnected)
        {
            printf("wifi 未连接!!!!\r\n");
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

        printf("mqtt 正在连接......\r\n");
        ulTaskNotifyTake(0x00000000, portMAX_DELAY);
        printf("mqtt 连接成功\r\n");

        while (1)
        {
            if (false == mqttdev.isConnected)
            {
                esp_mqtt_client_destroy(mqttdev.client);
                goto mqtt_connect_server;
            }
            char buf[100];
            sprintf(buf, "time: %ld  error:%ld\r\n", cnt++, error);

            esp_mqtt_client_publish(mqttdev.client, mqttNowTime, buf, 0, 1, 0);
            vTaskDelay(pdMS_TO_TICKS(250));
            esp_mqtt_client_publish(mqttdev.client, mqttFanCtrl, "mqttFanCtrl", 0, 1, 0);
            vTaskDelay(pdMS_TO_TICKS(250));
            esp_mqtt_client_publish(mqttdev.client, mqttCurtainCtrl, "mqttCurtainCtrl", 0, 1, 0);
            vTaskDelay(pdMS_TO_TICKS(250));
            esp_mqtt_client_publish(mqttdev.client, mqttCurtainCtrl, "mqttCurtainCtrl", 0, 1, 0);
            vTaskDelay(pdMS_TO_TICKS(250));
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

        esp_mqtt_client_subscribe(mqttdev.client, mqttRoomLedRead, 0);
        esp_mqtt_client_subscribe(mqttdev.client, mqttFanRead, 0);
        esp_mqtt_client_subscribe(mqttdev.client, mqttCurtainRead, 0);
        esp_mqtt_client_subscribe(mqttdev.client, mqttWcLedRead, 0);

        esp_mqtt_client_publish(mqttdev.client, mqttLastWill, MQTT_HOST_ONLINE, 0, 1, 1);
        break;
    case MQTT_EVENT_DISCONNECTED: // 断开连接成功
        mqttdev.isConnected = false;
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

        break;
    case MQTT_EVENT_ERROR: // 出现错误
        mqttdev.isConnected = false;
        error += 1;

        ESP_LOGE("mqtt", "mqtt出现错误");
        break;
    }
}
