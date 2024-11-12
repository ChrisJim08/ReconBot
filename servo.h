/*
 * servo.h
 *
 *  Created on: Nov 6, 2024
 *      Author: ljwalsh
 */

#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>
#include <inc/tm4c123gh6pm.h>
#include <stdbool.h>
#include "driverlib/interrupt.h"
#include "timer.h"

//
void servo_init();

//Moves the Servo by inc degrees. If  inc is 0 moves the servo to angle
void servo_move(int inc, int angle);

void servo_cal();

#endif /* SERVO_H_ */
