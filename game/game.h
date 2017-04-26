#include<stdint.h>
#include "queue.h"

typedef struct game_status_struct {
	uint8_t status;
	uint32_t data;
} GameStatus;

// The game itself
uint8_t game_test();

// Get the event queue.
Queue *get_event_queue();
