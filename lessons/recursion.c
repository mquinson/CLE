/* recursion.c: main of the Recursion lesson */

#include <stdio.h>

#include "core/lesson.h"

/* Prototypes of the exercises composing this lesson */
exercise_t rec_spiral_create(void);
exercise_t rec_tree_create(void);

lesson_t lesson_main() {
	return lesson_new("Recursion",2,
			"Spirals", rec_spiral_create,
			"Trees", rec_tree_create);
}
