#include <stdlib.h>
#include <stdint.h>

#define DEFAULT_SEED 48619737

// levelgen_init(seed)
// Give the random number generator a seed to use.
// Ideally this should be somewhat random, but it can also be fixed
// for a particular level to make the level be the same every time.
void levelgen_init(int32_t seed) {
	srand(seed);
}

// levelgen_rand(max)
// Return a random number n in 0 <= n < max.
// Remember to give the random generator a seed.
uint32_t levelgen_rand(int32_t max) {
	return rand() % max;
}
