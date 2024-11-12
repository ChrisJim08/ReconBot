#include "ping.h"

void ping_initi(){
    //Port B Initialization
    SYSCTL_RCGCGPIO_R  |= 0x02;                 //Enable Clock
    GPIO_PORTB_PCTL_R  &= ~0xF000;              //Alt Function Enable PB3
    GPIO_PORTB_PCTL_R  |= 0x7000;               //T3CCP1 (Timer 3B) for PB3
    GPIO_PORTB_DEN_R   |= 0x08;                 //Digital Enable PB3

    //Timer 3 Initialization
    SYSCTL_RCGCTIMER_R |= 0x08;                 //Enable clock
    TIMER3_CTL_R       &= ~0x100;               //Disable to Configure
    TIMER3_CFG_R        = 0x04;                 //16 bits
    TIMER3_TBMR_R       = 0x07;                 //Input Edge-Time Mode (Input Capture)
    TIMER3_TBPR_R       = 0xFF;                 //Load Prescaler
    TIMER3_TBILR_R      = 0xFFFE;               //Load Starting Count Value
    TIMER3_CTL_R       |= 0xC00;                //Capture on both edges
    ping_interrupt_init();                      //Initialize Interrupt
    TIMER3_CTL_R       |= 0x100;                //Re-Enable

    //State Initialization
    state = LOW;                                //Load Low Status
}


int ping_read(){
    float time; send_pulse();                   //Send pulse

    while(state != DONE);                       //Wait until times are captured

    if(rising_time >= falling_time) {           //Calculate time
        time = (rising_time - falling_time);
    }
    else{                                       //Wrap around case
        time =  ((rising_time + 0xFFFFFE) - falling_time);
    }

    state = LOW;                                //Reset state
    return ((time*343)/(2*160000));
}


void send_pulse() {
    GPIO_PORTB_AFSEL_R      &= ~0x08;       //Set PB3 as a peripheral
    TIMER3_IMR_R            &= ~0x400;      //Mask Timer3B interrupt
    GPIO_PORTB_DIR_R        |= 0x08;        //Set PB3 to output
    GPIO_PORTB_DATA_R       |= 0x08;        //Set PB3 high
    timer_waitMicros(5);                    //Per datasheet, wait 5 microseconds
    GPIO_PORTB_DATA_R       &= ~0x08;       //Set PB3 low
    GPIO_PORTB_DIR_R        &= ~0x08;       //Set PB3 to input
    GPIO_PORTB_AFSEL_R      |= 0x08;        //Set PB3 as a peripheral

    TIMER3_ICR_R |= 0x400;                  //Clear Timer3B interrupt
    TIMER3_IMR_R |= 0x400;                  //Unmask Timer3B interrupt
}


void ping_interrupt_init() {
    //Ping Interrrupt Initiazation
    NVIC_EN1_R         |= 0x10;                         //Enable Interrupt
    TIMER3_IMR_R       |= 0x400;                        //Un-mask Interrupt
    TIMER3_ICR_R       |= 0x400;                        //Clear Interrupt
    IntRegister(INT_TIMER3B, ping_interrupt_handler);   //Bind ISR
}


void ping_interrupt_handler() {
    if (TIMER3_MIS_R & 0x400) {                                         //Check masked interrupt status
        switch (state) {
            case LOW:
                rising_time = (TIMER3_TBPS_R << 16) | TIMER3_TBR_R;     //Capture time of event's rising edge
                state = HIGH;
                break;
            case HIGH:
                falling_time = (TIMER3_TBPS_R << 16) | TIMER3_TBR_R;    //Capture time of event's falling edge
                state = DONE;
                break;
        }
    }
    TIMER3_ICR_R |= 0x400;                                              //Clear interrupt
}

