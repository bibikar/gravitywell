// Sound.h
// Runs on TM4C123 or LM4F120
// Prototypes for basic functions to play sounds from the
// original Space Invaders.
// Jonathan Valvano
// November 17, 2014
#include<stdint.h>

void set_index(uint8_t);
uint8_t get_index(void);

#ifndef SONG_DEFINED
#define SONG_DEFINED

typedef enum song_type {
	SONG_VICTORY=0,
	SONG_TEST_SONG,
	SONG_YANKEE,
	SONG_HARRY_POTTER,
	SONG_BONUS_SOUND,
	SONG_COLLISION_SOUND,
	SONG_MISSILE_SOUND, 
	SONG_BOMB_SOUND,
	NUM_SONGS
} SongType;

#endif
	

void Sound_Init(SongType, uint8_t);
//void Sound_Play(const uint8_t *pt, uint32_t count);
void Sound_Play(uint32_t);
void timer0A_song(void);


