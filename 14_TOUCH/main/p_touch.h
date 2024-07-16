#ifndef _P_TOUCH_H_
#define _P_TOUCH_H_

#include "common.h"
#include "driver/touch_pad.h"
#include "soc/rtc_periph.h"
#include "soc/sens_periph.h"
#include "driver/touch_sensor.h"
void touch_init(touch_pad_t  touchNum);
void touch_init_isr(touch_pad_t touchNum, intr_handler_t touch_callback);

#endif