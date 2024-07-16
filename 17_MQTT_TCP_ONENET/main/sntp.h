#ifndef _SNTP_H_
#define _SNTP_H_

#include "common.h"
#include "esp_netif_sntp.h"
#include "lwip/ip_addr.h"
#include "esp_sntp.h"
#include "esp_sleep.h"

void ntp_init(void);
void get_time(void);
#endif
