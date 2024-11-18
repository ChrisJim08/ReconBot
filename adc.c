#include "adc.h"

void adc_init() {
    //Port B Initialization
    SYSCTL_RCGCGPIO_R |= 0x02;                  //Enable clock
    GPIO_PORTB_AFSEL_R   |= 0x10;               //Alt Function Enable PB4
    GPIO_PORTB_PCTL_R    &= ~0xF0000;           //AIN10 (Analog-in 10) for PB4
    GPIO_PORTB_DEN_R     &= ~0x10;              //Digital Disable PB4
    GPIO_PORTB_AMSEL_R   |= 0x10;               //Analog Enable PB4
    GPIO_PORTB_ADCCTL_R   = 0x00;               //Default Trigger Source

    //ADC Initialization
    SYSCTL_RCGCADC_R  |= 0x01;                  //Enable clock for ADC
    timer_waitMillis(250);
    ADC0_ACTSS_R   &= ~0x01;                    //Disable to Configure
    ADC0_EMUX_R    &= ~0x0F;                    //Default trigger source
    ADC0_SSMUX1_R   = 0x0A;                     //all samples in sequence for AIN10
    ADC0_SSCTL1_R   = 0x06;                     //Interrupt, End, and 0x06???
    ADC0_IM_R      &= ~0x02;                    //Disable SS1 interrupt
    ADC0_SAC_R      = 0x04;                     //16x
    ADC0_ACTSS_R   |= 0x02;                     //Re-Enable
}


int adc_read() {
    ADC0_PSSI_R |= 0x02;                        //Initiate ADC0
    while(!(ADC0_RIS_R & 0x02));                //Wait for an interrupt
    int adc_data = ADC0_SSFIFO1_R & 0xFFF;      //Retrieve ADC0 DATA
    ADC0_ISC_R |= 0b0010;                       //Clear interrupt
    return adc_data;                            //Return data
}


