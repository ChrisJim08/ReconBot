/*
 * servo.c
 *
 *  Created on: Nov 6, 2024
 *      Author: ljwalsh
 */
#include "button.h"
#include "servo.h"
#include "lcd.h"
#include <stdio.h>

//Theoretical:
//304000
//288000
//Bot 02:
//313240
//285168
//Bot 11:
//312620
//284410
#define zero_degrees 312620
#define left_degrees 284410
int delta_degrees = (zero_degrees - left_degrees) / 180;
extern volatile int button_event;
extern volatile int button_num;
extern volatile short servo_dir = 1;
extern volatile int curr_val = zero_degrees;

void servo_init(){
    //timer1b
    //GPIO port B pin 5
    //no interrupts
    //count down required
    //20ms cycle * 16MHz = 320000 tics --> 0x04E200

    //System Clock
    SYSCTL_RCGCGPIO_R   |=  0b000010;   // Enable port B clock
    SYSCTL_RCGCTIMER_R  |=  0b000010;   // Enable timer 1 clock
    timer_waitMillis(1);                // Small delay before accessing device after turning on clock

    //GPIO
    GPIO_PORTB_DEN_R    |= 0b00100000;  // Set PB5 to Digital
    GPIO_PORTB_DIR_R    |= 0b00100000;  // Set PB5 to Output
    GPIO_PORTB_AFSEL_R  |= 0b00100000;  // Set PB5 to Alternative Function
    GPIO_PORTB_PCTL_R   &= 0xFF0FFFFF;  // Force 0's in for PB5 Peripheral
    GPIO_PORTB_PCTL_R   |= 0x00700000;  // Set PB5 Peripheral to 7 or T1CCP1

    //#warning: ping_init() may also disable timer1B not sure whether thats good or bad
    //Timers per data sheet
    TIMER1_CTL_R        &= ~(0x0100);   //Disable timer1 B
    TIMER1_CFG_R        =  0x00000004;  // (use equal bc datasheet says so)
    TIMER1_TBMR_R       = 0b01010;      //set Timer1B to PWM mode (use equal bc there is only one timer1b)
    TIMER1_TBPR_R       = 0x04;         // PRESCALE + LOAD = 0x04E200 (see calculation above)
    TIMER1_TBILR_R      = 0xE200;       // PRESCALE + LOAD = 0x04E200 (see calculation above)

    TIMER1_CTL_R        = 0x0100;      //Enable timer1 B
}

void servo_move(int inc, int angle){
    char buff[40];
    if (inc){
        curr_val = (TIMER1_TBPMR_R << 16) | TIMER1_TBMATCHR_R;
        curr_val -= inc * delta_degrees;
        sprintf(buff, "Current Val: %d\nCurrent dir: %d", curr_val, servo_dir);
        lcd_printf(buff);
    }else{
        curr_val = zero_degrees - angle * delta_degrees ;
        sprintf(buff, "Current Val: %d\nCurrent dir: %d", curr_val, servo_dir);
        lcd_printf(buff);
    }
    TIMER1_CTL_R        &= ~(0x0100);   //Disable timer1 B
    TIMER1_TBMATCHR_R   = curr_val & 0x0000FFFF;               //set lower 2 bytes of angle
    TIMER1_TBPMR_R      = (curr_val & 0x00FF0000) >> 16;       //set upper byte of angle
    TIMER1_CTL_R        = 0x0100;      //Enable timer1 B
}

void servo_cal (){
    char buff[40];
    while (1){
       while(!button_event);

       switch (button_num) {
           case 1:
               servo_move(1*servo_dir, 0);
               break;
           case 2:
               button_event = 0;
               while(!button_event)
               servo_move(1*servo_dir, 0);
               break;
           case 3:
               servo_dir *= -1;
               break;
           case 4:
               button_event = 0;
               while(!button_event) {
                   if (servo_dir == 1){
                       servo_move(0, 180);
                   }
                   else{
                       servo_move(0, 0);
                   }
               }
               break;
       }

       button_event = 0;
       sprintf(buff, "Current Val: %d\nCurrent dir: %d", curr_val, servo_dir);
       lcd_printf(buff);
       timer_waitMillis(300);
   }
}

