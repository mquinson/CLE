/*
 * exos.h: list of all known exos
 */

#ifndef LOGO_H_
#define LOGO_H_
#include "core/lesson.h"
#include "logo/world.h"

lesson_t lesson_main(void);

exercise_t exercise_new(const char *mission, const char *template,const char *prof_solution, void* wo);

void exercise_demo(void* e);
int exercise_demo_is_running();
void exercise_demo_stop(void* e);
void exercise_stop(void* l);


/* Running related functions */
void exercise_run(void* e, char *source);
void exercise_run_stop(void* e);

#endif /* LOGO_H_ */
