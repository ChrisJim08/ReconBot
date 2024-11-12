#ifndef PING_H_
#define PING_H_

#include "Timer.h"
#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

volatile unsigned int rising_time;
volatile unsigned int falling_time;
volatile enum {LOW, HIGH, DONE} state;

int ping_read();
void ping_initi();
void send_pulse();
void ping_interrupt_init();
void ping_interrupt_handler();

#endif /* PING_H_ */
