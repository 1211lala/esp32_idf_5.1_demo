#ifndef _P_EXTI_H_
#define _P_EXTI_H_

#include "common.h"

#define extiPin 0
void gpio_0_isr_init(gpio_isr_t isr_handler, void *arg);

#endif