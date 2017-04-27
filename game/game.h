#include<stdint.h>
#include "queue.h"
#include "../math/physics.h"

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

// Get if Entity1 is colliding with Entity2
// Inputs: e1, e2 - pointers to entities
// w1, h1 - dimensions of e1
// w2, h2 - dimensions of e2
// 
uint8_t check_collision(Entity *e1, Entity *e2, uint16_t w1, uint16_t h1, uint16_t w2, uint16_t h2);
