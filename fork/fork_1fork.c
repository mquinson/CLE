#include <stdio.h>
#include "fork/exercise.h"
#include "fork/fork.h"
#include "fork/world.h"
#include "fork/entity.h"

/* We use here the fact that C strings get automatically concatenated by the preprocessor
 * Ie, "aze" "rty" is changed into "azerty" by the preprocessor
 */
exercise_t fork_1fork_create(void) {
	printf("Construction de l'exercise 1fork\n");
	world_t w = world_new(200,200);
	world_entity_add(w,entity_new(100,100,0.));
	exercise_t res = exercise_new(
			"Dans cet exercice, vous devez écrire un fork .\n",

			"void run() {\n"
			"  fork();\n"
			"}",
			"void run() {\n"
			"  fork();\n"
			"}", w);

	return res;
}
