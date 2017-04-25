// Random number generator using the middle squares method
// Algorithm from wikipedia.

#include <stdint.h>

uint64_t x = 0;
uint64_t weyl = 0;
uint64_t initial_weyl_sequence = 0xB5AD4ECEDA1CE2A9;

uint32_t srandom() {
	x *= x; // square x
	x += (weyl += initial_weyl_sequence); // Add the Weyl sequence, and move on to the next one.
	return x = (x >> 32) | (x << 32); // get the middle part
}
