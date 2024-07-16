#ifndef _P_GPTTIM_H_
#define _P_GPTTIM_H_

#include "common.h"

#include "driver/gptimer.h"

extern QueueHandle_t tim_queue;
extern gptimer_handle_t gptimer;

void gpttimer_init(gptimer_alarm_cb_t gpttim_isr_callback, uint32_t freq, uint32_t alarmValue);

#endif