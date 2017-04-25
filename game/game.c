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
	Random_Init(12579697);

	systick_init(80000, 1);
	portf_enable_interrupts();
	uint32_t time, dt;
	
	
	Entity e1, e2;
	//e = {1000, 0, -100, 64, 160};
	e1.mass=1000;
	e1.velX=0;
	e1.velY = -30;
	e1.posX=120000;
	e1.posY=80000;
	
	e2.mass=1000;
	e2.velX=0;
	e2.velY =0;
	e2.posX=64000;
	e2.posY=80000;	//e2 is in the center, stationary, exerts force on e1

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
				DisableInterrupts();
				buffer_rect(PAUSE_MESSAGE_X - 4, PAUSE_MESSAGE_Y - 4, 72, 14, 0);
				buffer_string(PAUSE_MESSAGE_X,PAUSE_MESSAGE_Y,"GAME PAUSED",buffer_color(0,0,255));
				buffer_rect_outline(PAUSE_MESSAGE_X - 4, PAUSE_MESSAGE_Y - 4, 72, 14, buffer_color(0,0,255));
				buffer_rect(72, 147, 50, 13, 0);
				buffer_string(75, 150, "Continue", buffer_color(0, 0, 255));
				buffer_write();
				while(portf_get(0)){};
				while(!queue_empty(&event_queue))
					queue_poll(&event_queue);
				EnableInterrupts();
				
				do
				{ while (queue_empty(&event_queue)) {}
					event = queue_poll(&event_queue);
				}while(event==0);
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

		buffer_clear();
		buffer_circle(e1.posX/1000,e1.posY/1000, 5, 0);	//erase the previous circle

		//update the position
		update_position(&e1, dt);
		for (int i = 0; i < STAR_STACK_SIZE; i++) {
			if (star_arr[i].vel == 0) continue;
			star_arr[i].posY += star_arr[i].vel * dt / 400;
		}

		// Remove old objects:
		for (int i = 0; i < STAR_STACK_SIZE; i++) {
			if (star_arr[i].posY > 160) {
				stack_push(&star_stack, i);
				star_arr[i].vel = 0;
			}
		}

		// Draw new objects:
		// Shooting stars are in the background:
		for (int i = 0; i < STAR_STACK_SIZE; i++) {
			if (star_arr[i].vel == 0) continue;
			buffer_star(star_arr[i].posX, star_arr[i].posY);
		}
		// The asteroids are in the foreground.
		buffer_circle(e2.posX/1000,e2.posY/1000, 20, buffer_color(255,0,0));	
		buffer_circle(e1.posX/1000,e1.posY/1000, 5, buffer_color(255,0,0));		
		buffer_write();

		// Generate new asteroids and shooting stars:
		if (!stack_empty(&star_stack)) {
			for (int i = 0; i < STARS_PER_FRAME; i++) {
				uint16_t star_index = stack_pop(&star_stack);
				star_arr[star_index] = (Star){srandom()%128,-10,srandom()%32+30};
			}
		}
	}
	portf_disable_interrupts();
	return 0;	
}

Queue *get_event_queue() {
	return &event_queue;
}
