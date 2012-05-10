 /*
  * logo/exercise.h : contains exercise's function of logo's world
  */
 
#ifndef LOGO_EXERCISE_H
#define LOGO_EXERCISE_H

#include "logo/world.h"
#include "logo/entity.h"
#include "core/lesson.h"
#include "core/exercise.h"
#include "core/world.h"

exercise_t exercise_new(const char *mission, const char *template,const char *prof_solution);

void exercise_demo(exercise_t e);
int exercise_demo_is_running(void* e);
//void exercise_demo_stop(void* e);
void exercise_stop(lesson_t l);


void exercise_add_world(exercise_t e, core_world_t world);



/* Running related functions */
void exercise_run(exercise_t e, char *source);
//void exercise_run_stop(exercise_t e);

void exercise_free(exercise_t e);

#endif