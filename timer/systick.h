// systick.h
// Header file for systick.c
// Functions for dealing with systick.

// Initialize SysTick with the given reload value
// If enable_interrupts is nonzero, enable interrupts.
void systick_init(uint32_t reload, uint8_t enable_interrupts);

// Stop systick.
void systick_stop(void);

// SysTick ISR
void SysTick_Handler(void);
