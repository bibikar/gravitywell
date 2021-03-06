// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 4/10/2016
// Student names: Sneha Pendharkar, Sameer Bibikar
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "../tm4c123gh6pm.h"
#include "../display/ST7735.h"
#include "../timer/systick.h"


// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void){
  volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x10;
  while((SYSCTL_PRGPIO_R&0x10) == 0){};
  GPIO_PORTE_DIR_R &= ~0x10; 
  GPIO_PORTE_AFSEL_R |= 0x10; 
  GPIO_PORTE_DEN_R &= ~0x10;   
  GPIO_PORTE_AMSEL_R |= 0x10;   
  SYSCTL_RCGCADC_R |= 0x01;      
  delay = GPIO_PORTE_DATA_R;
  delay = GPIO_PORTF_DATA_R;
  delay = GPIO_PORTE_DATA_R;
  delay = GPIO_PORTF_DATA_R;
  ADC0_PC_R = 0x01;               
  ADC0_SSPRI_R = 0x0123;         
  ADC0_ACTSS_R &= ~0x0008;      
  ADC0_EMUX_R &= ~0xF000;      
  ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+9; 
  ADC0_SSCTL3_R = 0x0006;     
  ADC0_IM_R &= ~0x0008;      
  ADC0_ACTSS_R |= 0x0008;   
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void){  
  unsigned long result;
  ADC0_PSSI_R = 0x0008;            // 1) initiate SS3
  while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion done
  result = ADC0_SSFIFO3_R&0xFFF;   // 3) read result
  ADC0_ISC_R = 0x0008;             // 4) acknowledge completion
  return result;
}

int32_t convert(uint32_t input){
	return 2048 - input;	//replace with equation for calibration
	// Input can be from 0 - 4095, so we want to get the most
	// granularity out of the input as we possibly can. All the
	// "conversion" can be done elsewhere.
}

int32_t adc_poll(){
	uint32_t data, result;
	data = ADC_In();	//get 12 bit number - between 0 to 4096
	result = convert(data);	//get the actual force
	return result;	
}



