/* logo_threesquare.c: first steps in logo: please draw 3 squares */

#include "core/exercise.h"
#include "logo/world.h"

void square(turtle_t this, int size) {
	int i;
	for (i=0;i<4;i++) {
		turtle_forward(this, size);
		turtle_right(this, 90);
	}
}

void exo_threesquare_solution(turtle_t this) {
	square(this, 90);
	square(this, 60);
	square(this, 30);
}

/* We use here the fact that C strings get automatically concatenated by the preprocessor
 * Ie, "aze" "rty" is changed into "azerty" by the preprocessor
 */
exercise_t logo_threesquare_create(void) {
	world_t w = world_new(200,200);
	world_turtle_add(w,turtle_new(100,100,0.));
	exercise_t res = exercise_new(
			"Dans cet exercice, vous devez faire trois carrés successifs.\n"
			"Leur taille en pas sera 30, 60 et 90.",

			"void square(int size) {\n"
			"  int i;\n"
			"  for (i=0;i<4;i++) {\n"
			"     forward(size);\n"
			"     right(90);\n"
			"  }\n"
			"}\n"
			"void run() {\n"
			"  square(90);\n"
			"  square(60);\n"
			"  square(30);\n"
			"}",
			exo_threesquare_solution, w);

	return res;
}

