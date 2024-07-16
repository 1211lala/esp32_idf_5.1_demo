#ifndef _SNTP_H_
#define _SNTP_H_

#include "datafile.h"
#include "esp_netif_sntp.h"
#include "lwip/ip_addr.h"
#include "esp_sntp.h"
#include "esp_sleep.h"

void sntp_start_task(void);
void sntp_check_time(struct SNTP_DEV *sntp);
void sntp_get_time(struct SNTP_DEV *sntp);
void sntp_task(void *arg);
#endif
