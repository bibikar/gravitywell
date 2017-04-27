// ADC.h
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0

// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
#include<stdint.h>
// ADC initialization function 
// Input: none
// Output: none

void ADC_Init(void);

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void);

uint32_t adc_poll();

uint32_t convert(uint32_t);
