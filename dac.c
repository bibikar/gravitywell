// dac.c
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// Program written by: Sneha Pendharkar, Sameer Bibikar
// Date Created: 3/6/17 
// Last Modified: 3/6/17 
// Lab number: 6
// Hardware connections
// PB0-3 are outputs, PE0-2 are inputs, PF0 and PF4 are inputs

#include <stdint.h>
#include "tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	// PB0-5 are outputs
	volatile uint32_t delay;
	SYSCTL_RCGC2_R |= 0x02;	//turn on clock for Port B
	delay = 42;	//time to allow the clock to stabilise
	GPIO_PORTB_AMSEL_R &= ~0x3F;	//turn off analog function
	GPIO_PORTB_DIR_R |= 0x3F;
	GPIO_PORTB_AFSEL_R &= ~0x3F;	//turn off alternate function
	GPIO_PORTB_DEN_R |= 0x3F;	//enable digital I/O
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(uint32_t data){
	GPIO_PORTB_DATA_R = data;	//output to dac
}
