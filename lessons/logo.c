/* logo.c: main of the Logo lesson */

#include "core/lesson.h"

/* Prototypes of the exercises composing this lesson */
exercise_t logo_threesquare_create(void);
exercise_t logo_circle_create(void);
exercise_t logo_star_create(void);


lesson_t lesson_main() {
	return lesson_new("Logo",3,
			"Three squares", logo_threesquare_create,
			"Circle", logo_circle_create,
			"Three stars", logo_star_create);
}
