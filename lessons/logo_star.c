/* logo_star.c: a simple exercise asking to draw a circle*/

#include <stdio.h>
#include "core/exercise.h"
#include "logo/world.h"

const int branch_count = 5;

void branch(turtle_t t, int size) {
	int it;
	turtle_forward(t,size);
	turtle_right(t,360/branch_count);
	turtle_forward(t,size);
	for (it=0;it<2;it++)
		turtle_left(t,360/branch_count);
}
void star(turtle_t t,int size) {
	int it;
	for (it=0;it<branch_count;it++)
		branch(t,size);
}
void exo_star_solution(turtle_t t) {
	star(t,100);
	turtle_right(t,120);
	star(t,80);
	turtle_right(t,120);
	star(t,60);
	turtle_right(t,120);
}

exercise_t logo_star_create(void) {
	world_t w = world_new(500,500);
	world_turtle_add(w,turtle_new(250,250,0));
	return exercise_new(
			/* Mission */
			"Please draw 3 stars of size 100, 80 and 60, with 120 degrees between them.",
			/* template */
			"void run() {\n"
			"  // Please type your code here\n"
			"}",
			exo_star_solution, w);
}
