 /*
  * logo/exercise.h : contains exercise's function of logo's world
  */
 
#ifndef LOGO_EXERCISE_H
#define LOGO_EXERCISE_H

#include "logo/world.h"
#include "logo/entity.h"
#include "core/exercise.h"

exercise_t exercise_new(const char *mission, const char *template,const char *prof_solution, void* wo);

void exercise_demo(void* e);
int exercise_demo_is_running(void* e);
void exercise_demo_stop(void* e);
void exercise_stop(void* l);


/* Running related functions */
void exercise_run(void* e, char *source);
void exercise_run_stop(void* e);

void exercise_free(exercise_t e);

#endif