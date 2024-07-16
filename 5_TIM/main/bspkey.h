#ifndef _BSPKEY_H_
#define _BSPKEY_H_

#include "common.h"

#define keyPin 0
void key_general_init(void);
uint8_t get_key_value(uint8_t pin);

#endif

