#include "common.h"
#include "bspled.h"
#include "bspkey.h"
#include "p_exti.h"
#include "p_adc.h"
#include "p_uart.h"
#include "p_gpttim.h"
#include "p_i2c_sht20.h"
#include "p_wifi_sta.h"
#include "spiffs.h"

#include "cJSON.h"

TaskHandle_t wifi_handle;
void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static int s_retry_num = 0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

        ESP_LOGI("TEST_ESP32", "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI("TEST_ESP32", "Got GW: " IPSTR, IP2STR(&event->ip_info.gw));
        ESP_LOGI("TEST_ESP32", "Got NT: " IPSTR, IP2STR(&event->ip_info.netmask));

        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < 10)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI("TAG", "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI("TAG", "connect to the AP fail");
    }
}

/*
    流程：创建JSON结构体 --> 添加数据 --> 释放内存
    流程：判断JSON格式 --> 解析数据 --> 释放内存

*/
void task_json(void *arg)
{
    /***************************************构建数据************************************************/
    // 创建父对象
    cJSON *pRoot = cJSON_CreateObject();
    // 在父对象根目录添加字符串数据
    cJSON_AddStringToObject(pRoot, "ssid", wp.ssid);
    cJSON_AddStringToObject(pRoot, "password", wp.password);
    // 在父对象根目录添加整型数据
    cJSON_AddNumberToObject(pRoot, "port", 2000);
    // 在父对象根目录添加整型数组
    int array[5] = {51, 15, 63, 22, 96};
    cJSON *pIntArray = cJSON_CreateIntArray(array, 5);
    cJSON_AddItemToObject(pRoot, "IntArray", pIntArray);
    // 在父对象根目录添加子对象
    cJSON *object = cJSON_CreateObject();
    cJSON_AddStringToObject(object, "ip", wp.ip);
    cJSON_AddStringToObject(object, "gateway", wp.gateway);
    cJSON_AddStringToObject(object, "subnet", wp.subnet);
    cJSON_AddItemToObject(pRoot, "ip_info", object);
    // 在父对象根目录添JSON数组,并在数组中新建两个子对象
    cJSON *jsonArray = cJSON_CreateArray();
    cJSON *pArray1 = cJSON_CreateObject();
    cJSON_AddStringToObject(pArray1, "name", "liuao");
    cJSON_AddNumberToObject(pArray1, "year", 18);
    cJSON_AddItemToArray(jsonArray, pArray1);
    cJSON *pArray2 = cJSON_CreateObject();
    cJSON_AddStringToObject(pArray2, "name", "xiaoming");
    cJSON_AddNumberToObject(pArray2, "year", 18);
    cJSON_AddItemToArray(jsonArray, pArray2);
    cJSON_AddItemToObject(pRoot, "info", jsonArray);
    // 获取格式化好的JSON字符串
    char *Json_string = cJSON_Print(pRoot);
    printf("%s\n", Json_string);

    /***************************************解析数据************************************************/
    // 首先整体判断是否为一个json格式的数据
    cJSON *pJsonAddress = NULL;
    cJSON *pJsonRoot = cJSON_Parse(Json_string);
    // 如果是否json格式数据
    if (pJsonRoot != NULL)
    {
        pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "ssid");
        if (!pJsonAddress)
            ESP_LOGE("JSON", "JSON中没有此对象");
        else
        { // 判断JSON的对象是否为String
            if (cJSON_IsString(pJsonAddress))
            {
                printf("ssid: %s\r\n", pJsonAddress->valuestring);
            }
        } // 下面不再做错误判断
        pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "password");
        printf("password: %s\r\n", pJsonAddress->valuestring);
        pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "port");
        printf("port: %d\r\n", pJsonAddress->valueint);
        //
        pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "IntArray");
        if (!pJsonAddress)
            return;
        else
        {
            int len = cJSON_GetArraySize(pJsonAddress);
            for (uint8_t i = 0; i < len; i++)
            {
                printf("IntArray[%d] = %d\r\n", i, cJSON_GetArrayItem(pJsonAddress, i)->valueint);
            }
        }

        pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "ip_info");
        if (!pJsonAddress)
            return;
        else
        {
            cJSON *address = cJSON_GetObjectItem(pJsonAddress, "ip");
            if (cJSON_IsString(address))
            {
                printf("ip: %s\r\n", address->valuestring);
            } // 不再做数据类型判断
            address = cJSON_GetObjectItem(pJsonAddress, "gateway");
            printf("gateway: %s\r\n", address->valuestring);
            address = cJSON_GetObjectItem(pJsonAddress, "subnet");
            printf("subnet: %s\r\n", address->valuestring);
        }

        pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "info");
        if (!pJsonAddress)
            return;
        else
        {
            cJSON *jsonAddress;
            cJSON *jsonData;
            int len = cJSON_GetArraySize(pJsonAddress);
            printf("JSON共有%d数据对象\r\n", len);
            for (uint8_t i = 0; i < len; i++)
            {
                jsonAddress = cJSON_GetArrayItem(pJsonAddress, i);

                jsonData = cJSON_GetObjectItem(jsonAddress, "name");
                printf("name: %s\r\n", jsonData->valuestring);
                jsonData = cJSON_GetObjectItem(jsonAddress, "year");
                printf("year: %d\r\n", jsonData->valueint);
            }
        }
    }
    cJSON_free((void *)Json_string);
    cJSON_Delete(pRoot);
    while (1)
    {
        vTaskDelay(1000 / portTICK);
        led_blink();
    }
}

void app_main()
{
    led_init();
    spiffs_mount();
    // wifi_sta_init(&wp, wifi_event_handler);
    xTaskCreate(task_json, "task_json", 1024 * 4, NULL, 5, &wifi_handle);
}
