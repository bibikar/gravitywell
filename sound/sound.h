// Sound.h
// Runs on TM4C123 or LM4F120
// Prototypes for basic functions to play sounds from the
// original Space Invaders.
// Jonathan Valvano
// November 17, 2014
#include<stdint.h>

void set_index(uint8_t);
uint8_t get_index(void);

void Sound_Init(uint8_t);
//void Sound_Play(const uint8_t *pt, uint32_t count);
void Sound_Play(uint32_t);
void timer0A_song(void);


