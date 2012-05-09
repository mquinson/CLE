/*
 * world.c: defines the data associated to a given world
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>

#include "logo/world.h"
#include "logo/entity.h"
#include "logo/world_view.h"
#include "core/world.h"
#include "logo/exercise.h"

typedef struct s_line {
	double x1,y1,x2,y2;
} s_line_t;

struct s_world {
	/* */
	struct s_core_world mother;

	/* inhabitants */
	int amount_entity;
	entity_t *entities;
	
	/* world state */
	int amount_lines;
	s_line_t *lines;
	
};

core_world_t world_new(double sizeX, double sizeY) {
	world_t res = malloc(sizeof(struct s_world));
	res->mother.sizeX = sizeX;
	res->mother.sizeY = sizeY;
	res->mother.step_delay = 0;
	res->mother.world_repaint = world_redraw;
	res->mother.exercise_run = exercise_run;
	res->mother.exercise_stop = exercise_stop;
	res->mother.exercise_demo = exercise_demo;

	res->amount_entity = 0;
	res->entities = NULL;

	res->amount_lines = 0;
	res->lines = NULL;
	return (core_world_t)res;
}
void world_free(core_world_t world) {
	world_t w = (world_t) world;
	int it;
	for (it=0;it<w->amount_entity;it++)
		entity_free(w->entities[it]);
	free(w->entities);
	free(w->lines);
	free(w);
}
core_world_t world_copy(core_world_t world) {
	world_t w = (world_t) world;
	int it;
	world_t res = (world_t)world_new(w->mother.sizeX,w->mother.sizeY);
	res->mother.step_delay = world->step_delay;
	res->mother.world_repaint = world->world_repaint;
	res->mother.exercise_run = world->exercise_run;
	res->mother.exercise_stop = world->exercise_stop;

	res->amount_entity = w->amount_entity;
	res->entities = malloc(sizeof(entity_t)*w->amount_entity);
	for (it=0;it<res->amount_entity;it++) {
		res->entities[it] = entity_copy(w->entities[it]);
		entity_set_world(res->entities[it], res);
	}

	res->amount_lines = w->amount_lines;
	res->lines = malloc(sizeof(struct s_line)*w->amount_lines);
	memcpy(res->lines,w->lines,sizeof(struct s_line)*w->amount_lines);
	return (core_world_t)res;
}


int world_eq(core_world_t world1, core_world_t world2) {
  	world_t w1 = (world_t) world1;
	world_t w2 = (world_t) world2;
	int it;

	if (w1->amount_entity != w2->amount_entity) return FALSE;
	for (it=0; it<w1->amount_entity; it++)
		if (!entity_eq(w1->entities[it], w2->entities[it]))
			return FALSE;

	if (w1->amount_lines != w2->amount_lines) return FALSE;
	for (it=0;it<w1->amount_lines;it++) {
		if (w1->lines[it].x1 != w2->lines[it].x1) return FALSE;
		if (w1->lines[it].x2 != w2->lines[it].x2) return FALSE;
		if (w1->lines[it].y1 != w2->lines[it].y1) return FALSE;
		if (w1->lines[it].y2 != w2->lines[it].y2) return FALSE;
	}
	return TRUE;
}
/* easy getters/setters */
int world_get_amount_entity(core_world_t world) {
	world_t w = (world_t) world;
	return w->amount_entity;
}
double world_get_sizeX(core_world_t world) {
	return world->sizeX;
}
double world_get_sizeY(core_world_t world) {
	return world->sizeY;
}
void world_set_step_delay(core_world_t world, int step_delay) {
	world->step_delay = step_delay;
}


/* Add an entity to the world, and update entity's statut*/
void world_entity_add(core_world_t world, entity_t t) {
	world_t w = (world_t) world;
	w->entities = realloc(w->entities,sizeof(entity_t)*(w->amount_entity+1));
	w->entities[w->amount_entity++] = t;
	entity_set_world(t,w);
	entity_set_rank(t,w->amount_entity-1);
}

/* Functions related to turtle moving */
void world_line_add(core_world_t world, double x1, double y1, double x2, double y2){
	/* Locking: several turtles may be changing the world at the same time */
	world_t w = (world_t) world;
	static GStaticMutex my_mutex = G_STATIC_MUTEX_INIT;
	g_static_mutex_lock(&my_mutex);

	w->lines=realloc(w->lines,sizeof(struct s_line)*(w->amount_lines+1));
	w->lines[w->amount_lines].x1 = x1;
	w->lines[w->amount_lines].y1 = y1;
	w->lines[w->amount_lines].x2 = x2;
	w->lines[w->amount_lines].y2 = y2;
	w->amount_lines++; // Don't move it higher so that the drawer only see valid lines

	g_static_mutex_unlock(&my_mutex);

	if (w->mother.step_delay) {
		usleep(w->mother.step_delay*1000);
		//g_thread_yield();
	}
	world_ask_repaint(world);
}

/* Helper function to implement world_turtle_foreach  */
entity_t world_entity_geti(core_world_t world, int i) {
	world_t w = (world_t) world;
	if (i>=w->amount_entity)
		return NULL;
	return w->entities[i];
}

/* Functions related to drawing */
void world_redraw(core_world_t we, cairo_t *c,int sizeX,int sizeY) {
	int it;
	world_t w = (world_t)we;
	cairo_t *cr = (cairo_t*)c;
	double ratioX = sizeX/w->mother.sizeX;
	double ratioY = sizeY/w->mother.sizeY;
	double ratio = ratioX>ratioY?ratioY:ratioX;

    // grey background everywhere
    cairo_set_source_rgb( cr, 0, 0, .8 );
    cairo_fill(cr);

    // Adapt to the size of the world
	cairo_translate(cr, fabs(sizeX-ratio*w->mother.sizeX)/2., fabs(sizeY-ratio*w->mother.sizeY)/2.);
    cairo_scale(cr, ratio, ratio);

    // white background to the usefull surface
    cairo_set_source_rgb( cr, 1, 1, 1 );
    cairo_rectangle (cr, 0, 0, w->mother.sizeX, w->mother.sizeY);
    //cairo_set_source_rgba (cr, 1, 0, 0, 0.80);
    cairo_fill (cr);

    cairo_set_source_rgb( cr, 0, 0, 0 );

    /* Draw the lines */
    for (it=0;it<w->amount_lines;it++) {
        cairo_move_to(cr, w->lines[it].x1, w->lines[it].y1);
    	cairo_line_to(cr, w->lines[it].x2, w->lines[it].y2);
    	cairo_stroke(cr);
    }

    /* Draw the turtles */
    entity_t t;
    world_foreach_entity(we,it,t) {
    	cairo_move_to(cr, entity_get_x(t), entity_get_y(t));
    	cairo_rotate(cr,entity_get_heading(t));
    	cairo_rel_move_to(cr,10,0);
    	cairo_rel_line_to(cr,-10,-5);
    	cairo_rel_line_to(cr,0,10);
    	cairo_close_path(cr);
    	cairo_fill(cr);
    	cairo_rotate(cr,-entity_get_heading(t));
    }
}

int world_entity_get_pid(core_world_t world, int num)
{
  world_t w = (world_t) world;
  return entity_get_pid(w->entities[num]);
}

