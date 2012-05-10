#include "core/exercise.h"
#include "core/world.h"
#include "logo/logo.h"
#include "logo/world.h"
#include "logo/entity.h"

exercise_t logo_square_create(void) {
	core_world_t w = world_new(200,200);
	world_entity_add(w, entity_new(100,100,0.000000));
	exercise_t res = exercise_new(
"lalalalalalalalalalallalalalalaallal la\n"
" \n"
"on rajout un truc pour l'honneur\n"
,"\n"
"void square(int size) {\n"
"	int i;\n"
"	for (i=0;i<4;i++) {\n"
"		forward(size);\n"
"		right(90);\n"
"	}\n"
"}\n"
"void run (){\n"
"	square(90);\n"
"	square(60);\n"
"	square(30);\n"
"}\n"
"\n"
,"\n"
"\n"
"void square(int size) {\n"
"	int i;\n"
"	for (i=0;i<4;i++) {\n"
"		forward(size);\n"
"		right(90);\n"
"	}\n"
"}\n"
"void run (){\n"
"	square(90);\n"
"	square(60);\n"
"	square(30);\n"
"}\n"
"\n"
"\n"
"\n");
	exercise_add_world(res, w);
	return res;
}
