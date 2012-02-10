/* lesson.c: lesson handling functions */

// BEGINKILL
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <dlfcn.h>
// ENDKILL
#include <stdlib.h>

#include "UI/CLE.h"
#include "core/lesson.h"
#include "core/exercise.h"


lesson_t lesson_new(const char *title, int amount, ...) {
	lesson_t res = malloc(sizeof(struct s_lesson));
	res->title = title;
	res->amount= amount;
	res->exos = malloc(sizeof(exercise_desc)*amount);
    va_list ap;
    int it;
    va_start(ap,amount);
    for (it=0;it<amount;it++) {
    	res->exos[it].label = va_arg(ap,char*);
    	res->exos[it].exo_constructor = va_arg(ap, exercise_constructor_t);
    	res->exos[it].rank = it;
    }
    va_end(ap);
    res->e_curr=NULL;
    return res;
}

lesson_t lesson_from_file(char *filename) {
	lesson_t res = NULL;

	char *error = NULL;
	lesson_t (*lesson_loader)(void);
	void *module;
	/* Load the module */
	module = dlopen(filename, RTLD_NOW);
	if (!module) {
		fprintf (stderr, "Cannot load the module: %s\n", dlerror());
		goto error;
	}
	/* Find entry point */
	lesson_loader = dlsym(module, "lesson_main");
	error = dlerror();
	if (error != NULL) {
		perror(error);
		goto error;
	}
	
	/* Call that function */
	res = (*lesson_loader)();
	/* Save the module for further cleaning */
	res->exercise_demo = dlsym(module, "exercise_demo");
	error = dlerror();
	if (error != NULL) {
		perror(error);
		goto error;
	}
	res->exercise_run = dlsym(module, "exercise_run");
	error = dlerror();
	if (error != NULL) {
		perror(error);
		goto error;
	}
	res->exercise_stop = dlsym(module, "exercise_stop");
	error = dlerror();
	if (error != NULL) {
		perror(error);
		goto error;
	}
	res->exercise_free = dlsym(module, "exercise_free");
	error = dlerror();
	if (error != NULL) {
		perror(error);
		goto error;
	}
	res->world_repaint = dlsym(module, "world_redraw");
	error = dlerror();
	if (error != NULL) {
		perror(error);
		goto error;
	}
	res->dlmodule = module;
	error:

	free(filename);
	return res;
}

void lesson_free(lesson_t l) {
	if (!l)
		return;
	if (l->dlmodule)
		dlclose(l->dlmodule);
	if(l->e_curr)
	  (*(l->exercise_free))(l->e_curr);
	free(l->exos);
	free(l);
}

void lesson_set_exo(lesson_t l, int num) {
	if (num>=l->amount) {
		printf("No exercise number %d in lesson %s\n",num,l->title);
		return;
	}
	if (l->e_curr)
		(*(l->exercise_free))(l->e_curr);
	l->e_curr = l->exos[num].exo_constructor();
	CLE_exercise_has_changed();
}
