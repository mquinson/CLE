/*
 * entity.c: functions related to the turtle
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <math.h>
#include <string.h>
#include "logo/world.h"
#include "UI/CLE.h"

const double EPSILON = .0000001;

struct s_entity {
	world_t world;
	int rank;
	double x,y;
	double heading;
	int pen_is_down;
	char* binary;  /*Contains the name of the binary which the turtle launch*/
	/*Contains the pid number of the processus which is launch for the turtle*/
	pid_t pid;
	f_run_t run_fct;
};

entity_t entity_new(double x, double y, double heading) {
	entity_t res = malloc(sizeof(struct s_entity));
	res->world = NULL;
	res->x = x;
	res->y = y;
	res->heading = heading / 180. * M_PI;
	res->pen_is_down = 1;
	res->run_fct = NULL;
	res->binary=NULL;
	res->pid = 0;
	return res;
}

void entity_free(entity_t t) {
	free(t->binary);
	free(t);
}

int entity_eq(entity_t t1,entity_t t2) {
	if (t1->x != t2->x) return 0;
	if (t1->y != t2->y) return 0;
	if (t1->heading != t2->heading) return 0;
	if (t1->pen_is_down != t2->pen_is_down) return 0;
	return 1;
}
entity_t entity_copy(entity_t from) {
	entity_t res = entity_new(from->x,from->y, 0);
   /* We don't use the heading argument of entity_new to avoid converting from radian to degree and back to radian [catched by Loic Poulain] */
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
double entity_get_x(entity_t t) {
	return t->x;
}
double entity_get_y(entity_t t) {
	return t->y;
}
double entity_get_heading(entity_t t) {
	return t->heading;
}

/* User API */
static void move_to(entity_t t, double new_x, double new_y) {
	/* FIXME: account for clipping */

	if (t->pen_is_down)
		world_line_add((core_world_t)t->world,t->x,t->y,new_x,new_y);
	t->x = new_x;
	t->y = new_y;
}
void entity_forward(entity_t t, double steps) {
	move_to(t, t->x+steps*cos(t->heading), t->y+steps*sin(t->heading));
}
void entity_backward(entity_t t, double steps) {
	move_to(t, t->x+steps*cos(t->heading+M_PI), t->y+steps*sin(t->heading+M_PI));
}

void entity_left(entity_t t, double angle) {
	t->heading -= angle / 180. * M_PI;
}
void entity_right(entity_t t, double angle) {
	t->heading += angle / 180. * M_PI;
}

void entity_pen_up(entity_t t) {
	t->pen_is_down = 0;
}
void entity_pen_down(entity_t t) {
	t->pen_is_down = 1;
}

/* Exercise mechanism */
void entity_set_world(entity_t t,world_t w) {
	t->world = w;
}

void entity_set_code(entity_t t, f_run_t run) {
	t->run_fct = run;
}

void entity_set_pid(entity_t t, pid_t pid) {
	t->pid = pid;
}

int entity_get_pid(entity_t e) {
	return e->pid;
}

void entity_stop_execution(entity_t t, pid_t pid) {
	if(t->pid>0)
	  kill(t->pid, SIGTERM);
}



void entity_set_binary(entity_t t, char* binary){
  if(t->binary)
    free(t->binary);
      t->binary = strdup(binary);
}

char* entity_get_binary(entity_t t){
  return t->binary;
}

/* Run a given entity. Funcky prototype so that it can run as separate thread */
void* entity_run(void *data) {
	entity_t t = data;
	if (t->run_fct)
		(*t->run_fct)(t);
	else
		printf("Not running the NULL run function for that entity\n");
	return NULL;
}

/* get the entity ranking in its world (to be called from world_add_entity only) */
void entity_set_rank(entity_t t, int rank) {
	t->rank = rank;
}
/* get the entity ranking in its world (to be called from exercise) */
int entity_get_rank(entity_t t) {
	return t->rank;
}

char* entity_get_description(entity_t t)
{
    char res[100];
    int world_number =-1;
    int entity_number =-1;
    int i;
    entity_t tmp;
    for(i=0; i<global_data->lesson->e_curr->worldAmount; ++i)
    {
      if(global_data->lesson->e_curr->w_curr[i] == (core_world_t)t->world)
      {
	world_number=i;
	break;
      }
    }
    
    world_foreach_entity((core_world_t)t->world, i, tmp)
      if(tmp == t)
      {
	entity_number = i;
	break;
      }
    
    sprintf(res, "[entity %d : world %d ]", entity_number, world_number);
    return strdup(res);
}

