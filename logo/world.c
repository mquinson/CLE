/*
 * world.c: defines the data associated to a given world
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>

#include "world.h"
#include "world_view.h"

typedef struct s_line {
	double x1,y1,x2,y2;
} s_line_t;

struct s_world {
	double sizeX, sizeY;
	int step_delay; /* amount of milliseconds to sleep after each redraw */

	/* inhabitants */
	int amount_turtle;
	turtle_t *turtles;

	/* world state */
	int amount_lines;
	s_line_t *lines;
};

world_t world_new(double sizeX, double sizeY) {
	world_t res = malloc(sizeof(struct s_world));
	res->sizeX = sizeX;
	res->sizeY = sizeY;
	res->step_delay = 0;

	res->amount_turtle = 0;
	res->turtles = NULL;

	res->amount_lines = 0;
	res->lines = NULL;
	return res;
}
void world_free(world_t w) {
	int it;
	for (it=0;it<w->amount_turtle;it++)
		turtle_free(w->turtles[it]);
	free(w->turtles);
	free(w->lines);
	free(w);
}
world_t world_copy(world_t w) {
	int it;
	world_t res = world_new(w->sizeX,w->sizeY);

	res->amount_turtle = w->amount_turtle;
	res->turtles = malloc(sizeof(turtle_t)*w->amount_turtle);
	for (it=0;it<res->amount_turtle;it++) {
		res->turtles[it] = turtle_copy(w->turtles[it]);
		turtle_set_world(res->turtles[it], res);
	}

	res->amount_lines = w->amount_lines;
	res->lines = malloc(sizeof(struct s_line)*w->amount_lines);
	memcpy(res->lines,w->lines,sizeof(struct s_line)*w->amount_lines);
	return res;
}
int world_eq(world_t w1, world_t w2) {
	int it;

	if (w1->amount_turtle != w2->amount_turtle) return FALSE;
	for (it=0; it<w1->amount_turtle; it++)
		if (!turtle_eq(w1->turtles[it], w2->turtles[it]))
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
int world_get_amount_turtle(world_t w) {
	return w->amount_turtle;
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
void world_turtle_add(world_t w, turtle_t t) {
	w->turtles = realloc(w->turtles,sizeof(turtle_t)*(w->amount_turtle+1));
	w->turtles[w->amount_turtle++] = t;
	turtle_set_world(t,w);
	turtle_set_rank(t,w->amount_turtle-1);
}

/* Functions related to turtle moving */
void world_line_add(world_t w, double x1, double y1, double x2, double y2){
	/* Locking: several turtles may be changing the world at the same time */
	static GStaticMutex my_mutex = G_STATIC_MUTEX_INIT;
	g_static_mutex_lock(&my_mutex);

	w->lines=realloc(w->lines,sizeof(struct s_line)*(w->amount_lines+1));
	w->lines[w->amount_lines].x1 = x1;
	w->lines[w->amount_lines].y1 = y1;
	w->lines[w->amount_lines].x2 = x2;
	w->lines[w->amount_lines].y2 = y2;
	w->amount_lines++; // Don't move it higher so that the drawer only see valid lines

	g_static_mutex_unlock(&my_mutex);

	if (w->step_delay) {
		usleep(w->step_delay*1000);
		//g_thread_yield();
	}
	world_ask_repaint(w);
}

/* Helper function to implement world_turtle_foreach  */
turtle_t world_turtle_geti(world_t w, int i) {
	if (i>=w->amount_turtle)
		return NULL;
	return w->turtles[i];
}

/* Functions related to drawing */
void world_redraw(world_t w, cairo_t *cr,int sizeX,int sizeY) {
	int it;

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
        cairo_move_to(cr, w->lines[it].x1, w->lines[it].y1);
    	cairo_line_to(cr, w->lines[it].x2, w->lines[it].y2);
    	cairo_stroke(cr);
    }

    /* Draw the turtles */
    turtle_t t;
    world_foreach_turtle(w,it,t) {
    	cairo_move_to(cr, turtle_get_x(t), turtle_get_y(t));
    	cairo_rotate(cr,turtle_get_heading(t));
    	cairo_rel_move_to(cr,10,0);
    	cairo_rel_line_to(cr,-10,-5);
    	cairo_rel_line_to(cr,0,10);
    	cairo_close_path(cr);
    	cairo_fill(cr);
    	cairo_rotate(cr,-turtle_get_heading(t));
    }
}

