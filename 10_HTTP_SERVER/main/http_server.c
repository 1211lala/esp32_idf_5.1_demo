#include "http_server.h"
#include "p_wifi_sta.h"
httpd_handle_t server = NULL;

static const char *TAG = "http";

/* 设置URL */
static const httpd_uri_t route = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = http_route_callback};

static const httpd_uri_t postdata = {
    .uri = "/postdata",
    .method = HTTP_POST,
    .handler = http_postdata_callback};

/******************************************************************************
 * 函数描述: 初始化HTTP服务器,并设置url
 * 参  数1: 服务端口
 * 返  回 : ESP_OK(成功)/其他失败
 *******************************************************************************/
int http_server_init(uint16_t port)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = port;
    ESP_LOGI(TAG, "服务端口:'%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        ESP_LOGI(TAG, "开启成功");
        httpd_register_uri_handler(server, &route);
        httpd_register_uri_handler(server, &postdata);
    }
    else
    {
        ESP_LOGE(TAG, "开启失败");
        return -1;
    }
    return ESP_OK;
}

/******************************************************************************
 * 函数描述: /route 回调函数
 *******************************************************************************/
esp_err_t http_route_callback(httpd_req_t *req)
{
    static const char *path = "/spiffs/html.html";
    char *buff = NULL;
    int len = fs_read(path, &buff, 1024 * 8);
    if (len > 0)
    {
        ESP_LOGI(TAG, "从%s总读取%d字节的数据", path, len);
        httpd_resp_send(req, buff, len);
        free(buff);
    }
    else
    {
        httpd_resp_send(req, "OPEN File Error", strlen("OPEN File Error"));
    }
    return ESP_OK;
}

/******************************************************************************
 * 函数描述: / 回调函数
 *******************************************************************************/
esp_err_t http_postdata_callback(httpd_req_t *req)
{
    int len = req->content_len;
    char *rec_str = malloc(len + 1);
    if (len > 0)
    {
        httpd_req_recv(req, rec_str, len);
        rec_str[len] = '\0';
        ESP_LOGI(TAG, "收到%d字节 ==> %s\r\n", len, rec_str);

        cJSON *JsonRoot = cJSON_Parse(rec_str);
        if (JsonRoot != NULL)
        {
            cJSON *ItemAdress = NULL;
            ItemAdress = cJSON_GetObjectItem(JsonRoot, "name");
            ESP_LOGI(TAG, "name = %s", ItemAdress->valuestring);
            ItemAdress = cJSON_GetObjectItem(JsonRoot, "age");
            ESP_LOGI(TAG, "age = %d", ItemAdress->valueint);
            ItemAdress = cJSON_GetObjectItem(JsonRoot, "time");
            ESP_LOGI(TAG, "time = %s", ItemAdress->valuestring);
        }
        else
        {
            ESP_LOGI(TAG, "这个字符串不是JSON格式");
        }
    }
    free(rec_str);

    cJSON *Root = cJSON_CreateObject();
    cJSON_AddStringToObject(Root, "status", "OK");
    char *json_str = cJSON_Print(Root);

    httpd_resp_send(req, json_str, strlen(json_str));

    cJSON_free((void *)json_str);
    return ESP_OK;
}
