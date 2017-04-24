// portf.c
// Contains initialization for port F and
// abstracts out on-board LEDs and buttons.

#include <stdint.h>
#include "../tm4c123gh6pm.h"

// The switch on the left side of the board (if looking at the side with buttons)
// is SW1 (which is PF4)
// The switch on the other side of the board is SW2 (which is PF0.)
// Thus, since we are using the board on the other side, the button 
// on the right is PF4, and the button on the left is PF0.

// Initialize port F.
// Also unlocks PF0.
// PF0, PF4 become inputs
// PF1-3 become outputs
void portf_init() {
	volatile unsigned long delay;
	SYSCTL_RCGCGPIO_R |= 0x20;
	delay = 42;
	delay = 43;
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY; // unlock port F
	GPIO_PORTF_CR_R |= 0x1F; // enable commit to PF0-4
	GPIO_PORTF_AMSEL_R &= ~0x1F; // disable analog functions
	GPIO_PORTF_PCTL_R &= ~0x1F; // disable this
	GPIO_PORTF_DIR_R |= 0x0E; // output on PF1-3
	GPIO_PORTF_AFSEL_R &= ~0x1F; // disable alternate function
	GPIO_PORTF_PUR_R |= 0x11; // enable PUR on PF0, PF4 (the buttons)
	GPIO_PORTF_DEN_R |= 0x1F; // digital enable port F
}

// Sets the pin to val.
void portf_set(uint8_t pin, uint8_t val) {
	if (val) GPIO_PORTF_DATA_R |= (1 << pin);
	else GPIO_PORTF_DATA_R &= ~(1 << pin);
}

// Gets the value (0 or 1) on pin.
uint8_t portf_get(uint8_t pin) {
	return !(1 & (GPIO_PORTF_DATA_R >> pin));
}

// Toggles the pin.
void portf_toggle(uint8_t pin) {
	GPIO_PORTF_DATA_R ^= (1 << pin);
}

void portf_enable_interrupts() {
	GPIO_PORTF_IS_R &= ~0x11; // PF0 and PF4 is edge-sensitive
	GPIO_PORTF_IBE_R &= ~0x11; // PF0 and PF4 are not triggered by both edges
	GPIO_PORTF_IEV_R &= ~0x11; // PF0, PF4 falling edge event
	GPIO_PORTF_ICR_R = 0x11; // clear flag0, flag4.
	GPIO_PORTF_IM_R |= 0x11; // arm interrupt on PF0, PF4.
	NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | 0x00A00000; // priority 5
	NVIC_EN0_R |= 0x40000000; // enable interrupt 30 in NVIC
}

void portf_disable_interrupts() {
	NVIC_EN0_R &= ~0x40000000; // disable interrupt 30 in NVIC
}

void gpio_portf_handler() {
	if (GPIO_PORTF_RIS_R & 0x01) {
		// add PF0 to event handler
		// acknowledge flag on PF0:
		GPIO_PORTF_ICR_R = 0x01;
	}
	if (GPIO_PORTF_RIS_R & 0x10) {
		// add PF4 to event handler
		// acknowledge flag on PF4:
		GPIO_PORTF_ICR_R = 0x10;
	}
}
