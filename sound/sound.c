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
#include "../input/portf.h"

#include "../display/ST7735.h"
#include "../display/print.h"
typedef struct note {
	uint8_t pitch;
	uint8_t duration;
} Note;

typedef struct song {
	const Note * const note_arr;
	uint32_t length;
} Song;

const Note const yankee[32] = {
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
	{62, 1},
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

const Note const harry_potter[31] = {
	{64, 3},
	{69, 3},
	{72, 3},
	{71, 3},
	{69, 4},
	{76, 3},
	{74, 5},
	{71, 5},
	{69, 3},
	{72, 3},
	{71, 3},
	{67, 4},
	{70, 3},
	{64, 7},
	{64, 3},
	{69, 3},
	{72, 3},
	{71, 3},
	{69, 4},
	{76, 3},
	{79, 4},
	{78, 3},
	{77, 4},
	{73, 3},
	{77, 3},
	{76, 3},
	{75, 3},
	{63, 4},
	{72, 3},
	{69, 7},
	{60, 3}
};
	
const Song songs[] = {
	{yankee, 32},
	{harry_potter, 31}
};

Note *song_notes;
uint32_t song_length;


//note_lookup has the systick reload values
	
//sine_lookup is the lookup table for the values of the sine wave

static int32_t current_note_index;
static int32_t current_note_duration_left;
static uint32_t song_timer_delay = 10000000;
static uint8_t Index;
//Index is for the sine_lookup - to find the values for the sine wave
uint8_t get_index(void){
	return Index;
}

void set_index(uint8_t data)
{
	Index = data;
}
	
void Sound_Init(uint8_t song_index){
// this will initialize the timer
	// Update the song pointer and the length
	Song *next_song = (Song *) &songs[song_index];
	song_notes = (Note *) next_song->note_arr;
	song_length = (uint32_t) next_song->length;

	// Reset the index of the sine lookup
	Index = 0;

	// Reset the index for which note to play
	current_note_index = 0;
	current_note_duration_left = 0;
	
	// Initialize the timers with appropriate delays
	Timer0_Init(song_timer_delay);
	Timer1_Init(10000);
}

void Sound_Play(uint32_t period){
	if (period == 0) {
		DAC_Out(0);
	}
	TIMER1_TAILR_R = period-1;		//time0A calls Sound_Play with the reload value for Timer1A according to the note to be played
} 


void timer0A_song(void){
	//do song related stuff
	portf_toggle(2);
	if (--current_note_duration_left <= 0) {
	  // Decrement this and check if the note is over already.
	  // If so, we need to get the next note.
		if(current_note_index==(song_length-1))
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
		Sound_Play(note_lookup[current_note.pitch]);
		// since we have a new note, get Sound.c to play it
  }
	portf_toggle(2);
	
}

void timer1_play(){
	//this is similar to the systick handler from lab6, this will call DAC_Out
	Index = (Index+1)&0x1F;      // 4,5,6,7,7,7,6,5,4,3,2,1,1,1,2,3,...
	DAC_Out(sine_lookup6[Index]);
}

