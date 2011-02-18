/*
 * exercice.h
 *
 *  Created on: 11 oct. 2010
 *      Author: mquinson
 */

#ifndef EXERCICE_H_
#define EXERCICE_H_

#include "logo/world.h"
#include "logo/turtle.h"

typedef struct s_exercise {
	const char*mission;
	const char*template;
	f_run_t solution;

	world_t w_init, w_curr, w_goal;
} *exercise_t;

/* Memory management */
exercise_t exercise_new(const char *mission, const char *template,
		f_run_t solution, world_t w);
void exercise_free(exercise_t e);

/* Demo-related functions */
void exercise_demo(exercise_t e);
int exercise_demo_is_running();
void exercise_demo_stop(exercise_t e);

// BEGINKILL
/* Running related functions */
void exercise_run(exercise_t e, char *source);
void exercise_run_stop(exercise_t e);
// ENDKILL


#endif /* EXERCICE_H_ */
