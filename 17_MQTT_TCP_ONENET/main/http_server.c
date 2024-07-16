/**
 * http请求头过长警告 Header fields are too long for server to interpret
 * 解决方法 : 修改 CONFIG_HTTPD_MAX_REQ_HDR_LEN 的值
 *
 */

#include "http_server.h"
#include "p_wifi_sta.h"
#include "cJSON.h"

httpd_handle_t server = NULL;

static const char *TAG = "http";

/* 设置URL */
static const httpd_uri_t route = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = http_route_callback};

static const httpd_uri_t getdata = {
    .uri = "/getinfo",
    .method = HTTP_GET,
    .handler = http_getdata_callback};

static const httpd_uri_t postdata = {
    .uri = "/configure",
    .method = HTTP_POST,
    .handler = http_postdata_callback};

/******************************************************************************
 * 函数描述: 初始化HTTP服务器,并设置url
 * 参  数1: 服务端口
 * 返  回 : ESP_OK(成功)/其他失败
 *******************************************************************************/
int http_server_start(uint16_t port)
{

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = port;
    ESP_LOGI(TAG, "服务端口:'%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        ESP_LOGI(TAG, "开启成功");
        httpd_register_uri_handler(server, &route);
        httpd_register_uri_handler(server, &postdata);
        httpd_register_uri_handler(server, &getdata);
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
    static const char *path = "/spiffs/index.html";
    char *buff = NULL;
    int len = fs_read(path, &buff, 1024 * 10);
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
esp_err_t http_getdata_callback(httpd_req_t *req)
{
    int len = req->content_len;
    cJSON *root = cJSON_CreateObject();    // 创建 JSON 对象作为根节点
    cJSON *networks = cJSON_CreateArray(); // 创建 JSON 数组
    httpd_resp_set_type(req, "application/json");
    // 添加每个网络的信息到数组中
    cJSON *network1 = cJSON_CreateObject();
    cJSON_AddStringToObject(network1, "ssid", wifi_list[0].ssid);
    cJSON_AddItemToArray(networks, network1);

    cJSON *network2 = cJSON_CreateObject();
    cJSON_AddStringToObject(network2, "ssid", wifi_list[1].ssid);
    cJSON_AddItemToArray(networks, network2);

    cJSON *network3 = cJSON_CreateObject();
    cJSON_AddStringToObject(network3, "ssid", wifi_list[2].ssid);
    cJSON_AddItemToArray(networks, network3);

    cJSON *network4 = cJSON_CreateObject();
    cJSON_AddStringToObject(network4, "ssid", wifi_list[3].ssid);
    cJSON_AddItemToArray(networks, network4);

    cJSON *network5 = cJSON_CreateObject();
    cJSON_AddStringToObject(network5, "ssid", wifi_list[5].ssid);
    cJSON_AddItemToArray(networks, network5);
    cJSON *network6 = cJSON_CreateObject();
    cJSON_AddStringToObject(network6, "ssid", wifi_list[6].ssid);
    cJSON_AddItemToArray(networks, network6);
    cJSON *network7 = cJSON_CreateObject();
    cJSON_AddStringToObject(network7, "ssid", wifi_list[7].ssid);
    cJSON_AddItemToArray(networks, network7);
    cJSON *network8 = cJSON_CreateObject();
    cJSON_AddStringToObject(network8, "ssid", wifi_list[8].ssid);
    cJSON_AddItemToArray(networks, network8);
    cJSON *network9 = cJSON_CreateObject();
    cJSON_AddStringToObject(network9, "ssid", wifi_list[9].ssid);
    cJSON_AddItemToArray(networks, network9);

    // 将 networks 数组添加到根对象中
    cJSON_AddItemToObject(root, "networks", networks);

    // 将 JSON 对象转换为字符串
    char *json_str = cJSON_Print(root);

    // 打印输出 JSON 字符串
    printf("%s\n", json_str);
    httpd_resp_set_type(req, "application/json; charset=UTF-8");
    httpd_resp_send(req, json_str, strlen(json_str));
    // 释放内存
    cJSON_Delete(root);
    free(json_str);

    return ESP_OK;
}
/******************************************************************************
 * 函数描述: / 回调函数
 *******************************************************************************/
esp_err_t http_postdata_callback(httpd_req_t *req)
{
    int received = 0;
    int total_len = req->content_len;
    if (total_len >= 1024 * 4)
    {
        // 数据过长，不处理
        httpd_resp_send_err(req, HTTPD_414_URI_TOO_LONG, "数据过长");
        return ESP_FAIL;
    }
    char *rBuf = malloc(1024 * 4);

    received = httpd_req_recv(req, rBuf, total_len);
    rBuf[received] = '\0';

    char ssid[50];
    char pwd[40];
    httpd_query_key_value(rBuf, "ssid", ssid, sizeof(ssid));
    httpd_query_key_value(rBuf, "password", pwd, sizeof(pwd));

    ssid[strlen(ssid)] = '\0';
    pwd[strlen(pwd)] = '\0';

    if (ssid[0] == '%')
    {
        char zh_ssid[40];
        url_decode(zh_ssid, ssid);
        printf("rBuf:%s     zh_ssid:%s     pwd:%s\r\n", rBuf, zh_ssid, pwd);
    }
    else
    {
        printf("rBuf:%s     ssid:%s     pwd:%s\r\n", rBuf, ssid, pwd);
    }

    httpd_resp_set_type(req, "text/plain; charset=utf-8");
    httpd_resp_send(req, "数据接收成功", strlen("数据接收成功"));

    free(rBuf);
    return ESP_OK;
}

/*********************************************************************************************
    while (cur_len < total_len)
    {
        // 读取数据
        received = httpd_req_recv(req, content + cur_len, total_len);
        if (received <= 0)
        {
            // 读取错误或结束
            if (received == HTTPD_SOCK_ERR_TIMEOUT)
            {
                // 超时
                httpd_resp_send_err(req, HTTPD_408_REQ_TIMEOUT, "Request timeout");
            }
            return ESP_FAIL;
        }
        cur_len += received;
    }
    content[total_len] = '\0';
 */

// URL 解码函数
void url_decode(char *dst, const char *src)
{
    char a, b;
    while (*src)
    {
        if ((*src == '%') && ((a = src[1]) && (b = src[2])) && (isxdigit(a) && isxdigit(b)))
        {
            if (a >= 'a')
                a -= 'a' - 'A';
            if (a >= 'A')
                a -= ('A' - 10);
            else
                a -= '0';
            if (b >= 'a')
                b -= 'a' - 'A';
            if (b >= 'A')
                b -= ('A' - 10);
            else
                b -= '0';
            *dst++ = 16 * a + b;
            src += 3;
        }
        else if (*src == '+')
        {
            *dst++ = ' ';
            src++;
        }
        else
        {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}