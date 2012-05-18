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
} valgrind_log_s;


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
	
	int gcc_report_new; //
	log_error* gcc_logs;
	int nb_logs;
	char* gcc_report;
	
	void (* exercise_free)(exercise_t);
};


/* Memory management */
void exercise_set_binary(exercise_t e, char* binary);
char* exercise_get_binary(exercise_t e);

void exercise_set_unauthorizedFunction(exercise_t e, char** functionNameList, int listSize);
char* exercice_get_log(exercise_t e, int line);
void exercice_add_log(exercise_t e, int line, char* msg);
void exercise_clear_log(exercise_t e);

void display_compilation_errors(exercise_t e);
int display_valgrind_errors(valgrind_log_s *data);
void exercise_append_gcc_log(exercise_t e,char* log, int size);

void exercise_print_unauthorized(exercise_t e, int fd);
void print_line_prepocessor_instruction(int fd, int line, char* filename);
char* generate_temporary_sourcefile_header(exercise_t e, const char* userside, const char* source);



#endif /* EXERCICE_H_ */
