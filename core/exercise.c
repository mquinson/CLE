/*
 * exercice.c: the mecanic to declare an exercice
 */
#include <stdlib.h>
#include <string.h>

#include "exercise.h"
#include "lesson.h"


void exercise_set_binary(exercise_t e, char* binary){
	if(e->binary)
	  free(e->binary);
	e->binary = strdup(binary);
}


char* exercise_get_binary(exercise_t e){
	return e->binary;
}