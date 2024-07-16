#ifndef _BSPKEY_H_
#define _BSPKEY_H_

#include "common.h"

#define KEY_NUM 5
#define KEY_TRIGGER_STATUS  0
#define KEY_DEF_STATUS  1

#define key1Pin 36
#define key2Pin 35
#define key3Pin 39
#define key4Pin 34
#define keyboot 0
void key_general_init(void);
int get_key_value(void);

#endif

