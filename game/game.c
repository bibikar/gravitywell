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
#include "../sound/sound.h"
#define EVENT_QUEUE_SIZE 8
#define STAR_STACK_SIZE 128
#define ASTEROID_STACK_SIZE 64
#define BONUS_STACK_SIZE 16
#define MISSILE_STACK_SIZE 8

#define STARS_PER_FRAME 3
#define ASTEROID_SPAWN_FRAMES 10
#define BONUS_SPAWN_FRAMES 100
#define MISSILE_VELOCITY_MULTIPLIER 3

#define SHIP_DISPLAY_X 55
#define SHIP_DISPLAY_Y 120
#define SHIP_WIDTH_PHYSICS 20000
#define SHIP_HEIGHT_PHYSICS 20000
#define SHIP_SPEED_LIMIT 200

#define BONUS_WIDTH_PHYSICS 10000
#define BONUS_HEIGHT_PHYSICS 10000
#define BONUS_WIDTH_DISPLAY BONUS_WIDTH_PHYSICS/1000
#define BONUS_HEIGHT_DISPLAY BONUS_HEIGHT_PHYSICS/1000
#define BONUS_SCORE_AMOUNT 100

#define MISSILE_WIDTH_PHYSICS 1000
#define MISSILE_HEIGHT_PHYSICS 10000

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
// BONUS_ERASE: allows the player to erase all asteroids in the field
typedef enum bonus_type {
	BONUS_UNUSED,
	BONUS_HEALTH,
	BONUS_ATTACK,
	BONUS_ERASE,
	BONUS_SCORE
} BonusType;

