// main.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 3/6/2015 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// *******Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// 6 bit DAC:
// 32*750 = 24k resistor DAC bit 0 on PB0 (least significant bit)
// 16*750 = 12k resistor DAC bit 1 on PB1
// 8*750 = 6k resistor DAC bit 2 on PB2
// 4*750 = 3k resistor DAC bit 3 on PB3
// 2*750 = 1.5k resistor DAC bit 4 on PB4
// 1*750 = 750 resistor DAC bit 5 on PB5
// fire missile button connected to PF0
// fire bomb button connected to PF4
// PF1-3 are outputs (heartbeats)

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "display/ST7735.h"
#include "math/random.h"
#include "TExaS.h"
#include "input/ADC.h"
#include "display/menu.h"
#include "input/portf.h"
#include "game/game.h"
#include "sound/dac.h"
#include "sound/sound.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds

// *************************** Capture image dimensions out of BMP**********

int main(void){
	TExaS_Init();  // set system clock to 80 MHz
	//Random_Init(1);

	Output_Init();
	ST7735_FillScreen(0x0000);            // set screen to black
	portf_init();
	DAC_Init();
	
	Sound_Init(SONG_TEST_SONG,0);
	GameStatus status;
	status.status = MENU_MAIN;
	while (1) {
		if (status.status == MENU_NEW_GAME) {
			status = game_test(1);
		}
		while (status.status >= 128) {
			status = game_test(status.status - 128);
		}
		Sound_Init(SONG_TEST_SONG,0);
		status.status = show_menu(status);	//second number is the score		
	}

}


// You can use this timer only if you learn how it works

void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
