// systick.c
// Functions for dealing with systick

#include "../tm4c123gh6pm.h"
#include<stdint.h>

uint64_t ms;

// Initialize SysTick with the given reload value
// If enable_interrupts is nonzero, enable interrupts.
void systick_init(uint32_t reload, uint8_t enable_interrupts) {
	NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
	NVIC_ST_RELOAD_R = reload;// reload value
	NVIC_ST_CURRENT_R = 0;      // any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
	if(enable_interrupts==1)
		NVIC_ST_CTRL_R = 0x0007; // enable SysTick with core clock and interrupts  
	else
		NVIC_ST_CTRL_R = 0x0005;
	ms=0;
} 

// Stop systick.
void systick_stop() {
	NVIC_ST_CTRL_R = 0;	//disables systick
}

int systick_get()
{
	return NVIC_ST_CURRENT_R;
}

int systick_getms()
{
	return ms;
}
// SysTick ISR
void SysTick_Handler(void) {
	// TODO fill this with something
	ms++;
}
