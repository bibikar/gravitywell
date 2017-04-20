#include "game.h"
#include "../input/portf.h"
#include "../display/drawing.h"
#include "../display/ST7735.h"
#include "../math/physics.h"
#include "../timer/systick.h"


uint8_t game_test()
{	
	ST7735_FillScreen(0);
	uint32_t time, dt;
	
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
	
	time = systick_getms();
	drawing_circle(e2.posX/1000,e2.posY/1000, 5, ST7735_Color565(255,0,0));	
	while(portf_get(0)==0) { //as long as PF0 is not pressed		
		// Calculate the force:
		vec2 force = calc_grav(&e1, &e2);
		// Get the time elapsed, in milliseconds:
		dt = systick_getms()-time;
		time = systick_getms();
		// Update the velocities:
		update_velocity(&e1, force, dt);
		// Erase old objects:
		drawing_circle(e1.posX/1000,e1.posY/1000, 5,0);	//erase the previous circle
		//update the position
		update_position(&e1, dt);
		// Draw new objects:
		drawing_circle(e1.posX/1000,e1.posY/1000, 5, ST7735_Color565(255,0,0));		
	}
	
	return 0;	
}
