#ifndef LAB_3_OPTICAL_H
#define LAB_3_OPTICAL_H

#include "lcd.h"                                        //For Debugging
#include "Timer.h"
#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

extern volatile signed int total_x_distance;
extern volatile signed int total_y_distance;

void optical_init();
void optical_interrupt_init();
void optical_interrupt_handler();

#endif //LAB_3_OPTICAL_H
