#include "core/exercise.h"
#include "core/world.h"
#include "logo/logo.h"
#include "logo/world.h"
#include "logo/entity.h"

/* We use here the fact that C strings get automatically concatenated by the preprocessor
 * Ie, "aze" "rty" is changed into "azerty" by the preprocessor
 */
exercise_t logo_forward_create(void) {
	core_world_t w = world_new(200,200);
	world_entity_add(w,entity_new(100,100,0.));
	exercise_t res = exercise_new(
			"<b>Dans cet exercice</b>, vous devez avancer de 30.\n",

			"void run() {\n"
			"  forward(30);\n"
			"}",
			"void run() {\n"
			"  forward(30);\n"
			"}", w);

	return res;
}