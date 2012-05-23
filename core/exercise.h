/*
 * exercice.h
 *
 *  Created on: 11 oct. 2010
 *      Author: mquinson
 */

#ifndef EXERCICE_H_
#define EXERCICE_H_

#include <gtk/gtk.h>

typedef struct s_exercise *exercise_t;

#include "core/world.h"
#define MAX_NB_LOG_ERRORS 10

typedef struct {
  int line;
  char* msg;
} log_error_s, *log_error;



typedef struct {
  char* line;
  char* source_name;
  int world_numero;
  int source_limit;
  int num_error;
  char last_error_message[1024];
  int error_stack;
  int adress_stack;
} valgrind_log_s;



struct log_listener_data{
  int pipe;
  valgrind_log_s* valgrind_log;
};


struct s_exercise {
	const char *mission;
	/* This is the template which are print for the student*/
	const char *template;
	/* This contains the solution's code*/
	const char *prof_solution;
	/* This the name of the binary's solution*/
	char* s_filename;
	char* binary; /* Current binary of student proposition*/
	char** unauthorizedFunction; /* Name of function which used are unauthorized for the exercise resolution */
	int unauthorizedNumber; /* Number of function which are prohibited */
	GMutex *demo_runner_running;
	GMutex *run_runner_running;
	core_world_t *w_init, *w_curr, *w_goal;
	int worldAmount;
	
	void (* exercise_free)(exercise_t);
};


void exercise_set_binary(exercise_t e, char* binary);
char* exercise_get_binary(exercise_t e);

/*Set the list give by parameter to the list of function prohibited*/
void exercise_set_unauthorizedFunction(exercise_t e, char** functionNameList, int listSize);

/* Function which process lines to recognize gcc log to add mark and print*/
void display_compilation_errors(char* message);

/* Function which process lines to recognize valgrinds log to add mark and print*/
int display_valgrind_errors(valgrind_log_s *data);

/* This function print the define's preprocessor directive used to prohibited function usage*/
void exercise_print_unauthorized(exercise_t e, int fd);

/*Print a line's preprocessor directive by using line and filename passed */
void print_line_prepocessor_instruction(int fd, int line, char* filename);

/*Generate a temporary sourcefile name by concatenantion of userside, and source.
 * The function print line's preprocessor directive and unauthorized function if we generate student file
 * Passing NULL as sources means that we used e->prof_solution for sources and compil for teacher */
char* generate_temporary_sourcefile_header(exercise_t e, const char* userside, const char* source);

/* Thread use to manage execution's log*/
void *exercise_run_log_listener(void *d);



#endif /* EXERCICE_H_ */
