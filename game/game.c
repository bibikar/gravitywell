#include "game.h"
#include "queue.h"
#include "stack.h"
#include "../input/portf.h"
#include "../display/drawing.h"
#include "../display/buffer.h"
#include "../display/ST7735.h"
#include "../display/print.h"
#include "../math/physics.h"
#include "../timer/systick.h"
#include "../math/random.h"
#include "../math/random_squares.h"

#define EVENT_QUEUE_SIZE 8
#define STAR_STACK_SIZE 128
#define ASTEROID_STACK_SIZE 64
#define STARS_PER_FRAME 3

#define PAUSE_MESSAGE_X 34
#define PAUSE_MESSAGE_Y 80

void DisableInterrupts(void);
void EnableInterrupts(void);
void StartCritical(void);
void EndCritical(void);

typedef struct star_struct {
	int16_t posX;
	int16_t posY;
	uint8_t vel;
} Star;

static uint32_t event_arr[EVENT_QUEUE_SIZE];
Queue event_queue;

static uint16_t star_stack_arr[STAR_STACK_SIZE];
Stack star_stack;
static Star star_arr[STAR_STACK_SIZE];

static uint16_t asteroid_stack_arr[ASTEROID_STACK_SIZE];
Stack asteroid_stack;
static Entity asteroid_arr[ASTEROID_STACK_SIZE];

uint8_t buffer_test() {
	uint8_t color = 0;
	while(portf_get(0) == 0) {
	buffer_fill(color++);
	buffer_write();
	}
	return 0;	
}
uint8_t game_test()
{	
	// Initialize the event queue.
	queue_init(&event_queue, event_arr, EVENT_QUEUE_SIZE);
	
	// Initialize the shooting star stack and entities.
	stack_init(&star_stack, star_stack_arr, STAR_STACK_SIZE);
	for (int i = 0; i < STAR_STACK_SIZE; i++) {
		star_arr[i] = (Star) {0, 0, 0};
	}

	// Initialize the Asteroid stack and entities.
	// Remember we are using "millipixels" as the unit
	stack_init(&asteroid_stack, asteroid_stack_arr, ASTEROID_STACK_SIZE);
	for (int i = 0; i < ASTEROID_STACK_SIZE; i++) {
		asteroid_arr[i] = (Entity) {0,0,0,0,0};
	}

	// Initialize the player's ship
	// Remember we are using "millipixels" as the unit
	Entity ship = (Entity) {0, 0, 0, 0, 0}; // TODO put values here which make sense.
	
	// Initialize the random number generator.
	// We actually have two methods of getting random numbers:
	// One is the linear congruential generator given in the lab documents
	// The second is the "middle squares method" with a Weyl sequence added each time
	// such that the output does not cycle. This one is extremely efficient and uses no loops,
	// and should be suitable for our application, especially for the shooting stars in
	// the background of the game.
	Random_Init(12579697);

	systick_init(80000, 1);
	portf_enable_interrupts();
	uint32_t time, dt;
	

	buffer_clear();
	buffer_write();
	time = systick_getms();
	portf_toggle(2);
	while(1) {
		portf_toggle(1);
		while (!queue_empty(&event_queue)) {
			portf_toggle(3);
			uint32_t event = queue_poll(&event_queue);
			if (event == 0) {

				// This means we've entered the pause menu.
				// Print the pause message and continue, with a box around the former.
				buffer_rect(PAUSE_MESSAGE_X - 4, PAUSE_MESSAGE_Y - 4, 72, 14, 0);
				buffer_string(PAUSE_MESSAGE_X,PAUSE_MESSAGE_Y,"GAME PAUSED",buffer_color(0,0,255));
				buffer_rect_outline(PAUSE_MESSAGE_X - 4, PAUSE_MESSAGE_Y - 4, 72, 14, buffer_color(0,0,255));
				buffer_rect(72, 147, 50, 13, 0);
				buffer_string(75, 150, "Continue", buffer_color(0, 0, 255));
				buffer_write();
				
				// Don't exit until PF4 is pressed.
				do { 
					while (queue_empty(&event_queue)) {}
					event = queue_poll(&event_queue);
				} while (event == 0);
				buffer_clear();
				time = systick_getms();
			}
		}
		// Calculate the force:
		//int32_t forceX, forceY;
		//calc_grav(&e1, &e2, &forceX, &forceY);

		// Get the time elapsed, in milliseconds:
		dt = systick_getms()-time;
		time = systick_getms();

		// Update the velocities:
		//update_velocity(&e1, forceX, forceY, dt);

		// Erase old objects:
		// Completely clearing the buffer is the easiest way to do this.
		// Everything will be redrawn anyway.

		buffer_clear();

		//update the position
		for (int i = 0; i < STAR_STACK_SIZE; i++) {
			if (star_arr[i].vel == 0) continue;
			// Since these are not of the same type as the asteroid entities
			// we haven't accounted for that in the physics engine, so we
			// just do our own calculation.
			// Everything here behaves differently anyway.
			// Stars are given coordinates which are the same as their
			// on-screen coordinates,
			// while the asteroids must be rendered relative to the spaceship.
			star_arr[i].posY += star_arr[i].vel * dt / 400;
		}
		for (int i = 0; i < ASTEROID_STACK_SIZE; i++) {
			update_position(&asteroid_arr[i], dt);
		}

		// Remove old objects:
		for (int i = 0; i < STAR_STACK_SIZE; i++) {
			if (star_arr[i].posY > 160) {
				stack_push(&star_stack, i);
				// If vel ==0 skip draw
				star_arr[i].vel = 0;
			}
		}
		for (int i = 0; i < ASTEROID_STACK_SIZE; i++) {
			// TODO account for the ship's position and the fact that
			// the asteroids are being rendered relative to the ship instead 
			// of absolutely.
			if (asteroid_arr[i].posY > 160) {
				stack_push(&asteroid_stack, i);
				// If the mass is zero, then we'll skip drawing.
				asteroid_arr[i].mass = 0;
			}
		}

		// Draw new objects:
		// Shooting stars are in the background:
		for (int i = 0; i < STAR_STACK_SIZE; i++) {
			if (star_arr[i].vel == 0) continue;
			buffer_star(star_arr[i].posX, star_arr[i].posY);
		}
		// The asteroids are in the foreground.
		buffer_write();

		// Generate new asteroids and shooting stars:
		if (!stack_empty(&star_stack)) {
			for (int i = 0; i < STARS_PER_FRAME; i++) {
				uint16_t star_index = stack_pop(&star_stack);
				star_arr[star_index] = (Star){srandom()%128,-10,srandom()%32+30};
			}
		}

		if (!stack_empty(&asteroid_stack)) {
			// TODO add a condition here: only spawn new asteroids when we want to
			uint16_t asteroid_index = stack_pop(&asteroid_stack);
			asteroid_arr[asteroid_index] = (Entity){0, 0, 0, 0, 0}; 
			// TODO fill these with random values which make sense
		}
	}
	portf_disable_interrupts();
	// TODO Return what happened, instead of just 0 = main menu
	return 0;	
}

Queue *get_event_queue() {
	return &event_queue;
}
