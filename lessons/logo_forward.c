#include <stdio.h>
#include "core/exercise.h"
#include "lessons/logo.h"
#include "logo/world.h"
#include "logo/turtle.h"

/* We use here the fact that C strings get automatically concatenated by the preprocessor
 * Ie, "aze" "rty" is changed into "azerty" by the preprocessor
 */
exercise_t logo_forward_create(void) {
	world_t w = world_new(200,200);
	world_turtle_add(w,turtle_new(100,100,0.));
	exercise_t res = exercise_new(
			"Dans cet exercice, vous devez avancer de 30.\n",

			"void run() {\n"
			"  forward(30);\n"
			"}",
			"void run() {\n"
			"  forward(30);\n"
			"}", w);

	return res;
}