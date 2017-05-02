// menu.h
// All public functions for menu.c
// Author = Sameer Bibikar, Sneha Pendharkar
#include <stdint.h>
#include "../game/game.h"

#define MENU_MAIN 0
#define MENU_NEW_GAME 1
#define MENU_HELP 2
#define MENU_INFO 3
#define MENU_GAME_OVER 64
#define MENU_NEXT_LEVEL 65
#define MENU_YOU_WIN 66
#define MENU_ERROR 126
#define MENU_EXIT 127
#define GAME_BEGIN 128


// Show the menu whose ID is given by id.
// Remember to give data if applicable.
uint8_t show_menu(GameStatus status);
