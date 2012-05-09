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
	core_world_t w_init, w_curr, w_goal;
	
	void (* exercise_free)(exercise_t);
};

/* Memory management */
void exercise_set_binary(exercise_t e, char* binary);
char* exercise_get_binary(exercise_t e);

void exercise_set_unauthorizedFunction(exercise_t e, char** functionNameList, int listSize);



#endif /* EXERCICE_H_ */
