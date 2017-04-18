#include "ST7735.h"
#include "drawing.h"

#define MENU_MAIN 0
#define MENU_NEW_GAME 1
#define MENU_HELP 2
#define MENU_ERROR 126
#define MENU_EXIT 127

#define LEFT_BUTTON 0x01
#define RIGHT_BUTTON 0x02

#define HELP_COLOR_MULTIPLIER_R 4
#define HELP_COLOR_MULTIPLIER_G 8
#define HELP_COLOR_MULTIPLIER_B 12


#define MAIN_MENU_SELECTION_ROW_COUNT 3
static const uint8_t main_menu_selection_rows[3] = {8,9,10};

// Help screens
#define HELP_SCREEN_COUNT 3
static const char help_strings[][] = {
	"This is page 1\nof the help file",
	"This is page 2\nof the help file!",
	"This is the last\nhelp page, I think."
};

uint8_t menu_get_button_status() {
	// TODO add code to actually give the status of the buttons (which ones are pressed)
	// use the #defines above.
	return 0;
}

void menu_print_integer(uint8_t n) {
	// TODO print an integer n to the screen.
}

// Shows the menu given by the id (see define statements)
// Returns a status code (0 = default success)
// Any other status code given becomes the new menu id.
// A status code above 127 means to go to the actual game.
uint8_t show_menu(uint32_t id) {
	switch(id) {
		case MENU_MAIN:
			ST7735_SetCursor(4, 3);
			ST7735_OutString("GRAVITY WELL");
			ST7735_SetCursor(6, 5);
			ST7735_OutString("Main Menu");

			ST7735_SetCursor(8, 8);
			ST7735_OutString("Play");
			ST7735_SetCursor(9, 8);
			ST7735_OutString("Help");
			ST7735_SetCursor(10, 8);
			ST7735_OutString("Exit");

			uint8_t main_menu_selection = show_main_menu_selector(main_menu_selection_rows, MAIN_MENU_SELECTION_ROW_COUNT);

			switch(main_menu_selection) {
				case 0: return MENU_NEW_GAME;
				case 1: return MENU_HELP;
				case 2: return MENU_EXIT;
				default: return MENU_ERROR;
			}
		case MENU_HELP:
			uint8_t help_screen_index = 0;
			do {
				// Fill the screen with a blank color, but let's make it somewhat more interesting than just a color like that
				ST7735_FillScreen(ST7735_Color565(help_screen_index*HELP_COLOR_MULTIPLIER_R, help_screen_index*HELP_COLOR_MULTIPLIER_G, help_screen_index*HELP_COLOR_MULTIPLIER_B));
				// Tell the user we're in the help menu
				ST7735_OutString("Help [");
				menu_print_integer(help_screen_index+1);
				ST7735_OutChar('/');
				menu_print_integer(HELP_SCREEN_COUNT);
				ST7735_OutChar(']');
				// Move down to print the help string itself.
				// Hopefully the newlines will actually work.
				// Otherwise we will need to write a function to
				// handle the printing of string with newlines
				ST7735_SetCursor(0,2);
				ST7735_OutString(help_strings[help_screen_index]);
				// Make sure the button is released before continuing
				while (menu_get_button_status() != 0) {}
				// Make sure some button is pressed before continuing.
				while (menu_get_button_status() == 0) {}
				if (menu_get_button_status() == RIGHT_BUTTON) {
					help_screen_index++;
					// if we already went through all the help screens, then go back to the main menu.
					if (help_screen_index == HELP_SCREEN_COUNT) return MENU_MAIN; 
				} else if (menu_get_button_status() == LEFT_BUTTON) {
					help_screen_index--;
					if (help_screen_index < 0) help_screen_index = 0;
				}
			} while (help_screen_index < HELP_SCREEN_COUNT);
			return MENU_MAIN;
		case MENU_ERROR:
			// TODO Figure out how we want to handle errors.
			// In fact, we probably don't even need this since the code should be bug-free.
			ST7735_FillScreen(ST7735_Color565(255, 0, 0)); // color the screen red
			ST7735_SetCursor(5,9);
			ST7735_OutString("ERROR!");
			ST7735_SetCursor(0,10);
			ST7735_OutString("Where am I?\nHow did I\nget here??\n");
			ST7735_OutString("(TODO messages)");
			uint16_t color = 0;
			uint16_t count = 0;
			// Cycle through all colors 5 times on this random pixel.
			while (count < 5) {
				ST7735_DrawPixel(100,100,color++);
				if (color == 0) count++;
			}
			return MENU_MAIN; // let's try to get back to the main menu.
			
	}
}

uint8_t show_main_menu_selector(uint8_t rows[], uint8_t row_count) {
	uint8_t button_status = 0;
	uint8_t current_row = 0;
	while (menu_get_button_status() != RIGHT_BUTTON) {
		ST7735_SetCursor(0, rows[current_row]);
		ST7735_OutChar('*');
		ST7735_SetCursor(15, rows[current_row]);
		ST7735_OutChar('*');
		if (menu_get_button_status() == LEFT_BUTTON) { // if the scroll button is pressed
			ST7735_SetCursor(0, rows[current_row]);
			ST7735_OutChar(' ');
			ST7735_SetCursor(15, rows[current_row]);
			ST7735_OutChar(' ');
			if (current_row < row_count) current_row++;
			else current_row = 0;
			while(menu_get_button_status() != LEFT_BUTTON) {} // until the scroll button is released
		}
	}
	return current_row; // We return the row which was selected (e.g. 0 is play, 1 is help, 2 exit)
}

