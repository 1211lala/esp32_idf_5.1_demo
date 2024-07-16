#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "common.h"
#include <esp_http_server.h>
#include "spiffs.h"
extern httpd_handle_t server;

int http_server_init(uint16_t port);
esp_err_t http_route_callback(httpd_req_t *req);
esp_err_t http_json_callback(httpd_req_t *req);
esp_err_t http_postdata_callback(httpd_req_t *req);
#endif