// An even simpler type of entity for bonuses. 
// There will be a maximum number of bonuses which can appear per level.
// They will be stationary in the field, but not stationary to the player.
typedef struct bonus_struct {
	int32_t posX;
	int32_t posY;
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
static uint32_t bonus_spawn_index = 0;

static uint16_t missile_stack_arr[MISSILE_STACK_SIZE];
Stack missile_stack;
static Entity missile_arr[MISSILE_STACK_SIZE];

static Entity ship;
static uint8_t ship_health;
static uint32_t score;
static uint8_t bombs, missiles;

typedef struct point_struct {
	int32_t x;
	int32_t y;
} Point;

Point get_display_coordinates(Entity *e) {
	return (Point) {(e->posX - ship.posX)/1000 + SHIP_DISPLAY_X, 
		(e->posY - ship.posY)/1000 + SHIP_DISPLAY_Y};
}

// Get if Entity1 is colliding with Entity2
// Inputs: e1, e2 - pointers to entities
// w1, h1 - dimensions of e1
// w2, h2 - dimensions of e2
// Entity has mass, velX, velY, posX, posY
uint8_t check_collision(Entity *e1, Entity *e2, uint16_t w1, uint16_t h1, uint16_t w2, uint16_t h2){
	//entity has the properties: mass, velX, velY, posX, posY, here position is the top left coordinates
	int32_t flag=0;	//flag will be 1 if the rectangles overlap 
	
	if ((e1->posX >= e2->posX && e1->posX <= e2->posX + w2) || (e2->posX >= e1->posX && e2->posX <= e1->posX + w1))
		flag |= 1;
	if ((e1->posY >= e2->posY && e1->posY <= e2->posY + h2) || (e2->posY >= e1->posY && e2->posY <= e1->posY + h1))
		flag |= 2;
	return flag == 3;
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

	// Initialize the Missile stack and entities.
	// Again using millipixels.
	stack_init(&missile_stack, missile_stack_arr, MISSILE_STACK_SIZE);
	for (int i = 0; i < MISSILE_STACK_SIZE; i++) {
		missile_arr[i].posX = 0;
		missile_arr[i].posY = 0;
		missile_arr[i].velX = 0;
		missile_arr[i].velX = 0;
		missile_arr[i].mass = 0;
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
	ship.velY = -50; // the ship moves upwards
	ship_health = 3;
	if (level == 1) {
		score = 0; // only clear the score if new game
		missiles = 0;
		bombs = 0;
	}
	
	
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
			if (event == 0 && missiles) {
				// Fire a missile

				missiles--;
				Entity missile;
				missile.posX = ship.posX;
				missile.posY = ship.posY;
				missile.velX = ship.velX;
				missile.velY = ship.velY * MISSILE_VELOCITY_MULTIPLIER;
				missile.mass = 1000;

				uint16_t missile_index = stack_pop(&missile_stack);
				missile_arr[missile_index] = missile;
			}
			else if (event == 4 && bombs) {
				// Erase all asteroids.
				bombs = 0; // get rid of bombs
				for (int i = 0; i < ASTEROID_STACK_SIZE; i++) {
					if (asteroid_arr[i].mass != 0) {
						asteroid_arr[i].mass = 0;
						stack_push(&asteroid_stack, i);
					}
				}
			}
		}

		// Poll the potentiometer(s)
		int32_t thruster_force = adc_poll()/16;


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

		// Evaluate speed limits:
		if (ship.velX > SHIP_SPEED_LIMIT) ship.velX = SHIP_SPEED_LIMIT;
		else if (ship.velX < -1*SHIP_SPEED_LIMIT) ship.velX = -1*SHIP_SPEED_LIMIT;

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
		for (int i = 0; i < MISSILE_STACK_SIZE; i++) {
			update_position(&missile_arr[i], dt);
		}

		// Update the ship position
		update_position(&ship, dt);

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
		for (uint16_t i = 0; i < MISSILE_STACK_SIZE; i++) {
			// If this missile is to be drawn (mass!=0) and is below the screen...
			if (missile_arr[i].mass != 0 && get_display_coordinates(&missile_arr[i]).y > 300) {
				// Put its index back in the stack of missiles we can use again
				stack_push(&missile_stack, i);
				// Set the "unused missile" flag - mass = 0.
				// If the mass is zero, then we'll skip drawing.
				missile_arr[i].mass = 0;
			}
		}
		for (int i = 0; i < BONUS_STACK_SIZE; i++) {
			// If this bonus is to be drawn (BONUS_UNUSED) and is below the screen...
			Entity e;
			e.posX = bonus_arr[i].posX;
			e.posY = bonus_arr[i].posY;
			if (get_display_coordinates(&e).y > 170) {
				// Put its index back in the stack of bonuss we can use again
				stack_push(&bonus_stack, i);
				// Set the "unused bonus" flag 
				// If the mass is zero, then we'll skip drawing.
				bonus_arr[i].type = BONUS_UNUSED;
			}
		}


		// Draw new objects:
		// Shooting stars are in the background:
		for (int i = 0; i < STAR_STACK_SIZE; i++) {
			if (star_arr[i].vel == 0) continue;
			buffer_star(star_arr[i].posX, star_arr[i].posY);
		}

		// Draw bonuses (between asteroids and shooting stars)
		for (int i = 0; i < BONUS_STACK_SIZE; i++) {
			if (bonus_arr[i].type == BONUS_UNUSED) continue;

			// Get the display coordinates of the bonus:
			Entity e;
			e.posX = bonus_arr[i].posX;
			e.posY = bonus_arr[i].posY;
			Point bonus_pt = get_display_coordinates(&e);

			// If these coordinates are off the screen, don't attempt to render anything.
			if (bonus_pt.x < 0 || bonus_pt.x > 128 || bonus_pt.y < 0 || bonus_pt.y > 160)
				continue;

			// Else, actually render the bonus.
			buffer_circle(bonus_pt.x, bonus_pt.y, BONUS_WIDTH_DISPLAY/2, buffer_color(0, 255, 0));
		}

		// Draw asteroids (in the foreground)
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

		// Draw missiles
		for (int i = 0; i < MISSILE_STACK_SIZE; i++) {
			// If unused, don't draw
			if (missile_arr[i].mass == 0) continue;

			// Get display coordinates:
			Point missile_pt = get_display_coordinates(&missile_arr[i]);

			// If these coordinates are off the screen, don't attempt to render anything.
			if (missile_pt.x < 0 || missile_pt.x > 128 || missile_pt.y < 0 || missile_pt.y > 160)
				continue;

			// Else, actually render the missile.
			buffer_missile(missile_pt.x, missile_pt.y);
		}

		//
		// TODO draw the ship here
		Point ship_pt = get_display_coordinates(&ship);
		buffer_circle(ship_pt.x, ship_pt.y, 10, buffer_color(255, 0, 0));


		// TODO Check if the level is over. If so, return!
		// Check collisions:
		
		// Check if the player collided with asteroids:
		for (int i = 0; i < ASTEROID_STACK_SIZE; i++) {
			if (asteroid_arr[i].mass == 0) continue;
			// check the collision
			if (check_collision(&ship, &asteroid_arr[i], SHIP_WIDTH_PHYSICS, SHIP_HEIGHT_PHYSICS,
				 asteroid_arr[i].mass*2, asteroid_arr[i].mass*2)) {

				// Since we know the collision has occurred,
				// make sure we get rid of the asteroid
				// so the same asteroid doesn't cause multiple collisions.
				asteroid_arr[i].mass = 0;
				stack_push(&asteroid_stack, i);

				// Decrement the ship's health.
				ship_health--;
				// The ship can't collide with multiple things in one frame:
				break;
			}
			// if collision is true, then decrement health
		}

		// Check if any missiles collided with asteroids:
		for (int i = 0; i < MISSILE_STACK_SIZE; i++) {
			if (missile_arr[i].mass == 0) continue;

			// Iterate through all asteroids
			for (int j = 0; j < ASTEROID_STACK_SIZE; j++) {
				if (asteroid_arr[j].mass == 0) continue;
				if (check_collision(&missile_arr[i], &asteroid_arr[i], MISSILE_WIDTH_PHYSICS, MISSILE_HEIGHT_PHYSICS,
							asteroid_arr[j].mass*2, asteroid_arr[j].mass*2)) {
					// The collision occurred. We get rid of both 
					// the asteroid and the missile.
					asteroid_arr[j].mass = 0;
					stack_push(&asteroid_stack, j);
					missile_arr[i].mass = 0;
					stack_push(&missile_stack, i);

					// The missile can't collide with multiple things in one frame.
					break;
				}
			}
		}

		// Check if the player collided with bonuses:
		for (int i = 0; i < BONUS_STACK_SIZE; i++) {
			if (bonus_arr[i].type == BONUS_UNUSED) continue;
			// check the collision
			Entity bonus_e;
			bonus_e.posX = bonus_arr[i].posX;
			bonus_e.posY = bonus_arr[i].posY;

			if (check_collision(&ship, &bonus_e, SHIP_WIDTH_PHYSICS, SHIP_HEIGHT_PHYSICS,
				BONUS_WIDTH_PHYSICS, BONUS_HEIGHT_PHYSICS)) {

				// We collided with a bonus.
				score += BONUS_SCORE_AMOUNT;
				switch(bonus_arr[i].type) {
				case BONUS_HEALTH:
					ship_health++;
					break;
				case BONUS_ATTACK:
					if (missiles +3 < 256) missiles += 3;
					break;
				case BONUS_ERASE:
					if (bombs == 0) bombs = 1;
					break;
				default:
					// do nothing
					break;
				}

				// Get rid of the bonus.
				bonus_arr[i].type = BONUS_UNUSED;
				stack_push(&bonus_stack, i);
			}

		}

		// Display number of missiles, top left corner
		buffer_num(0, 0, missiles, buffer_color(255,255,0));
		// Display if we have a bomb, top right corner (icon)
		if (bombs) buffer_char(122, 0, '\4', buffer_color(255,255,0));
		// Display the health:
		for (int i = 0; i < ship_health; i++) {
			buffer_string(2+i*6, 150, "\3", buffer_color(255,0,0));
		}

		// Check if the player is still alive.
		// If dead, return game over.
		if (ship_health < 1) {
			// Display GAME OVER with a box
			buffer_string(36, 48, "GAME OVER", buffer_color(255, 0, 0));
			buffer_rect_outline(36 - 4, 48 - 4, 60, 14, buffer_color(255,0,0));

			// Display you died and score
			buffer_string(0, 64, "      You died!", buffer_color(255, 0, 0));
			buffer_string(0, 80, "    Score:", buffer_color(255, 0, 0));
			buffer_num(72, 80, score, buffer_color(255, 0, 0));

			// Cue to push button
			buffer_string(0, 128, " Press any button to", buffer_color(255, 255, 0));
			buffer_string(0, 144, "  exit to main menu", buffer_color(255, 255, 0));
			
			// Write new buffer
			buffer_write();

			// Wait for event queue to get something, then
			// disable the GPIO interrupts. 
			// We then finally return the GameStatus saying
			// to go back to the main menu.
			while (queue_empty(&event_queue)) {}
			portf_disable_interrupts();
			GameStatus ret = {0, score};
			return ret;
		}

		// Write the buffer to the display.
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
			if (++asteroid_spawn_index % (ASTEROID_SPAWN_FRAMES) == 0) {
				// Spawn one asteroid above the player
				uint16_t asteroid_index = stack_pop(&asteroid_stack);
				asteroid_arr[asteroid_index].posX = ship.posX + srandom() % 160000 - 80000;
				asteroid_arr[asteroid_index].posY = ship.posY - SHIP_DISPLAY_Y*1000 - srandom() % 10000;
				asteroid_arr[asteroid_index].velX = 0;
				asteroid_arr[asteroid_index].velY = 0;
				asteroid_arr[asteroid_index].mass = 4000;
				
				// Spawn one asteroid to the side of the player
				asteroid_index = stack_pop(&asteroid_stack);
				// Make it relative to the player, either to the left or right (positive or negative)
				// of a random X
				// This way the player cna't just go left or right.
				asteroid_arr[asteroid_index].posX = ship.posX + ((srandom() % 2) * 2 - 1) * (srandom() % 80000 + 64000);
				asteroid_arr[asteroid_index].posY = ship.posY - srandom() % 64000;
				asteroid_arr[asteroid_index].velX = 0;
				asteroid_arr[asteroid_index].velY = 0;
				asteroid_arr[asteroid_index].mass = 5000;
			}
			// TODO fill these with random values which make sense
		}

		// Generate new bonuses
		if (!stack_empty(&bonus_stack)) {
			if (++bonus_spawn_index % BONUS_SPAWN_FRAMES == 0) {
				// Spawn bonuses randomly. Some have better chances than others.
				uint16_t bonus_index = stack_pop(&bonus_stack);
				bonus_arr[bonus_index].posX = ship.posX + srandom() % 160000 - 80000;
				bonus_arr[bonus_index].posY = ship.posY - SHIP_DISPLAY_Y*1000 - srandom() % 10000;
				bonus_arr[bonus_index].type = BONUS_SCORE; // by default, we'll just spawn score objects
				
				// Assign bonus types by random:
				uint16_t chooser = srandom() % 1000;
				if (chooser < 500) bonus_arr[bonus_index].type = BONUS_HEALTH;
				if (chooser < 300) bonus_arr[bonus_index].type = BONUS_ATTACK;
				if (chooser < 100) bonus_arr[bonus_index].type = BONUS_ERASE;
			}
		}
	}
	portf_disable_interrupts();
	// TODO Return what happened, instead of just 0 = main menu
	return (GameStatus) {0, score};	
}

Queue *get_event_queue() {
	return &event_queue;
}
