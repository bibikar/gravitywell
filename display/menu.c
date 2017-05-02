#include<stdint.h>
#include "ST7735.h"
#include "drawing.h"
#include "print.h"
#include "../input/portf.h"

// We have done some #defines in the menu.h file.
#include "menu.h"
#include "../game/game.h"

#define LEFT_BUTTON 0x01
#define RIGHT_BUTTON 0x02

#define HELP_COLOR_MULTIPLIER_R 24
#define HELP_COLOR_MULTIPLIER_G 12
#define HELP_COLOR_MULTIPLIER_B 20


#define MAIN_MENU_SELECTION_ROW_COUNT 3
static const uint8_t main_menu_selection_rows[3] = {8,9,10};

// Help screens
#define HELP_SCREEN_COUNT 6
static char help_strings[][200] = {
	// Page 1:
	"Welcome, Captain,\nto the USS Newton!\n\nYou are on a research\nmission in Saturn's\nrings. It is your\n"
	"job to collect the\ngreen crystals while\navoiding the field\nof dangerous\nasteroids.",
	// Page 2:
	"Navigation:\n\nUse the slide pot\nto control your ship\nthrusters. While in\ncenter, the ship's\n"
	"velocity is constant.\nMoving it to either\nside will cause the\nship to accelerate\nleft or right.",
	// Page 3:
	"Due to the ship's\ndesign, you cannot\ncontrol the forward\nmotion of the ship.\nThus, you will have\n"
	"to plan ahead as\nthe ship cannot\nslow down in the\nforward direction!",
	// Page 4:
	"Crystals:\n\nThroughout the field,\nyou will see green\nspheres. These are\ncrystals. We don't\n"
	"know what all of\nthem do, but some\nwill help you on\nyour mission.",
	// Page 5:
	"Missiles:\n\nYour ship can carry\n255 missiles. You\nstart off with 3, but\nsome crystals will\ngrant you"
	" more.\nMissiles are shown in\nthe top left corner\nof the screen. The\nleft button fires\none or two.",
	// Page 6:
	"Bombs:\n\nYour ship can carry\none bomb. You can\nonly obtain one from\ncrystals. It appears\nas a diamond in the\n"
	"top right corner of\nthe screen. Use it to\nerase all visible\nasteroids with the\nright button."
};

// Get the status of the buttons
// PF4 is the button on the right (hence 0x02 in #defines)
// PF0 is on the left
uint8_t menu_get_button_status() {
	return portf_get(0) | (portf_get(4)<<1);
	// It's a bit less efficient getting them separately, but 
	// it's worth it to get it abstracted out.
}

uint8_t show_main_menu_selector(const uint8_t rows[], const uint8_t row_count) {
	uint8_t current_row = 0;
	while (menu_get_button_status() != RIGHT_BUTTON) {
		ST7735_SetCursor(0, rows[current_row]);
		ST7735_OutChar('*');
		//ST7735_SetCursor(20, rows[current_row]);
		//ST7735_OutChar('*');
		if (menu_get_button_status() == LEFT_BUTTON) { // if the scroll button is pressed
			ST7735_SetCursor(0, rows[current_row]);
			ST7735_OutCharErase('*');
			//ST7735_SetCursor(20, rows[current_row]);
			//ST7735_OutChar(' ');
			if ((current_row+1) < row_count) current_row++;
			else current_row = 0;
			while(menu_get_button_status() == LEFT_BUTTON) {} // until the scroll button is released
		}
	}
	return current_row; // We return the row which was selected (e.g. 0 is play, 1 is help, 2 exit)
}



