/*
 * world.c: defines the data associated to a given world
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>

#include "fork/world.h"
#include "fork/world_view.h"

typedef struct s_line {
	double x1,y1,x2,y2;
	int color[3];
} s_line_t;

struct s_world {
	double sizeX, sizeY;
	int step_delay; /* amount of milliseconds to sleep after each redraw */

	/* inhabitants */
	int amount_entity;
	entity_t *entities;

	/* world state */
	int amount_lines;
	s_line_t *lines;
};

world_t world_new(double sizeX, double sizeY) {
	world_t res = malloc(sizeof(struct s_world));
	res->sizeX = sizeX;
	res->sizeY = sizeY;
	res->step_delay = 0;

	res->amount_entity = 0;
	res->entities = NULL;

	res->amount_lines = 0;
	res->lines = NULL;
	return res;
}
void world_free(world_t w) {
	int it;
	for (it=0;it<w->amount_entity;it++)
		entity_free(w->entities[it]);
	free(w->entities);
	free(w->lines);
	free(w);
}
world_t world_copy(world_t w) {
	int it;
	world_t res = world_new(w->sizeX,w->sizeY);

	res->amount_entity = w->amount_entity;
	res->entities = malloc(sizeof(entity_t)*w->amount_entity);
	for (it=0;it<res->amount_entity;it++) {
		res->entities[it] = entity_copy(w->entities[it]);
		entity_set_world(res->entities[it], res);
	}

	res->amount_lines = w->amount_lines;
	res->lines = malloc(sizeof(struct s_line)*w->amount_lines);
	memcpy(res->lines,w->lines,sizeof(struct s_line)*w->amount_lines);
	return res;
}

int tree_eq(tree_fork *s,tree_fork *p){
	int i;
	for(i=0;i<s->nb_son;i++){
		if(((tree_fork *)s->s[i])->nb_son!=((tree_fork *)p->s[i])->nb_son || !tree_eq((tree_fork *)s->s[i],(tree_fork *)p->s[i]))
			return FALSE;
	}
	return TRUE;
}

int world_eq(tree_fork *s,tree_fork *p,world_t wc, world_t wo) {
	if(!tree_eq(s,p)){
		return FALSE;
	}
	int i;
	for(i=0;i<wc->amount_entity;i++){
		if(entity_get_end(wc->entities[i]) != entity_get_end(wo->entities[i]))
			return FALSE;
	}
	return TRUE;
}
/* easy getters/setters */
int world_get_amount_entity(world_t w) {
	return w->amount_entity;
}
double world_get_sizeX(world_t w) {
	return w->sizeX;
}
double world_get_sizeY(world_t w) {
	return w->sizeY;
}
void world_set_step_delay(world_t w, int step_delay) {
	w->step_delay = step_delay;
}


/* */
void world_entity_add(world_t w, entity_t t) {
	w->entities = realloc(w->entities,sizeof(entity_t)*(w->amount_entity+1));
	w->entities[w->amount_entity++] = t;
	entity_set_world(t,w);
	entity_set_rank(t,w->amount_entity-1);
}

/* Functions related to turtle moving */
void world_line_add(world_t w, double x1, double y1, double x2, double y2,int *color){
	/* Locking: several turtles may be changing the world at the same time */
	static GStaticMutex my_mutex = G_STATIC_MUTEX_INIT;
	g_static_mutex_lock(&my_mutex);
	
	w->lines=realloc(w->lines,sizeof(struct s_line)*(w->amount_lines+1));
	w->lines[w->amount_lines].x1 = x1;
	w->lines[w->amount_lines].y1 = y1;
	w->lines[w->amount_lines].x2 = x2;
	w->lines[w->amount_lines].y2 = y2;
	w->lines[w->amount_lines].color[0] = color[0];
	w->lines[w->amount_lines].color[1] = color[1];
	w->lines[w->amount_lines].color[2] = color[2];
	w->amount_lines++; // Don't move it higher so that the drawer only see valid lines

	g_static_mutex_unlock(&my_mutex);

	if (w->step_delay) {
		usleep(w->step_delay*1000);
		//g_thread_yield();
	}
	world_ask_repaint(w);
}

/* Helper function to implement world_turtle_foreach  */
entity_t world_entity_geti(world_t w, int i) {
	if (i>=w->amount_entity)
		return NULL;
	return w->entities[i];
}

/* Functions related to drawing */
void world_redraw(void* we, void *c,int sizeX,int sizeY) {
	int it;
	world_t w = (world_t)we;
	cairo_t *cr = (cairo_t*)c;
	double ratioX = sizeX/w->sizeX;
	double ratioY = sizeY/w->sizeY;
	double ratio = ratioX>ratioY?ratioY:ratioX;

    // grey background everywhere
    cairo_set_source_rgb( cr, 0, 0, .8 );
    cairo_fill(cr);

    // Adapt to the size of the world
	cairo_translate(cr, fabs(sizeX-ratio*w->sizeX)/2., fabs(sizeY-ratio*w->sizeY)/2.);
    cairo_scale(cr, ratio, ratio);

    // white background to the usefull surface
    cairo_set_source_rgb( cr, 1, 1, 1 );
    cairo_rectangle (cr, 0, 0, w->sizeX, w->sizeY);
    //cairo_set_source_rgba (cr, 1, 0, 0, 0.80);
    cairo_fill (cr);

    cairo_set_source_rgb( cr, 0, 0, 0 );

    /* Draw the lines */
    for (it=0;it<w->amount_lines;it++) {
    	cairo_set_source_rgb( cr, w->lines[it].color[0], w->lines[it].color[1], w->lines[it].color[2] );
        cairo_move_to(cr, w->lines[it].x1, w->lines[it].y1);
    	cairo_line_to(cr, w->lines[it].x2, w->lines[it].y2);
    	cairo_stroke(cr);
    }

    /* Draw the turtles */
    entity_t t;
    world_foreach_entity(w,it,t) {
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

