/*
 * entity.h
 */

#ifndef TURTLE_H_
#define TURTLE_H_

typedef struct s_entity *entity_t;
typedef struct s_world *world_t;

/* Memory management */
entity_t entity_new(double x, double y, double heading);
entity_t entity_copy(entity_t from);
void entity_free(entity_t t);
int entity_eq(entity_t t1,entity_t t2);

/* easy getters */
double entity_get_x(entity_t t);
double entity_get_y(entity_t t);
double entity_get_heading(entity_t t);

/* user API */
void entity_forward(entity_t t, double steps);
void entity_backward(entity_t w, double steps);
void entity_left(entity_t t, double angle);
void entity_right(entity_t t, double angle);
void entity_pen_up(entity_t t);
void entity_pen_down(entity_t t);

/* Exercise mechanism */
typedef void (*f_run_t)(entity_t);

void entity_set_world(entity_t t,world_t w);
void entity_set_code(entity_t t, f_run_t run);
void entity_set_binary(entity_t t, char* binary);
char* entity_get_binary(entity_t y);
void* entity_run(void* t);

/* get the entity ranking in its world (to be called from world_add_entity only) */
void entity_set_rank(entity_t t, int rank);
/* get the entity ranking in its world (to be called from exercise mechanism) */
int entity_get_rank(entity_t t);


#endif /* TURTLE_H_ */
