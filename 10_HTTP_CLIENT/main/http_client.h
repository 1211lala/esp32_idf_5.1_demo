#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include "common.h"
#include "esp_http_client.h"

void http_get_request(const char *url);
void http_post_request(const char *url);
#endif