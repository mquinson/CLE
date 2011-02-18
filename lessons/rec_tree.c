/* rec_tree.c: Drawing trees */

#include "core/exercise.h"
#include "logo/turtle.h"

static void tree_rec(turtle_t t, int steps, double length, double angle, double shrink){
	if (steps <= 0) {
		/* do nothing */
	} else {
		turtle_forward(t,length);
		turtle_right(t,angle);
		tree_rec(t,steps-1, length*shrink, angle, shrink);
		turtle_left(t,2*angle);
		tree_rec(t,steps-1, length*shrink, angle, shrink);
		turtle_right(t,angle);
		turtle_backward(t,length);
	}
}

void rec_tree_sol(turtle_t t) {
	tree_rec(t,7,75.,30.,.8);
	// These should go into other worlds when you're multi-world-ready
	// You'll also need to add parameters to the world. Check how it's done in JLM
	//tree_rec(t,7,75.,15.,.8);
	//tree_rec(t,10,80.,45.,.7);
	//tree_rec(t,100.,90.,.68);
}

exercise_t rec_tree_create(void) {
	world_t w = world_new(400,400);
	world_turtle_add(w,turtle_new(50, 200, 0));
	return exercise_new(
			/* Mission */
			"Please reproduce the tree.",
			/* template */
			"void run() {\n"
			"  // Please type your code here\n"
			"}",
			rec_tree_sol, w);
}

