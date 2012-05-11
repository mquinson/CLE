/*
 * world.h: world state modification
 */

#ifndef WORLD_H_
#define WORLD_H_

#include "fork/entity.h"
#include "fork/exercise.h"


/* Type definition */
/* The type world_t is defined in entity.h to break a cyclic dependency */

/* memory management */
core_world_t world_new(double sizeX, double sizeY);
void world_free(core_world_t w);
core_world_t world_copy(core_world_t w);
int tree_eq(tree_fork *s,tree_fork *p);
int world_eq(tree_fork **s, tree_fork **p,core_world_t *wc, core_world_t *wo, int nb_world);

/* easy getters/setters */
int world_get_amount_entity(core_world_t w);
void world_decrease_amount_entity(core_world_t w);
void world_set_step_delay(core_world_t w, int step_delay);
double world_get_sizeX(core_world_t w);
double world_get_sizeY(core_world_t w);


/* state modifications from the turtle (protected functions) */
void world_entity_add(core_world_t w, entity_t t);
void world_line_add(core_world_t w, double x1, double y1, double x2, double y2,int *color);

void world_redraw(core_world_t w, cairo_t *cr,int sizeX,int sizeY);

entity_t world_entity_geti(core_world_t w,int i); /* helper for the foreach, don't use directly */
#define world_foreach_entity(w,it,t)\
	for (it=0; (t=world_entity_geti(w,it)) ; it++)



#endif /* WORLD_H_ */
