#ifndef FINAL_H_
#define FINAL_H_

#include "lcd.h"
#include "adc.h"
#include "uart.h"
#include "ping.h"
#include <math.h>
#include "Timer.h"
#include <stdio.h>
#include "servo.h"
#include <stdint.h>
#include "optical.h"
#include "movement.h"
#include "open_interface.h"
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

typedef struct {double m[3][3];} matrix;
matrix M = {0};
char buffer[50];
double a2, a1, a0;
oi_t *sensor_data;
float small_distance = 0;
int i, small_deg = 0, b[3] = {0};
volatile char flag = 0, uart_data;
volatile signed int total_x_distance = 0, total_y_distance = 0;

void scan();
void auto_move();
void calibrate();
double det(matrix mx);
matrix M_i(int i);
void initialize();

#endif /* FINAL_H_ */
