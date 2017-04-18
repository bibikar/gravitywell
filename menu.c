#include "ST7735.h"
#include "drawing.h"

#define MAIN_MENU_ID 0

#define MENU_SUCCESS 0
#define MENU_NEW_GAME 1
#define MENU_GOTO_HELP 2
#define MENU_ERROR 126
#define MENU_EXIT 127

#define SCROLL_BUTTON 0x01
#define SELECT_BUTTON 0x02

#define MAIN_MENU_SELECTION_ROW_COUNT 3
static const uint8_t main_menu_selection_rows[3] = {8,9,10};

// Shows the menu given by the id (see define statements)
// Returns a status code (0 = default success)
// Any other status code given becomes the new menu id.
// A status code above 127 means to go to the actual game.
uint8_t show_menu(uint32_t id) {
	switch(id) {
		case MAIN_MENU_ID:
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
				case 1: return MENU_GOTO_HELP;
				case 2: return MENU_EXIT;
				default: return MENU_ERROR;
			}
	}
}

uint8_t show_main_menu_selector(uint8_t rows[], uint8_t row_count) {
	uint8_t button_status = 0;
	uint8_t current_row = 0;
	while (1) { // Replace 1 with "until the select button is pressed"
		ST7735_SetCursor(0, rows[current_row]);
		ST7735_OutChar('*');
		ST7735_SetCursor(15, rows[current_row]);
		ST7735_OutChar('*');
		if (1) { // replace with "if the scroll button is pressed"
			while(1) {} // replace with "until the scroll button is released"
			ST7735_SetCursor(0, rows[current_row]);
			ST7735_OutChar(' ');
			ST7735_SetCursor(15, rows[current_row]);
			ST7735_OutChar(' ');
			if (current_row < row_count) current_row++;
			else current_row = 0;
		}
	}
	return current_row; // We return the row which was selected (e.g. 0 is play, 1 is help, 2 exit)
}

