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
#include "../input/ADC.h"
#define EVENT_QUEUE_SIZE 8
#define STAR_STACK_SIZE 128
#define ASTEROID_STACK_SIZE 64
#define BONUS_STACK_SIZE 16

#define STARS_PER_FRAME 3
#define ASTEROID_SPAWN_FRAMES 10
#define BONUS_PER_LEVEL 3

#define SHIP_DISPLAY_X 60
#define SHIP_DISPLAY_Y 40
#define PAUSE_MESSAGE_X 34
#define PAUSE_MESSAGE_Y 80

void DisableInterrupts(void);
void EnableInterrupts(void);
void StartCritical(void);
void EndCritical(void);

// A simpler type of entity for stars,
// which only has one velocity. These are rendered
// with absolute coordinates, unlike asteroids, which
// are in relative millipixels
typedef struct star_struct {
	int16_t posX;
	int16_t posY;
	uint8_t vel;
} Star;

// The possible types of bonuses that can appear
// BONUS_UNUSED: notes that the bonus isn't on the screen and can't be collected
// BONUS_HEALTH: repairs the player's ship immediately
// BONUS_ATTACK: allows the player to fire a projectile which destroys asteroids
// BONUS_SCORE: grants the player a score bonus
typedef enum bonus_type {
	BONUS_UNUSED,
	BONUS_HEALTH,
	BONUS_ATTACK,
	BONUS_SCORE
} BonusType;

// An even simpler type of entity for bonuses. 
// There will be a maximum number of bonuses which can appear per level.
// They will be stationary in the field, but not stationary to the player.
typedef struct bonus_struct {
	int16_t posX;
	int16_t posY;
	BonusType type;
} Bonus;

// The event queue (port F GPIO edge triggered interrupts will update)
static uint32_t event_arr[EVENT_QUEUE_SIZE];
Queue event_queue;

// The stack of array indices for stars.
// This is the basis of our alternative to malloc.
// We have a given number of array indices stored in
// the star_stack, which are popped off as we need new
// spaces to put new stars. This way, stars can have different
// velocities and can be reused at different times.
// When the stars are off the screen, they are flagged as done,
// and their array index is pushed back onto the stack.
static uint16_t star_stack_arr[STAR_STACK_SIZE];
Stack star_stack;
static Star star_arr[STAR_STACK_SIZE];

static uint16_t asteroid_stack_arr[ASTEROID_STACK_SIZE];
Stack asteroid_stack;
static Entity asteroid_arr[ASTEROID_STACK_SIZE];
static uint32_t asteroid_spawn_index = 0;

static uint16_t bonus_stack_arr[BONUS_STACK_SIZE];
Stack bonus_stack;
static Bonus bonus_arr[BONUS_STACK_SIZE];

static Entity ship;
static uint8_t ship_health;
static uint32_t score;

typedef struct point_struct {
	int32_t x;
	int32_t y;
} Point;

Point get_display_coordinates(Entity *e) {
	return (Point) {(e->posX - ship.posX)/1000 + SHIP_DISPLAY_X, 
		(e->posY - ship.posY)/1000 + SHIP_DISPLAY_Y};
}

