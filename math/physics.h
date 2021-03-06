//physics.h
// Header file for physics.c
// Please view comments in physics.c

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

#ifndef VECTOR_DEFINED
#define VECTOR_DEFINED

typedef struct vec2_struct {
	int32_t x;
	int32_t y;
} vec2;
#endif

void calc_grav_raw(int32_t, int32_t, int32_t, int32_t, int32_t *, int32_t *);
void calc_grav(Entity *, Entity *, int32_t *, int32_t *);
void update_position(Entity *, int32_t);
void update_velocity(Entity *, int32_t, int32_t, int32_t);
void calc_net_grav(Entity *, Entity *, uint32_t, int32_t *, int32_t *);
