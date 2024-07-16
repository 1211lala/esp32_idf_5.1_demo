#ifndef _P_UART_H_
#define _P_UART_H_

#include "common.h"

#include "driver/uart.h"

#define tx1Pin 25
#define rx1Pin 26

#define UART1_BUFF_SIZE 200

extern QueueHandle_t uart1_queue;

void uart1_init(uint32_t baud, uint8_t tx, uint8_t rx);

#endif