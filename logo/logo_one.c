#include "core/exercise.h"
#include "core/world.h"
#include "logo/logo.h"
#include "logo/world.h"
#include "logo/entity.h"

exercise_t logo_one_create(void) {
	core_world_t w = world_new(200,200);
	world_entity_add(w, entity_new(100,100,0.000000));
	exercise_t res = exercise_new(
"dessiner un carre de 40 de cote\n"
,
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"void square(int size) {\n"
"	char* str = malloc(sizeof(char)*10);\n"
"	printf(\"%c\\n\", str[11]);\n"
"}\n"
"void run (){\n"
"	square(40);\n"
"}\n"
,"\n"
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"void square(int size) {\n"
"	char* str = malloc(sizeof(char)*10);\n"
"	printf(\"%c\\n\", str[10]);\n"
"}\n"
"void run (){\n"
"	square(40);\n"
"}\n");
	exercise_add_world(res, w);
	return res;
}
