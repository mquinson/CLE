/* rec_spiral.c: Drawing spirals */

#include "core/exercise.h"
#include "logo/turtle.h"

static void spiral_rec(turtle_t t, int steps, int angle, int length, int increment)     {
	if (steps <= 0) {
		// do nothing
	} else {
		turtle_forward(t,length);
		turtle_left(t,angle);
		spiral_rec(t,steps-1, angle, length+increment, increment);
	}
}

void rec_spiral_sol(turtle_t t) {
	spiral_rec(t,50,120,0,6);
	// These should go into other worlds when you're multi-world-ready
	// You'll also need to add parameters to the world. Check how it's done in JLM
	//spiral_rec(t,100,90,0,3);
	//spiral_rec(t,70,72,0,2);
	//spiral_rec(t,25,60,0,6);
}

exercise_t rec_spiral_create(void) {
	world_t w = world_new(400,400);
	world_turtle_add(w,turtle_new(200,200,0));
	return exercise_new(
			/* Mission */
			"Please reproduce the spiral.",
			/* template */
			"void run() {\n"
			"  // Please type your code here\n"
			"}",
			rec_spiral_sol, w);
}

