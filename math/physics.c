// physics.c
// Contains utility functions for calculating gravitational force
// and other physics things that we may use in lab 10.
// Author: Sameer Bibikar


#include <stdint.h>
#include <stdlib.h>
#include "physics.h"

// This isn't the real-world gravitational constant.
// It's just something we can scale all gravity by to make the numbers
// work for the display.
#define GRAVITATIONAL_CONSTANT 10
#define GRAVITATIONAL_DIVISOR 1

// The entity struct.
// We initialize the asteroids as an array for each level, and we can
// iterate through the array and tick each.
// We have a few options for managing gravity.
// We can have all asteroids interact with each other as well.
// This will be O(N^2) efficiency...
// Alternatively, we can have the asteroids never interact with each other
// and only exert forces on the ship.
// This method will yield O(N) efficiency. But the asteroids will still have
// to be updated every tick since they're moving downwards!
// We will also have to erase the previous positions of the asteroids from the 
// screen, then redraw all.
#ifndef ENTITY_DEFINED
#define ENTITY_DEFINED
typedef struct entity_struct {
	int32_t mass;
	int32_t velX;
	int32_t velY;
	int32_t posX;
	int32_t posY;
} Entity;
#endif

// calc_grav_raw(mass1, mass2, disp_vec)
// Calculate gravitational force (N) between two masses,
// given the displacement vector.
// This will calculate the force of mass2 on mass1.
// Inputs: mass1, mass2 (kg)
// disp_vec (array of length 2)
// Output: array of length 2 giving force in X and Y dirs.
// REMEMBER TO FREE THE ARRAY! Otherwise these things will just fill the entire RAM
vec2 calc_grav_raw(int32_t mass1, int32_t mass2, int32_t dispX, int32_t dispY) {
	int32_t x = mass1 * mass2 / dispX / dispX * GRAVITATIONAL_CONSTANT / GRAVITATIONAL_DIVISOR;
	int32_t y = mass1 * mass2 / dispY / dispY * GRAVITATIONAL_CONSTANT / GRAVITATIONAL_DIVISOR;
	vec2 force = {x, y};
	return force;
}

// calc_grav(e1, e2)
// Calculate gravitational force of entity e2 on e1.
// Inputs: Pointers to two entities
// Output: array of length 2 giving force in X and Y dirs.
// REMEMBER TO FREE THE ARRAY! Otherwise these things will just fill the entire RAM
vec2 calc_grav(Entity *e1, Entity *e2) {
	return calc_grav_raw(e1->mass, e2->mass, e1->posX - e2->posX, e1->posY - e2->posY);
}

// update_position(e, dt)
// Update position of e using its velocity values
// and the time interval dt.
// Inputs: Pointer to entity and time interval
// Outputs nothing
void update_position(Entity *e, int32_t dt) {
	e->posX += e->velX * dt;
	e->posY += e->velY * dt;
}

// update_position(e, dt)
// Update position of e using its velocity values
// and the time interval dt.
// Inputs: Pointer to entity and time interval
// Outputs nothing
void update_velocity(Entity *e, vec2 force, int32_t dt) {
	e->posX += force.x * dt / e->mass;
	e->posY += force.y * dt / e->mass;
}

// calc_net_grav(e, others, entity_count)
// Calculate the net gravitational force on e by others
// Inputs: Entity *e - pointer to entity on which the forces are working
// Entity *others - pointer to first position in array of entities
// int32_t entity_count - length of the array of entities
// Output: array of length 2 giving force in X and Y dirs.
// REMEMBER TO FREE THE ARRAY! Otherwise these things will just fill the entire RAM
vec2 calc_net_grav(Entity *e, Entity *others, uint32_t entity_count) {
	vec2 net_force = {0, 0};
	for (int i = 0; i < entity_count; i++) {
		vec2 force = calc_grav(e, others+i);
		net_force.x += force.x;
		net_force.y += force.y;
	}
	return net_force;
}
