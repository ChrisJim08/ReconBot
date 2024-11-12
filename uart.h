#ifndef UART_H_
#define UART_H_

#include "lcd.h"
#include "Timer.h"
#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

uint16_t ibrd = 8;
uint16_t fbrd = 44;
extern volatile char flag;
extern volatile char uart_data;

void uart_init();
char uart_receive();
void socket_echo ();
void uart_interrupt_init();
void uart_interrupt_handler();
void uart_sendChar(char data);
void socket_response(char letter);
void uart_sendStr(const char *data);

#endif /* UART_H_ */
