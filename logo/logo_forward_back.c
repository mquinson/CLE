#include "core/exercise.h"
#include "core/world.h"
#include "logo/logo.h"
#include "logo/world.h"
#include "logo/entity.h"

exercise_t logo_forward_back_create(void) {
	core_world_t w = world_new(200.000000,200.000000);
	world_entity_add(w, entity_new(100,100,0.000000));
	exercise_t res = exercise_new(
"avancer de 40 puis reculer\n"
," \n"
"  \n"
"void run (){\n"
" \n"
"}\n"
," \n"
"  \n"
"void run (){\n"
" \n"
"	forward(40);\n"
"	backward(40);\n"
" \n"
"}\n");
	exercise_add_world(res, w);
	return res;
}
