// menu.h
// All public functions for menu.c
// Author = Sameer Bibikar, 2017-04-18

#define MENU_MAIN 0
#define MENU_NEW_GAME 1
#define MENU_HELP 2
#define MENU_GAME_OVER 64
#define MENU_NEXT_LEVEL 65
#define MENU_YOU_WIN 66
#define MENU_ERROR 126
#define MENU_EXIT 127
#define GAME_BEGIN 128


// Show the menu whose ID is given by id.
// Remember to give data if applicable.
uint8_t show_menu(uint8_t id, uint32_t data);

// Show the menu whose ID is given by id.
// But since we're lazy don't put any data
// (you could use show_menu with zero
// and it would do the same thing)
uint8_t show_boring_menu(uint8_t id);
