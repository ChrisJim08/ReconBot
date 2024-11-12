//The buttons are on PORTE 3:0
// GPIO_PORTE_DATA_R -- Name of the memory mapped register for GPIO Port E,
// which is connected to the push buttons
#include "button.h"
//#include "cyBot_uart.h"

// Global varibles
volatile int button_event;
volatile int button_num;

/**
 * Initialize PORTE and configure bits 0-3 to be used as inputs for the buttons.
 */
void button_init() {
    static uint8_t initialized = 0;

    if(initialized){
        return;
    }

    SYSCTL_RCGCGPIO_R |= 0b010000;
    timer_waitMillis(1);

    GPIO_PORTE_DIR_R &= 0xF0;

    GPIO_PORTE_DEN_R |= 0x0F;

    initialized = 1;
    init_button_interrupts();
}


/**
 * Initialize and configure PORTE interupts
 */
void init_button_interrupts() {
    //#warning: "Unimplemented function: void init_button_interrupts() -- You must configure GPIO to detect interrupts" // delete warning after implementing
    // In order to configure GPIO ports to detect interrupts, you will need to visit pg. 656 in the Tiva datasheet.
    // Notice that you already followed some steps in 10.3 for initialization and configuration of the GPIO ports in the function button_init().
    // Additional steps for setting up the GPIO port to detect interrupts have been outlined below.
    // TODO: Complete code below

    // 1) Mask the bits for pins 0-3
    GPIO_PORTE_IM_R &= 0b11110000;

    // 2) Set pins 0-3 to use edge sensing
    GPIO_PORTE_IS_R &= 0b11110000;

    // 3) Set pins 0-3 to use both edges. We want to update the LCD
    //    when a button is pressed, and when the button is released.
    GPIO_PORTE_IBE_R |= 0b00001111;

    // 4) Clear the interrupts
    GPIO_PORTE_ICR_R = 0b00001111; //not sure

    // 5) Unmask the bits for pins 0-3
    GPIO_PORTE_IM_R |= 0b00001111;

    //#warning: "Unimplemented function: void init_button_interrupts() -- You must configure interrupts" // delete warning after implementing
    // TODO: Complete code below
    // 6) Enable GPIO port E interrupt
    NVIC_EN0_R |= 0b00010000;

    // Bind the interrupt to the handler.
    IntRegister(INT_GPIOE, gpioe_handler);
}


/**
 * Interrupt handler -- executes when a GPIO PortE hardware event occurs (i.e., for this lab a button is pressed)
 */
void gpioe_handler() {

//#warning: "Unimplemented function: void gpioe_handler() -- You must configure interrupts" // delete warning after implementing
    // Clear interrupt status register
    GPIO_PORTE_ICR_R = 0b00001111; //not sure
    button_event = 1;
    button_num = button_getButton();
}






/**
 * Returns the position of the rightmost button being pushed.
 * @return the position of the righttmost button being pushed. 4 is the rightmost button, 1 is the leftmost button.  0 indicates no button being pressed
 */
uint8_t button_getButton() {

    if((GPIO_PORTE_DATA_R & 0x08) == 0x00) {

        //return 52;
        return 4;
    }
    else if((GPIO_PORTE_DATA_R & 0x04) == 0x00) {

        //return 51;
        return 3;
    }
    else if((GPIO_PORTE_DATA_R & 0x02) == 0x00) {

        //return 50;
        return 2;
    }
    else if ((GPIO_PORTE_DATA_R & 0x01) == 0x00) {

        //return 49;
        return 1;
    }
    else {
        return 0;
    }
}

