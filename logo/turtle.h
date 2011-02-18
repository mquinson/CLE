/*
 * turtle.h
 */

#ifndef TURTLE_H_
#define TURTLE_H_

typedef struct s_turtle *turtle_t;
typedef struct s_world *world_t;

/* Memory management */
turtle_t turtle_new(double x, double y, double heading);
turtle_t turtle_copy(turtle_t from);
void turtle_free(turtle_t t);
int turtle_eq(turtle_t t1,turtle_t t2);

/* easy getters */
double turtle_get_x(turtle_t t);
double turtle_get_y(turtle_t t);
double turtle_get_heading(turtle_t t);

/* user API */
void turtle_forward(turtle_t t, double steps);
void turtle_backward(turtle_t w, double steps);
void turtle_left(turtle_t t, double angle);
void turtle_right(turtle_t t, double angle);
void turtle_pen_up(turtle_t t);
void turtle_pen_down(turtle_t t);

/* Exercise mechanism */
typedef void (*f_run_t)(turtle_t);

void turtle_set_world(turtle_t t,world_t w);
void turtle_set_code(turtle_t t, f_run_t run);
void* turtle_run(void* t);

/* get the turtle ranking in its world (to be called from world_add_turtle only) */
void turtle_set_rank(turtle_t t, int rank);
/* get the turtle ranking in its world (to be called from exercise mechanism) */
int turtle_get_rank(turtle_t t);


#endif /* TURTLE_H_ */
