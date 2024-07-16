#ifndef _KEYBOAED_H_
#define _KEYBOAED_H_


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"



void key_general_init(void);
int get_key_value(void);
#endif
