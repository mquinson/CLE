/*
 * turtle.c: functions related to the turtle
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "world.h"

const double EPSILON = .0000001;

struct s_turtle {
	world_t world;
	int rank;
	double x,y;
	double heading;
	int pen_is_down;
	char* binary;
	f_run_t run_fct;
};

turtle_t turtle_new(double x, double y, double heading) {
	turtle_t res = malloc(sizeof(struct s_turtle));
	res->world = NULL;
	res->x = x;
	res->y = y;
	res->heading = heading / 180. * M_PI;
	res->pen_is_down = 1;
	res->run_fct = NULL;
	res->binary=NULL;
	return res;
}

void turtle_free(turtle_t t) {
	free(t->binary);
	free(t);
}

int turtle_eq(turtle_t t1,turtle_t t2) {
	if (t1->x != t2->x) return 0;
	if (t1->y != t2->y) return 0;
	if (t1->heading != t2->heading) return 0;
	if (t1->pen_is_down != t2->pen_is_down) return 0;
	return 1;
}
turtle_t turtle_copy(turtle_t from) {
	turtle_t res = turtle_new(from->x,from->y, 0);
   /* We don't use the heading argument of turtle_new to avoid converting from radian to degree and back to radian [catched by Loic Poulain] */
        res->heading = from->heading;
	res->pen_is_down = from->pen_is_down;
	res->rank = from->rank;
	if(from->binary)
	  res->binary = strdup(from->binary);
	else
	  res->binary=NULL;
	return res;
}

/* easy getters */
double turtle_get_x(turtle_t t) {
	return t->x;
}
double turtle_get_y(turtle_t t) {
	return t->y;
}
double turtle_get_heading(turtle_t t) {
	return t->heading;
}

/* User API */
static void move_to(turtle_t t, double new_x, double new_y) {
	/* FIXME: account for clipping */

	if (t->pen_is_down)
		world_line_add(t->world,t->x,t->y,new_x,new_y);
	t->x = new_x;
	t->y = new_y;
}
void turtle_forward(turtle_t t, double steps) {
	move_to(t, t->x+steps*cos(t->heading), t->y+steps*sin(t->heading));
}
void turtle_backward(turtle_t t, double steps) {
	move_to(t, t->x+steps*cos(t->heading+M_PI), t->y+steps*sin(t->heading+M_PI));
}

void turtle_left(turtle_t t, double angle) {
	t->heading -= angle / 180. * M_PI;
}
void turtle_right(turtle_t t, double angle) {
	t->heading += angle / 180. * M_PI;
}

void turtle_pen_up(turtle_t t) {
	t->pen_is_down = 0;
}
void turtle_pen_down(turtle_t t) {
	t->pen_is_down = 1;
}

/* Exercise mechanism */
void turtle_set_world(turtle_t t,world_t w) {
	t->world = w;
}
void turtle_set_code(turtle_t t, f_run_t run) {
	t->run_fct = run;
}

void turtle_set_binary(turtle_t t, char* binary){
  if(t->binary)
    free(t->binary);
      t->binary = strdup(binary);
}

char* turtle_get_binary(turtle_t t){
  return t->binary;
}

/* Run a given turtle. Funcky prototype so that it can run as separate thread */
void* turtle_run(void *data) {
	turtle_t t = data;
	if (t->run_fct)
		(*t->run_fct)(t);
	else
		printf("Not running the NULL run function for that turtle");
	return NULL;
}
/* get the turtle ranking in its world (to be called from world_add_turtle only) */
void turtle_set_rank(turtle_t t, int rank) {
	t->rank = rank;
}
/* get the turtle ranking in its world (to be called from exercise) */
int turtle_get_rank(turtle_t t) {
	return t->rank;
}
