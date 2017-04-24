#include "game.h"
#include "../input/portf.h"
#include "../display/drawing.h"
#include "../display/buffer.h"
#include "../display/ST7735.h"
#include "../display/print.h"
#include "../math/physics.h"
#include "../timer/systick.h"

void DisableInterrupts(void);
void EnableInterrupts(void);
void StartCritical(void);
void EndCritical(void);

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
	//buffer_test();
	uint32_t time, dt;
	
	// If we enable interrupts here, the buffer doesn't work
	// even if we disable interrupts around the buffer functions
	// as we execute them below.
	systick_init(80000, 1);
	
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
	buffer_circle(e2.posX/1000,e2.posY/1000, 20, buffer_color(255,0,0));	
	time = systick_getms();
	portf_toggle(2);
	while(portf_get(0)==0) { //as long as PF0 is not pressed		
		portf_toggle(1);
		// Calculate the force:
		int32_t forceX, forceY;
		calc_grav(&e1, &e2, &forceX, &forceY);
		ST7735_SetCursor(0,0);
		ST7735_OutString("         ");
		ST7735_SetCursor(0,0);
		LCD_OutDec(forceX);
		ST7735_SetCursor(0,1);
		ST7735_OutString("         ");
		ST7735_SetCursor(0,1);
		LCD_OutDec(forceY);

		// Get the time elapsed, in milliseconds:
		dt = systick_getms()-time;
		time = systick_getms();
		// Update the velocities:
		update_velocity(&e1, forceX, forceY, dt);
		// Erase old objects:
		buffer_circle(e1.posX/1000,e1.posY/1000, 5, 0);	//erase the previous circle
		//update the position
		update_position(&e1, dt);
		// Draw new objects:
		buffer_circle(e1.posX/1000,e1.posY/1000, 5, buffer_color(255,0,0));		
//		buffer_write();
	}
	return 0;	
}