// Shows the menu given by the id (see define statements)
// Returns a status code (0 = default success)
// Any other status code given becomes the new menu id.
// A status code above 127 means to go to the actual game.
// The data to pass is uneccessary except for the following cases:
// MENU_GAME_OVER, MENU_YOU_WIN: the score to display
// MENU_NEXT_LEVEL: the next level to go to
uint8_t show_menu(GameStatus status) {
	uint8_t id = status.status;
	uint32_t data = status.data;
	uint8_t help_screen_index = 0;
	char *this_help = 0;
	switch(id) {
		case MENU_MAIN:
			while (menu_get_button_status() != 0) {}
			ST7735_FillScreen(0);
			ST7735_SetCursor(4, 3);
			ST7735_OutString("GRAVITY WELL");
			ST7735_SetCursor(6, 5);
			ST7735_OutString("Main Menu");

			ST7735_SetCursor(8, 8);
			ST7735_OutString("Play");
			ST7735_SetCursor(8, 9);
			ST7735_OutString("Help");
			ST7735_SetCursor(8, 10);
			ST7735_OutString("Info");
			ST7735_SetCursor(13,14);
			ST7735_OutString("Select");
			ST7735_SetCursor(2,14);
			ST7735_OutString("Next");

			uint8_t main_menu_selection = show_main_menu_selector(main_menu_selection_rows, MAIN_MENU_SELECTION_ROW_COUNT);

			switch(main_menu_selection) {
				case 0: return MENU_NEW_GAME;
				case 1: return MENU_HELP;
				case 2: return MENU_INFO;
				default: return MENU_ERROR;
			}
		case MENU_HELP:
			
			do {
				while (menu_get_button_status() != 0) {}
				// Fill the screen with a blank color, but let's make it somewhat more interesting than just a color like that
				ST7735_FillScreen(ST7735_Color565(help_screen_index*HELP_COLOR_MULTIPLIER_R, help_screen_index*HELP_COLOR_MULTIPLIER_G, help_screen_index*HELP_COLOR_MULTIPLIER_B));
				// Tell the user we're in the help menu
				ST7735_SetCursor(0,0);
				ST7735_OutString("Help (");
				LCD_OutDec(help_screen_index+1);
				ST7735_OutChar('/');
				LCD_OutDec(HELP_SCREEN_COUNT);
				ST7735_OutChar(')');
				// Move down to print the help string itself.
				// Hopefully the newlines will actually work.
				// Otherwise we will need to write a function to
				// handle the printing of string with newlines
				ST7735_SetCursor(0,2);
				this_help = help_strings[help_screen_index];
				ST7735_OutString(this_help);
				ST7735_SetCursor(15,14);
				if(help_screen_index < HELP_SCREEN_COUNT - 1) ST7735_OutString("Next");
				else ST7735_OutString("Exit");
				if (help_screen_index != 0) {
					ST7735_SetCursor(2,14);
					ST7735_OutString("Prev");
				}
				// Make sure the button is released before continuing
				while (menu_get_button_status() == 0) {}
				// Make sure some button is pressed before continuing.
				//while (menu_get_button_status() == 0) {}
				if (menu_get_button_status() == RIGHT_BUTTON) {
					help_screen_index++;
					// if we already went through all the help screens, then go back to the main menu.
					if (help_screen_index == HELP_SCREEN_COUNT) return MENU_MAIN; 
				} 
				else if (menu_get_button_status() == LEFT_BUTTON) {
					if(help_screen_index>0)
						help_screen_index--;
				}
				while (menu_get_button_status() != 0) {}	//wait for release
			}while (help_screen_index < HELP_SCREEN_COUNT);
			while (menu_get_button_status() != 0) {}
			return MENU_MAIN;
		case MENU_INFO:
			while (menu_get_button_status() != 0) {}
			ST7735_FillScreen(ST7735_Color565(0, 32, 0));
			ST7735_SetCursor(0,0);
			ST7735_OutString("GRAVITY WELL\n");
			ST7735_OutString("EE 319K, Spring 2017\n\n");
			ST7735_OutString("Authors:\n");
			ST7735_OutString("Sameer Bibikar,\n");
			ST7735_OutString("Sneha Pendharkar.\n\n");
			ST7735_OutString("bibikar.github.io");
			while (menu_get_button_status() == 0) {}
			while (menu_get_button_status() != 0) {}
			break;


		case MENU_GAME_OVER:
		case MENU_YOU_WIN:
			// Data should have the score!!
			ST7735_FillScreen(ST7735_Color565(0,0,0));
			ST7735_SetCursor(3,9);
			if (id == MENU_GAME_OVER) ST7735_OutString("GAME OVER");
			else if (id == MENU_YOU_WIN) ST7735_OutString(" YOU WIN ");
			ST7735_SetCursor(0,10);
			ST7735_OutString("Score: ");
			LCD_OutDec(data);
			ST7735_SetCursor(0,14);
			ST7735_OutString("   Press any key\n   to return...");
			// Wait until some button is pressed.
			while (menu_get_button_status() == 0) {}
			return MENU_MAIN;
		case MENU_NEXT_LEVEL:
			ST7735_FillScreen(ST7735_Color565(0,20,0));
			ST7735_SetCursor(0,6);
			ST7735_OutString("Field ");
			LCD_OutDec(data-1);
			ST7735_OutString(" cleared");
			ST7735_SetCursor(0,14);
			ST7735_OutString("   Press any key\n   to continue...");
			// Wait until some button is pressed.
			while (menu_get_button_status() == 0) {}
			return GAME_BEGIN + data; // tell the main program we want to start the game with level specified by data.
		case MENU_ERROR:
		default:
			// In fact, we probably don't even need this since the code should be bug-free.
			// But this is the default and will happen if we don't have a particular menu programmed.
			//while(menu_get_button_status!=0){}	//wait for button to be released
			ST7735_FillScreen(ST7735_Color565(255, 0, 0)); // color the screen red
			ST7735_SetCursor(7,0);
			ST7735_OutString("ERROR!\n");
			ST7735_OutString("Most likely, you\nentered a menu which\ndoesn't exist.");
			ST7735_SetCursor(0,10);
			ST7735_OutString("id=");
			LCD_OutDec(id);
			ST7735_SetCursor(0,11);
			ST7735_OutString("data=");
			LCD_OutDec(data);
			ST7735_SetCursor(0,12);
			ST7735_OutString("[Attempting to exit\n  to main menu...]");

			uint16_t color = 0;
			uint16_t count = 0;
			// Cycle through all colors 2 times on this random pixel.
			while (count < 2) {
				ST7735_DrawPixel(id,data,color++);
				if (color == 0) count++;
			}
			return MENU_MAIN; // let's try to get back to the main menu.
	}
}

