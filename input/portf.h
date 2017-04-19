// portf.h
// Header file for portf.c
#include <stdint.h>

// Initialize Port F.
// Also unlock PF0
void portf_init(void);

// Set pin pin to value val (1 or 0)
void portf_set(uint8_t, uint8_t);

// Get value (0 or 1) from pin pin.
uint8_t portf_get(uint8_t);

// Toggle pin pin.
void portf_toggle(uint8_t);
