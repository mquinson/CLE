/* logo_circle.c: a simple exercise asking to draw a circle*/

#include "core/exercise.h"
#include "logo/world.h"

void exo_circle_solution(turtle_t this) {
	int i;
	for (i=0;i<360;i++) {
		turtle_forward(this, 1);
		turtle_right(this, 1);
	}
}


exercise_t logo_circle_create(void) {
	world_t w = world_new(500,500);
	world_turtle_add(w,turtle_new(250,250,0));
	exercise_t res = exercise_new("blah","bli",exo_circle_solution, w);

	return res;
}

