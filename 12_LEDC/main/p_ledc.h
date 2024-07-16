#ifndef _P_LEDC_H_
#define _P_UART_H_

#include "common.h"
#include "driver/ledc.h"

#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_HIGH_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_GPIO GPIO_NUM_2

void ledc_led_init(void);

#endif