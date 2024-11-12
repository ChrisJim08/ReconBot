#include "timer.h"
#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

#ifndef ADC_H_
#define ADC_H_

void adc_init();
int adc_read();

#endif /* ADC_H_ */
