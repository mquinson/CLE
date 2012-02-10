/*
 * exercice.h
 *
 *  Created on: 11 oct. 2010
 *      Author: mquinson
 */

#ifndef EXERCICE_H_
#define EXERCICE_H_


typedef struct s_exercise {
	const char*mission;
	const char*template;
	const char*prof_solution;
	char* s_filename;
	char* binary;
	void *demo_runner_running;
	void *run_runner_running;
	void* w_init, *w_curr, *w_goal;
} *exercise_t;

/* Memory management */
void exercise_set_binary(exercise_t e, char* binary);
char* exercise_get_binary(exercise_t e);



#endif /* EXERCICE_H_ */
