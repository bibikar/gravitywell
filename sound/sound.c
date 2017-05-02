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

typedef struct note {
	uint8_t pitch;
	uint8_t duration;
} Note;

typedef struct song {
	const Note * const note_arr;
	uint32_t length;
	uint32_t delay;
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

const Note const test_song[] = {
	{67, 8},
	{74, 8},
	{70, 12},
	{69, 4},
	{67, 4},
	{70, 4},
	{69, 4},
	{67, 4},
	{66, 4},
	{69, 4},
	{62, 8},
	{67, 4},
	{62, 4},
	{69, 4},
	{62, 4},
	{70, 4},
	{69, 2},
	{67, 2},
	{69, 4},
	{62, 4},
	{67, 4},
	{62, 2},
	{67, 2},
	{69, 4},
	{62, 2},
	{69, 2},
	{70, 4},
	{69, 2},
	{67, 2},
	{69, 2},
	{62, 2},
	{74, 2},
	{72, 2},
	{70, 2},
	{69, 2},
	{67, 2},
	{70, 2},
	{69, 2},
	{67, 2},
	{66, 2},
	{69, 2},
	{67, 2},
	{62, 2},
	{67, 2},
	{69, 2},
	{70, 2},
	{72, 2},
	{74, 2},
	
	
};	
	
// Mendelssohn, violin concerto op. 64, third movement
const Note const victory[] = {
	{68, 2}, {71, 2}, {76, 2}, {80, 2},
	{83, 4}, {82, 2}, {85, 2},
	{83, 2}, {0, 2}, {88, 2}, {0, 2},
	{83, 2}, {0, 2}, {80, 2}, {0, 2},
	{76, 2}, {0, 2}, {78, 2}, {0, 2},
	{80, 4}, {79, 2}, {81, 2},
	{80, 2}, {0, 2}, {83, 2}, {0, 2},
	{80, 2}, {0, 2}, {76, 2}, {0, 2},
	{71, 2}, {0, 2}, {75, 2}, {73, 2},
	
	{71, 3}, {0, 1}, {78, 2}, {75, 2},
	{71, 3}, {0, 1}, {75, 2}, {73, 2},
	{71, 3}, {0, 1}, {78, 2}, {75, 2},
	{71, 3}, {0, 1}, {75, 2}, {73, 2},
	{71, 3}, {0, 1}, {71, 2}, {73, 2},
	{75, 2}, {76, 2}, {78, 2}, {80, 2},
	{81, 2}, {78, 2}, {75, 2}, {69, 2},
};

const Note const bonus_sound[] = {
	{67,2},
	{72,2}
};

const Note const collision_sound[] = {
	{60,2},
	{0,1},
	{59,2},
	{0,1},
	{67,2}
};

const Note const missile_sound[] = {
	
	
};

const Note const bomb_sound[] = {
	
};

const Song songs[] = {
	{victory, 62, 4000000},	
	{test_song, 47, 5000000},	
	{yankee, 32, 10000000},
	{harry_potter, 31, 10000000},
	{bonus_sound, 2, 10000000},
	{collision_sound, 5, 10000000},
	{missile_sound,5, 10000000},
	
};

Note *song_notes;
uint32_t song_length;
uint32_t song_flag =0;	//song_flag = 1 when a special song effect has to be played
uint32_t temp_index;
uint32_t temp_song_index, temp_song_length, temp_timer_delay;
uint32_t song_index;

//note_lookup has the systick reload values
	
//sine_lookup is the lookup table for the values of the sine wave

static int32_t current_note_index;
static int32_t current_note_duration_left;
static uint32_t song_timer_delay;
static uint8_t Index;
//Index is for the sine_lookup - to find the values for the sine wave
uint8_t get_index(void){
	return Index;
}

void set_index(uint8_t data)
{
	Index = data;
}
	
void Sound_Init(SongType index, uint8_t flag_s){
	//check the flag to see if a sound effect has to be played
	//if a sound effect has to be played then we have to save the state of the song that was playing
	if(flag_s == 1 && song_flag != 1){
		song_flag = flag_s;
		//in this case a sound effect has to be played. 
		//save the state
		temp_index = current_note_index;
		temp_song_index = song_index;
		temp_song_length = song_length;
		temp_timer_delay = song_timer_delay;
	}
	// this will initialize the timer
	// Update the song pointer and the length
	song_index = index;
	Song *next_song = (Song *) &songs[song_index];
	song_notes = (Note *) next_song->note_arr;
	song_length = (uint32_t) next_song->length;
	song_timer_delay = next_song->delay;

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
		DAC_Out(31);
	}
	TIMER1_TAILR_R = period-1;		//time0A calls Sound_Play with the reload value for Timer1A according to the note to be played
} 


void timer0A_song(void){
	//do song related stuff
	portf_toggle(2);
	if (--current_note_duration_left <= 0) {
	  // Decrement this and check if the note is over already.
	  // If so, we need to get the next note.
		if((current_note_index==(song_length-1))&&(song_flag==0))
			current_note_index=0;	//if only the song is playing, not the sound effects
		else if((current_note_index!=(song_length-1)))
			current_note_index++;
		else if((current_note_index==(song_length-1))&&(song_flag==1))
		{	//this means that the sound effect has been done, and we want to continue playing the previous song
			//restore the state
			current_note_index = temp_index;
			song_index = temp_song_index;
			song_length = temp_song_length;
			song_timer_delay = temp_timer_delay;
			song_flag = 0;	//reset the song_flag to 0 since we are no longer playing a sound effect
			Song *next_song = (Song *) &songs[song_index];
			song_notes = (Note *) next_song->note_arr;
		}
	}
  
  // Get the note to be played
  struct note current_note = song_notes[current_note_index];
  if (current_note_duration_left <= 0){
	  // If this is true, we just got a new note.
		current_note_duration_left = current_note.duration;
		// get the new duration to last the note for
		if (current_note.pitch == 0) Sound_Play(0);
		else Sound_Play(note_lookup[current_note.pitch]);
		// since we have a new note, get Sound.c to play it
  }

}

void timer1_play(){
	//this is similar to the systick handler from lab6, this will call DAC_Out
	Index = (Index+1)&0x1F;      // 4,5,6,7,7,7,6,5,4,3,2,1,1,1,2,3,...
	DAC_Out(sine_lookup6[Index]);
}

