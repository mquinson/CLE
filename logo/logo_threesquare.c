/* logo_threesquare.c: first steps in logo: please draw 3 squares */
#include <stdlib.h>
#include <string.h>

#include "core/exercise.h"
#include "core/world.h"
#include "logo/logo.h"
#include "logo/world.h"
#include "logo/entity.h"


/* We use here the fact that C strings get automatically concatenated by the preprocessor
 * Ie, "aze" "rty" is changed into "azerty" by the preprocessor
 */
exercise_t logo_threesquare_create(void) {
	core_world_t w = world_new(200,200);
	//world_entity_add(w,entity_new(100,100,0.));
	world_entity_add(w,entity_new(100,40,0.));
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
			"}");
	exercise_add_world(res, w);
	
	core_world_t w2 = world_new(200,200);
	//world_entity_add(w2,entity_new(20,100,0.));
	world_entity_add(w2,entity_new(20,20,0.));
	exercise_add_world(res, w2);

	char** functionList = malloc(sizeof(char*)*1);
	functionList[0] = strdup("get_y");
	exercise_set_unauthorizedFunction(res, functionList, 1);
	return res;
}

