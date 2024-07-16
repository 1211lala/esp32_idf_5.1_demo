#include "p_mqtt.h"
#include "p_wifi_sta.h"
#include "bspled.h"
#include "cJSON.h"

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

bool flag = false;
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
        // mqttdev.config.session.last_will.topic = mqttLastWill;
        // mqttdev.config.session.last_will.msg = MQTT_HOST_OFFLINE;
        // mqttdev.config.session.last_will.msg_len = strlen(MQTT_HOST_OFFLINE);
        // mqttdev.config.session.last_will.retain = true;
        // mqttdev.config.session.last_will.qos = 1;
        /* 设置主机监听时间 */
        // mqttdev.config.session.keepalive = 20;
        mqttdev.config.credentials.client_id = "client";

#if 1
        /* 设置登录密码 */
        mqttdev.config.credentials.username = "05Efh6cDv9";

        mqttdev.config.credentials.authentication.password = "version=2018-10-31&res=products%2F05Efh6cDv9%2Fdevices%2Fclient&et=1720576476000&method=md5&sign=zS1QoyD%2FY%2B55O0JDfb2DjQ%3D%3D";
#endif

        mqttdev.client = esp_mqtt_client_init(&mqttdev.config);
        esp_mqtt_client_register_event(mqttdev.client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
        esp_mqtt_client_start(mqttdev.client);

        printf("mqtt 正在连接......\r\n");
        ulTaskNotifyTake(0x00000000, portMAX_DELAY);
        printf("mqtt 连接成功\r\n");

        gpio_set_level(ledPin, 1);
        // if (gpio_get_level(ledPin))
        // {
        //     esp_mqtt_client_publish(mqttdev.client, mqttRoomLedCtrl, "on", 0, 1, 0);
        // }
        // else
        // {
        //     esp_mqtt_client_publish(mqttdev.client, mqttRoomLedCtrl, "off", 0, 1, 0);
        // }
        // const char *json_string = "{"
        //                           "\"id\": \"123\","
        //                           "\"version\": \"1.0\","
        //                           "\"params\": {"
        //                           "\"power\": {"
        //                           "\"value\": \"380\""
        //                           "},"
        //                           "\"temp\": {"
        //                           "\"value\": 5.6"
        //                           "}"
        //                           "}"
        //                           "}";
        float Temp = 1.10;
        int dianya = 0;
        char buf1[100];
        char buf2[100];
        while (1)
        {

            cJSON *Root = cJSON_CreateObject();

            cJSON_AddStringToObject(Root, "id", "123");
            cJSON_AddStringToObject(Root, "verion", "123");

            cJSON *Params = cJSON_CreateObject();
            cJSON_AddItemToObject(Root, "params", Params);

            cJSON *power = cJSON_CreateObject();
            cJSON_AddItemToObject(Params, "power", power);
            dianya = dianya + 1;
            sprintf(buf1, "%d", dianya);
            cJSON_AddStringToObject(power, "value", buf1);

            cJSON *temp = cJSON_CreateObject();
            cJSON_AddItemToObject(Params, "temp", temp);
            Temp += 0.13;
            sprintf(buf2, "%.2f", Temp);
            cJSON_AddStringToObject(temp, "value", buf2);
            char *json_string = cJSON_Print(Root);
            printf("%s\n", json_string);

            if (false == mqttdev.isConnected)
            {
                esp_mqtt_client_destroy(mqttdev.client);
                goto mqtt_connect_server;
            }

            esp_mqtt_client_publish(mqttdev.client, "$sys/05Efh6cDv9/client/thing/property/post", json_string, 0, 1, 0);
            cJSON_Delete(Root);
            cJSON_free(json_string);
            vTaskDelay(pdMS_TO_TICKS(5000));
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

        esp_mqtt_client_subscribe(mqttdev.client, "$sys/05Efh6cDv9/client/thing/property/post/+", 0);
        // esp_mqtt_client_subscribe(mqttdev.client, mqttFanRead, 0);
        // esp_mqtt_client_subscribe(mqttdev.client, mqttCurtainRead, 0);
        // esp_mqtt_client_subscribe(mqttdev.client, mqttWcLedRead, 0);

        // esp_mqtt_client_publish(mqttdev.client, mqttLastWill, MQTT_HOST_ONLINE, 0, 1, 1);
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
        if (0 == strcmp(topic, mqttRoomLedRead))
        {
            if (0 == strcmp(data, "off"))
            {
                gpio_set_level(ledPin, 0);
            }
            else if (0 == strcmp(data, "on"))
            {
                gpio_set_level(ledPin, 1);
            }
            flag = true;
        }
        break;
    case MQTT_EVENT_ERROR: // 出现错误
        mqttdev.isConnected = false;

        ESP_LOGE("mqtt", "mqtt出现错误");
        break;
    }
}
