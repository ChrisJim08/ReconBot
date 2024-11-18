#include "uart.h"

void uart_init() {
    //Port B Initialization
    SYSCTL_RCGCGPIO_R  |= 0x02;             //Enable Clock
    GPIO_PORTB_AFSEL_R |= 0x03;             //Set PB0 & PB1 as Peripheral
    GPIO_PORTB_PCTL_R  |= 0x11;             //pmc0 and pmc1 for PB0 & PB1
    GPIO_PORTB_DEN_R   |= 0x03;             //Digital Enable PB0 & PB1
    GPIO_PORTB_DIR_R   &= ~0x02;            //Reset PB0 & PB1 Direction
    GPIO_PORTB_DIR_R   |=  0x01;            //Set PB0 & PB1 Direction

    //UART1 Initialization
    uint16_t ibrd = 8;
    uint16_t fbrd = 44;
    SYSCTL_RCGCUART_R  |= 0x02;             //Enable Clock
    timer_waitMillis(250);
    UART1_CTL_R &= ~0x01;                   //Disable to Configure
    UART1_IBRD_R = ibrd;                    //Load Integer Portion of BRD
    UART1_FBRD_R = fbrd;                    //Load Fractional Portion of BRD
    UART1_LCRH_R = 0x60;                    //8bit No Parity
    UART1_CC_R   = 0x00;                    //System Clock
    uart_interrupt_init();
    UART1_CTL_R |= 0x01;                    //Re-Enable UART1
}

void uart_sendChar(char data) {
   while((UART1_FR_R & 0x20) != 0);
   UART1_DR_R = data;
}

char uart_receive(void) {
    uint32_t rbi;
    char rchar;

    while ((UART1_FR_R & 0x10) != 0);       //Wait for Buffer
    rbi = UART1_DR_R;

    if (rbi & 0xF00) {                      //Error Handler
        GPIO_PORTB_DATA_R = 0xF;
    }
    else {
        rchar = (rbi & 0xFF);
    }

    return rchar;
}

void uart_sendStr(const char *data){
    while (*data) {
        uart_sendChar(*data++);
    }
}

void uart_interrupt_init() {
    //UART Interrupt Initialization
    NVIC_EN0_R  |= 0x40;                                    //Enable
    UART1_IM_R  |= 0x10;                                    //Un-Mask Interrupt
    UART1_ICR_R |= 0x10;                                    //Clear Interrupt
    IntRegister(INT_UART1, uart_interrupt_handler);         //Bind ISR Handler

}

void uart_interrupt_handler() {
    if (UART1_MIS_R & 0x10) {
        uart_data = (UART1_DR_R & 0xFF);
        UART1_ICR_R |= 0x10;
        flag = 1;
    }
}

void socket_response(char letter){
    uart_sendChar(letter);
    uart_sendChar('\n');
    uart_sendChar('\r');
}

void socket_echo(){
    char my_data = 1;       // Variable to get bytes from Client
    char command[100];  // Buffer to store command from Client
    int index = 0;      // Index position within the command buffer

    lcd_printf("Running");

    while(my_data != '\n' ) {
      my_data = uart_receive();
      command[index] = my_data;
      index++;
    }
    command[index] = 0;
    lcd_printf("Got: %s", command);

    // Send a response to the Client (Starter Client expects the response to end with \n)
    // In this case I am just sending back the first byte of the command received and a '\n'
    uart_sendChar(command[0]);
    // Only send a '\n' if the first byte of the command is not a '\n',
    // to avoid sending back-to-back '\n' to the client
    if(command[0] != '\n')
    {
      uart_sendChar('\n');
    }
}
