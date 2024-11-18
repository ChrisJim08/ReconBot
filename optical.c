#include "optical.h"

void optical_init() {
    //Port A Initialization
    SYSCTL_RCGCGPIO_R      |=    0x01;          //Enable Clock
    timer_waitMillis(250);
    GPIO_PORTA_AFSEL_R     |=    0x14;          //Alt Function Enable PA2 & PA4
    GPIO_PORTA_PCTL_R      |=    0x20200;       //SSI0 Clk/Rx Alt Function for PA2/PA4
    GPIO_PORTA_DEN_R       |=    0x14;          //Digital Enable PA4 & PA2
    GPIO_PORTA_PUR_R       |=    0x04;          //Pull-Up Enable for PA2

    //SSI0 Initialization
    SYSCTL_RCGCSSI_R       |=    0x01;          //Enable Clock
    timer_waitMillis(250);
    SSI0_CR1_R             &=   ~0x02;          //Disable to Configure
    SSI0_CR1_R              =    0x0C;          //Slave Mode (Output disabled)
    SSI0_CC_R               =    0x05;          //PIOSC Clock Source (16 MHz)
    SSI0_CPSR_R             =    0x02;          //Load Prescale divisor (even 02-FE)
    SSI0_CR0_R              =    0x194;        //SCR = 25 | 1st Clk Edge, Active High, Freescale SPI | 16-bit data size
    optical_interrupt_init();                   //Initialize Interrupt
    SSI0_CR1_R             |=    0x02;          //Re-Enable
}

void optical_interrupt_init() {
    //SSI0 Interrupt Initialization
    NVIC_EN0_R             |= 0x80;                         //Enable Interrupt
    SSI0_IM_R              |= 0x04;                         //Un-Mask Interrupt
    SSI0_ICR_R             |= 0x03;                         //Clear Interrupt

    IntRegister(INT_SSI0, optical_interrupt_handler);       //Bind ISR
}

void optical_interrupt_handler() {
    lcd_printf("blah");
    int16_t motion_status   = SSI0_DR_R & 0xFF;             //Read Motion Status Data
    int8_t delta_x          = SSI0_DR_R & 0xFF;             //Read Motion Status Data
    int8_t delta_y          = SSI0_DR_R & 0xFF;             //Read Motion Status Data

    total_x_distance        += delta_x/100;                 //Update X Distance
    total_y_distance        += delta_y/100;                 //Update Y Distance

    SSI0_ICR_R             |= 0x03;                         //Clear Interrupt
}
