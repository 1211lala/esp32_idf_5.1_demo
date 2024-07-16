#include "p_mqtt.h"

struct mqtt_dev mqttdev;

static const char *TAG = "mqtt";

void mqtt_init(void)
{
    xTaskCreate(mqtt_task, "mqtt_task", 1024 * 3, NULL, 3, &mqttdev.taskHandle);
    vTaskSuspend(mqttdev.taskHandle);

    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = mqttUri;

    mqtt_cfg.session.last_will.topic = "liuao/lastWill";
    mqtt_cfg.session.last_will.msg = "offline";
    mqtt_cfg.session.last_will.msg_len = strlen("offline");
    mqtt_cfg.session.last_will.retain = true;
    mqtt_cfg.session.last_will.qos = 1;

    mqtt_cfg.session.keepalive = 20;

    mqttdev.client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqttdev.client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqttdev.client);
}

void mqtt_task(void *arg)
{
    esp_mqtt_client_subscribe(mqttdev.client, mqttTopic_1, 0);
    esp_mqtt_client_subscribe(mqttdev.client, mqttTopic_2, 0);
    esp_mqtt_client_subscribe(mqttdev.client, mqttTopic_3, 0);
    esp_mqtt_client_unsubscribe(mqttdev.client, mqttTopic_3);

    esp_mqtt_client_publish(mqttdev.client, "liuao/lastWill", "online", 0, 1, 1);
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        esp_mqtt_client_publish(mqttdev.client, mqttTopic_1, "/liuao/sensor/temp/room_1", 0, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
        esp_mqtt_client_enqueue(mqttdev.client, mqttTopic_2, "/liuao/sensor/temp/room_2", 0, 0, 0, true);
    }
}


void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch (event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "mqtt服务端连接成功\r\n");
        vTaskResume(mqttdev.taskHandle);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "mqtt断开连接成功\r\n");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "mqtt订阅成功\r\n");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "mqtt取消订阅成功\r\n");
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "mqtt发布成功\r\n");
        break;
    case MQTT_EVENT_DATA:
        char topic[100];
        char data[100];
        memcpy(topic, event->topic, event->topic_len);
        memcpy(data, event->data, event->data_len);
        topic[event->topic_len] = '\0';
        topic[event->data_len] = '\0';
        printf("topic: %s  data: %s\r\n", topic, data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE("mqtt", "mqtt出现错误");
        break;
    }
}