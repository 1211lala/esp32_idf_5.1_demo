#include "http_client.h"

static const char *TAG = "http";
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(TAG, "错误事件");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "连接成功事件");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGI(TAG, "发送头事件");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(TAG, "接收头事件");
        printf("%.*s", evt->data_len, (char *)evt->data);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG, "接收数据事件, len=%d", evt->data_len);
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            printf("%.*s\r\n", evt->data_len, (char *)evt->data);

            cJSON *pJsonRoot = cJSON_Parse((char *)evt->data);
            if (pJsonRoot != NULL)
            {
                printf("这是一个JSON\r\n");
                cJSON *pJsonAddress = NULL;
                pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "message");
                printf("message: %s\r\n", pJsonAddress->valuestring);
                pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "data");
                cJSON *pNextAddress = cJSON_GetObjectItem(pJsonAddress, "key");
                printf("key: %d\r\n", pNextAddress->valueint);
            }
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP会话完成");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP已断开");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}
/******************************************************************************
 * 函数描述: 将pin0设置为中断
 * 参  数1: 中断的回调函数,函数类型  void (*gpio_isr_t)(void *arg);
 * 参  数2: 中断函数携带的参数
 *******************************************************************************/
void http_get_request(const char *url)
{
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .event_handler = _http_event_handler,
        .buffer_size = 2048,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI("HTTP", "请求成功  CODE = %d, LEN = %llu", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE("HTTP", "请求失败: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}
/******************************************************************************
 * 函数描述: 将pin0设置为中断
 * 参  数1: 中断的回调函数,函数类型  void (*gpio_isr_t)(void *arg);
 * 参  数2: 中断函数携带的参数
 *******************************************************************************/
void http_post_request(const char *url)
{
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .event_handler = _http_event_handler,
        .buffer_size = 2048,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    const char *post_data = "{\"key\":12}";
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
        ESP_LOGI("HTTP", "请求成功  CODE = %d, LEN = %llu", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE("HTTP", "请求失败: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}