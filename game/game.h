#include<stdint.h>
#include "queue.h"

#ifndef GAME_STATUS_DEFINED
#define GAME_STATUS_DEFINED
typedef struct game_status_struct {
	uint8_t status;
	uint32_t data;
} GameStatus;
#endif

// The game itself
GameStatus game_test(uint8_t level);

// Get the event queue.
Queue *get_event_queue();
