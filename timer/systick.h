// systick.h
// Header file for systick.c
// Functions for dealing with systick.

// Initialize SysTick with the given reload value
// If enable_interrupts is nonzero, enable interrupts.
#include<stdint.h>
void systick_init(uint32_t reload, uint8_t enable_interrupts);

// Stop systick.
void systick_stop(void);
	
void systick_suspend(void);

void systick_resume(void);

// SysTick ISR
void SysTick_Handler(void);

//get current value of SysTick
int systick_get(void);

int systick_getms(void);

