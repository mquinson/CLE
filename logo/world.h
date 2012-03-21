/*
 * world.h: world state modification
 */

#ifndef LOGO_WORLD_H
#define LOGO_WORLD_H

#include "logo/entity.h"
#include "core/world.h"

/* Type definition */
/* The type world_t is defined in entity.h to break a cyclic dependency */

/* memory management */
core_world_t world_new(double sizeX, double sizeY);
void world_free(core_world_t w);
core_world_t world_copy(core_world_t w);
int world_eq(core_world_t w1, core_world_t w2);

/* easy getters/setters */
int world_get_amount_entity(core_world_t w);
void world_set_step_delay(core_world_t w, int step_delay);
double world_get_sizeX(core_world_t w);
double world_get_sizeY(core_world_t w);


/* state modifications from the turtle (protected functions) */
void world_entity_add(core_world_t w, entity_t t);
void world_line_add(core_world_t w, double x1, double y1, double x2, double y2);

//void world_redraw(void* w, void *cr,int sizeX,int sizeY);

entity_t world_entity_geti(core_world_t w,int i); /* helper for the foreach, don't use directly */
#define world_foreach_entity(w,it,t)\
	for (it=0; (t=world_entity_geti(w,it)) ; it++)



#endif /* WORLD_H_ */
