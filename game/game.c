#include "game.h"
#include "queue.h"
#include "../input/portf.h"
#include "../display/drawing.h"
#include "../display/buffer.h"
#include "../display/ST7735.h"
#include "../display/print.h"
#include "../math/physics.h"
#include "../timer/systick.h"

#define EVENT_QUEUE_SIZE 8
#define STAR_STACK_SIZE 128

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
static Star[STAR_STACK_SIZE];


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
				buffer_string(0,80,"GAME PAUSED",buffer_color(0,0,255));
				buffer_write();
				while(portf_get(0)){};
				while(!queue_empty(&event_queue))
					queue_poll(&event_queue);
				EnableInterrupts();
				
				do
				{ while (queue_empty(&event_queue)) {}
					event = queue_poll(&event_queue);
				}while(event==0);
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
		buffer_circle(e1.posX/1000,e1.posY/1000, 5, 0);	//erase the previous circle
		//update the position
		update_position(&e1, dt);

		// Remove old objects:
		for (int i = 0; i < STAR_STACK_SIZE; i++) {
			if (star_arr[star_index].posY > 160) {
				stack_push(&star_stack, i);
				star_arr[star_index].vel = 0;
			}
		}

		// Draw new objects:
		buffer_circle(e2.posX/1000,e2.posY/1000, 20, buffer_color(255,0,0));	
		buffer_circle(e1.posX/1000,e1.posY/1000, 5, buffer_color(255,0,0));		
		buffer_write();

		// Generate new asteroids and shooting stars:
		if (!stack_empty(&star_stack)) {
			uint16_t star_index = stack_pop(&star_stack);
			star_arr[star_index] = {20,-10,20}; // this needs to be random
		}
	}
	portf_disable_interrupts();
	return 0;	
}

Queue *get_event_queue() {
	return &event_queue;
}
