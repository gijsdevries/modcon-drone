#ifndef UART_H 
#define UART_H

#include "driver/gpio.h"
#include "stdio.h"
#include "driver/uart.h"
#include "string.h"

static const int RX_BUF_SIZE = 1024;
#define TXD_PIN (gpio_num_t)17

void uart_init(void);
int sendData(const char* data);

#endif
