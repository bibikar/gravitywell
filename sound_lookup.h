#include <stdint.h>

// note_lookup contains Systick values required to produce the pitch given by the index
// as a MIDI note number.
const extern uint32_t note_lookup[];

// sine_lookup6 contains lookup values for a 6-bit DAC
const extern uint8_t sine_lookup6[];

// sine_lookup4 contains lookup values for a 4-bit DAC
const extern uint8_t sine_lookup4[];