GameStatus game_test(uint8_t level)
{	
	ADC_Init();
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
		asteroid_arr[i].posX = 0;
		asteroid_arr[i].posY = 0;
		asteroid_arr[i].velX = 0;
		asteroid_arr[i].velX = 0;
		asteroid_arr[i].mass = 0;
	}

	// Initialize the Bonus stack and entities.
	// Using millipixels as the unit
	stack_init(&bonus_stack, bonus_stack_arr, BONUS_STACK_SIZE);
	for (int i = 0; i < BONUS_STACK_SIZE; i++) {
		bonus_arr[i].type = BONUS_UNUSED;
	}

	// Initialize the player's ship
	// Remember we are using "millipixels" as the unit
	ship.mass = 1000;
	ship.posX = 0;
	ship.posY = 0; // starting position
	ship.velX = 0;
	ship.velY = -10; // the ship moves upwards
	ship_health = 3;
	if (level == 1) score = 0; // only clear the score if new game
	
	
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

		// TODO Poll the potentiometer(s)
		int32_t thruster_force = adc_poll(); // TODO get this from potentiometer


		// Calculate the force:
		//int32_t forceX, forceY;
		//calc_grav(&e1, &e2, &forceX, &forceY);

		// Get the time elapsed, in milliseconds:
		dt = systick_getms()-time;
		time = systick_getms();

		// Update the velocities:
		// We really only need to do this for the ship, given the forces of the potentiometer.
		// If we have a second potentiometer, we put that value where 
		// it's 0 right now.
		update_velocity(&ship, thruster_force, 0, dt);

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
			if (star_arr[i].vel != 0 && star_arr[i].posY > 160) {
				stack_push(&star_stack, i);
				// If vel ==0 skip draw
				star_arr[i].vel = 0;
			}
		}
		for (uint16_t i = 0; i < ASTEROID_STACK_SIZE; i++) {
			// If this asteroid is to be drawn (mass!=0) and is below the screen...
			if (asteroid_arr[i].mass != 0 && get_display_coordinates(&asteroid_arr[i]).y > 170) {
				// Put its index back in the stack of asteroids we can use again
				stack_push(&asteroid_stack, i);
				// Set the "unused asteroid" flag - mass = 0.
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
		for (int i = 0; i < ASTEROID_STACK_SIZE; i++) {
			// Our flag for "unused asteroid" is a mass of zero.
			// If this element is unused, don't draw it.
			if (asteroid_arr[i].mass == 0) continue;

			// Get the coordinates on the screen of the asteroid:
			Point aster_pt = get_display_coordinates(&asteroid_arr[i]);

			// If these coordinates are off the screen, don't attempt to render anything.
			if (aster_pt.x < 0 || aster_pt.x > 128 || aster_pt.y < 0 || aster_pt.y > 160)
				continue;

			// Else, actually render the asteroid. It may be half off the screen, but
			// that will be fixed by buffer_circle().
			buffer_circle(aster_pt.x, aster_pt.y, asteroid_arr[i].mass / 1000, 255);
		}
		// The asteroids are in the foreground.
		//
		// TODO draw the ship here
		// Write the buffer to the display.
		buffer_write();

		// TODO Check if the level is over. If so, return!
		// TODO Check collisions.
		
		// Check if the player collided with asteroids:
		for (int i = 0; i < ASTEROID_STACK_SIZE; i++) {
			if (asteroid_arr[i].mass == 0) continue;
			// check the collision
			// if collision is true, then decrement health
		}

		for (int i = 0; i < BONUS_STACK_SIZE; i++) {
			if (bonus_arr[i].type == BONUS_UNUSED) continue;
			// check the collision
			// if collision is true, do somethign
			// depending on what type of bonus
		}
		// TODO Check if the player is still alive.
		// If dead, return game over.

		// Generate new asteroids and shooting stars:
		if (!stack_empty(&star_stack)) {
			for (int i = 0; i < STARS_PER_FRAME; i++) {
				uint16_t star_index = stack_pop(&star_stack);
				star_arr[star_index] = (Star){srandom()%128,-10,srandom()%32+30};
			}
		}

		if (!stack_empty(&asteroid_stack)) {
			// TODO add a condition here: only spawn new asteroids when we want to
			if (++asteroid_spawn_index % (srandom() % ASTEROID_SPAWN_FRAMES) == 0) {
				uint16_t asteroid_index = stack_pop(&asteroid_stack);
				asteroid_arr[asteroid_index].posX = ship.posX + srandom() % 160000 - 80000;
				asteroid_arr[asteroid_index].posY = ship.posY - SHIP_DISPLAY_Y*1000 - srandom() % 10000;
				asteroid_arr[asteroid_index].velX = 0;
				asteroid_arr[asteroid_index].velY = 0;
				asteroid_arr[asteroid_index].mass = 5000;
			}
			// TODO fill these with random values which make sense
		}
	}
	portf_disable_interrupts();
	// TODO Return what happened, instead of just 0 = main menu
	return (GameStatus) {0, score};	
}

Queue *get_event_queue() {
	return &event_queue;
}
