 /*
  * fork/exercise.h : contains exercise's function of logo's world
  */
 
#ifndef FORK_EXERCISE_H
#define FORK_EXERCISE_H

#include <math.h>

#include "fork/teststrace.h"
#include "fork/entity.h"
#include "core/exercise.h"


exercise_t exercise_new(const char *mission, const char *template,const char *prof_solution, void* wo);

void exercise_demo(void* e);
int exercise_demo_is_running(void* e);
void exercise_demo_stop(void* e);
void exercise_stop(void* l);

void *entity_fork_run(void *pr);
 
 /**
Structure
*/
typedef struct{
	struct tree_fork *f;
	struct tree_fork **s;
	int nb_son;
	int pos;
	int pid;
}tree_fork;

typedef struct{
	int *list_pid;
	entity_t *list_t;
	tree_fork *racine;
	tree_fork **list_nodes_tree;
	int nb_t;
	int fd;	
}param_runner;

tree_fork *allocate_tree_fork(tree_fork *tff);
void tree_fork_add_son(tree_fork *tf,param_runner *pr,int pid);

void free_tree_fork(tree_fork *tf);

int tree_fork_nb_branch_up(tree_fork *tf);

param_runner *allocate_param_runner(entity_t t,int fd);

void free_param_runner(param_runner *pr);

void add_entity(param_runner *pr,int pos_f,int pid_s);

int find_pos_pid(int *list_pid,int size,int pid);

void stop_zombies_son(param_runner *pr,int pos_f,int *color);

/* Running related functions */
void exercise_run(void* e, char *source);
void exercise_run_stop(void* e);

void exercise_free(exercise_t e);

#endif
