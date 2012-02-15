#include "core/exercise.h"
#include "logo/logo.h"
#include "logo/world.h"
#include "logo/entity.h"

exercise_t logo_Forward_Back_create(void) {
	world_t w = world_new(200,200);
	world_entity_add(w, entity_new(100,100,0.000000));
	exercise_t res = exercise_new(
"avancer de 40 puis reculer\n"
,"\n"
"\n"
,"\n"
"\n"
"\n"
"void run (){\n"
"	forward(40);\n"
"	backward(40);\n"
"}\n"
"\n"
"\n"
, w);
	return res;
}
