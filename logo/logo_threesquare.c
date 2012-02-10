/* logo_threesquare.c: first steps in logo: please draw 3 squares */

#include <stdio.h>
#include "core/exercise.h"
#include "logo/logo.h"
#include "logo/world.h"
#include "logo/entity.h"


/* We use here the fact that C strings get automatically concatenated by the preprocessor
 * Ie, "aze" "rty" is changed into "azerty" by the preprocessor
 */
exercise_t logo_threesquare_create(void) {
	printf("Construction de l'exercise threesquare\n");
	world_t w = world_new(200,200);
	world_entity_add(w,entity_new(100,100,0.));
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
			"}", w);

	return res;
}

