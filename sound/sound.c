// Sound.c
// Jonathan Valvano
// November 17, 2014

// Adapted for use with song sequences
// (EE 319K Piano DAC lab)
// Sameer Bibikar, Sneha Pendharkar, 2017 spring
#include <stdint.h>
#include "../sound/sound.h"
#include "dac.h"
#include "../timer/Timer1.h"
#include "../tm4c123gh6pm.h"
#include "../timer/Timer0.h"
#include "../sound/sound_lookup.h"

struct note {
	uint8_t pitch;
	uint8_t duration;
};
const struct note song_notes[32] = {
	{60, 1},
	{0, 1},
	{60, 2},
	{62, 2},
	{64, 2},
	{60, 2},
	{64, 2},
	{62, 2},
	{55, 2},
	{60, 1},
	{0, 1},
	{60, 2},
	{62, 2},
	{64, 2},
	{60, 4},
	{59, 4},
	{60, 1},
	{0, 1},
	{60, 2},
	{62, 2},
	{64, 2},
	{65, 2},
	{64, 2},
	{62, 2},
	{60, 2},
	{59, 2},
	{55, 2},
	{57, 2},
	{59, 2},
	{60, 3},
	{0, 1},
	{60, 3}
};



//note_lookup has the systick reload values
	
//sine_lookup is the lookup table for the values of the sine wave

uint32_t song_timer_delay = 10000000;
uint8_t Index;
//Index is for the sine_lookup - to find the values for the sine wave
uint8_t get_index(void){
	return Index;
}

void set_index(uint8_t data)
{
	Index = data;
}
	
void Sound_Init(void){
// this will initialize the timer
	DAC_Init();
	Index = 0;
	Timer0_Init(song_timer_delay);
	Timer1_Init(10000);
}

void Sound_Play(uint32_t period){
	TIMER1_TAILR_R = period-1;		//time0A calls Sound_Play with the reload value for Timer1A according to the note to be played
} 

int32_t current_note_index=0;
int32_t current_note_duration_left=0;
uint32_t song_length=32;

void timer0A_song(void){
	//do song related stuff
	if (--current_note_duration_left <= 0) {
	  // Decrement this and check if the note is over already.
	  // If so, we need to get the next note.
		if(current_note_index==song_length)
			current_note_index=0;
		else
			current_note_index++;
	 }
  
  // Get the note to be played
  struct note current_note = song_notes[current_note_index];
  if (current_note_duration_left <= 0){
	  // If this is true, we just got a new note.
		current_note_duration_left = current_note.duration;
		// get the new duration to last the note for
		Sound_Play(current_note.pitch);
		// since we have a new note, get Sound.c to play it
  }
	
}

void timer1_play(){
	//this is similar to the systick handler from lab6, this will call DAC_Out
	uint8_t index;
	index = get_index();
	index = (index+1)&0x1F;      // 4,5,6,7,7,7,6,5,4,3,2,1,1,1,2,3,...
	set_index(index);
	DAC_Out(sine_lookup6[index]);
}

