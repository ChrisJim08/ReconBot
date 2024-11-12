#include "uart.h"

void uart_init() {
    SYSCTL_RCGCGPIO_R  |= 0x02;                // enables clock GPIOB (page 340)
    SYSCTL_RCGCUART_R  |= 0x02;                // enables clock UART1 (page 344)
    GPIO_PORTB_AFSEL_R |= 0x03;                // sets PB0 and PB1 as peripherals (page 671)
    GPIO_PORTB_PCTL_R  |= 0x11;                // pmc0 and pmc1       (page 688)  also refer to page 650
    GPIO_PORTB_DEN_R   |= 0x03;                // enables pb0 and pb1
    GPIO_PORTB_DIR_R   &= ~0x02;
    GPIO_PORTB_DIR_R   |=  0x01;

    //compute baud values [UART clock= 16 MHz] 
    uint16_t    fbrd = 44;                  // page 903
    uint16_t    ibrd = 8;

    UART1_CTL_R &= ~0x01;                   // disable UART1 (page 918)

    UART1_IBRD_R = ibrd;                    // write integer portion of BRD to IBRD
    UART1_FBRD_R = fbrd;                    // write fractional portion of BRD to FBRD

    UART1_LCRH_R = 0x60;                    // write serial communication parameters (page 916) * 8bit and no parity

    UART1_CC_R   = 0x00;                    // use system clock as clock source (page 939)

    UART1_CTL_R |= 0x01;                    // enable UART1

    uart_interrupt_init();
}

void uart_sendChar(char data) {
   while((UART1_FR_R & 0x20) != 0);
   UART1_DR_R = data;
}

char uart_receive(void) {
    uint32_t rbi;
    char rchar;

    while ((UART1_FR_R & 0x10) != 0);

    rbi = UART1_DR_R;

    if (rbi & 0xF00) {
        GPIO_PORTB_DATA_R = 0xF;
    }
    else {
        rchar = (char)(rbi & 0xFF);
    }

    return rchar;

}

void uart_sendStr(const char *data){
    while (*data != '\0') {
        uart_sendChar(*data);
        data++;
    }
}

void uart_interrupt_init() {
    // Enable interrupts for receiving bytes through UART1
    UART1_IM_R  |= 0x10; //enable interrupt on receive - page 924

    // Find the NVIC enable register and bit responsible for UART1 in table 2-9
    // Note: NVIC register descriptions are found in chapter 3.4
    NVIC_EN0_R  |= 0x40; //enable uart1 interrupts - page 104

    UART1_ICR_R |= 0x10;
    flag = 0;
    // Find the vector number of UART1 in table 2-9 ! UART1 is 22 from vector number page 104
    IntRegister(INT_UART1, uart_interrupt_handler); //give the microcontroller the address of our interrupt handler - page 104 22 is the vector number

}

void uart_interrupt_handler() {
        if (UART1_MIS_R & 0x10) {                               // STEP1: Check the Masked Interrupt Status
            uart_data = (char)(UART1_DR_R & 0xFF);              //STEP2:  Copy the data
            UART1_ICR_R |= 0x10;                                //STEP3:  Clear the interrupt
            flag = 1;
        }
}
