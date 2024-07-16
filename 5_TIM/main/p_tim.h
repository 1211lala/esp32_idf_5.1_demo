#ifndef _P_TIM_H_
#define _P_TIM_H_

#include "common.h"

#include "driver/timer.h"
#include "soc/rtc.h"

extern QueueHandle_t tim_queue;

void tim0_init(timer_isr_t timer_callback, void *arg, uint32_t value);



#endif