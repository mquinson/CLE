

#include <string.h> // KILLIT
#include <stdlib.h>
#include <stdio.h>  // KILLIT
#include <unistd.h> // KILLIT
#include <sys/types.h> // KILLIT
#include <sys/wait.h> // KILLIT
#include <errno.h>


#include "core/exercise.h"
#include "core/lesson.h"
#include "logo/entity_userside.h"
#include "logo/entity.h"
#include "logo/exercise.h"
#include "UI/CLE.h"


void* exercise_demo_runner(void *exo);
void exercise_run_one_entity(entity_t t);
void exercise_run_stop(void* ex);


static GMutex* demo_runner_running;
static GMutex* run_runner_running;
static char *binary; // The name of the binary
static pid_t*pids;

/* Function launched in a separate thread to run the demo without locking the UI
 * It is in charge of starting a thread for each entity to animate, and wait for their completion
 */
void* exercise_demo_runner(void *exo) {
	int it;
	entity_t t;
	
	//static Gthread* = NULL;
	exercise_t e = exo;
	demo_runner_running = (GMutex *)e->demo_runner_running;
	if(e->s_filename==NULL)
	{
	  char *filename= strdup("/tmp/CLEs.XXXXXX");
	  char* binary_t = strdup("/tmp/CLEb.XXXXXX");
	  int ignored =mkstemp(binary_t); // avoid the useless warning on mktemp dangerousness
	  close(ignored);
	  int fd = mkstemp(filename);

	  /* Copy stringified version of userside to file */
	  char *p = userside;
	  /*"#include <stdio.h>\n"
			  "double get_x(void);\n"
			  "double get_y(void);\n"
			  "double get_heading(void);\n"
			  "void forward(double steps);\n"
			  "void backward(double steps);\n"
			  "void left(double angle);\n"
			  "void right(double angle);\n"
			  "void pen_up(void);\n"
			  "void pen_down(void);\n"
			  "#line 1 \"yourcode\"\n";*/
	  int todo = strlen(p);
	  while (todo>0)
		  todo -= write(fd,p,todo);

	  p = strdup(e->prof_solution);
	  todo = strlen(e->prof_solution);
	  while (todo>0)
		  todo -= write(fd,p,todo);

	  close(fd);

	  /* Fork a process to compile it */
	  int status;
	  int gcc[2];
	  pipe(gcc);
	  int pid = fork();
	  switch (pid) {
	  case -1:
		  CLE_log_append(strdup("Error while forking the compiler"));
		  CLE_log_append(strdup(strerror(errno)));
		  break;
	  case 0: // Child: run gcc
		  close(gcc[0]);
		  close(1);
		  close(2);
		  dup2(gcc[1],1);
		  dup2(gcc[1],2);
		  execlp("gcc","gcc","-g","-x","c",filename, "-o",binary_t,"-Wall",NULL);
	  default: // Father: listen what gcc has to tell
		  close(gcc[1]);
		  char buff[1024];
		  int got;
		  while ((got = read(gcc[0],&buff,1023))>0) {
			  buff[got] = '\0';
			  CLE_log_append(strdup(buff));
		  }
		  waitpid(pid,&status,0);
	  }
	  e->s_filename = binary_t;
	  unlink(filename);
	  free(filename);
	}
	
	GThread **runners = malloc(sizeof(GThread*)*(world_get_amount_entity(e->w_goal)));

	
	
	/* Reset the goal world */
	world_free(e->w_goal);
	e->w_goal = world_copy(e->w_init);
	world_set_step_delay(e->w_goal,50); /* FIXME: should be configurable from UI */
	world_foreach_entity(e->w_goal,it,t){
	entity_set_code(t,exercise_run_one_entity);
	entity_set_binary(t, e->s_filename);
	}
	
	if (pids)
		free(pids);
	pids=malloc (sizeof(pid_t)*world_get_amount_entity(e->w_goal));

	/* Launch all the runners */
	world_foreach_entity(e->w_goal,it,t)
	runners[it] = g_thread_create(entity_run,t,1,NULL);

	/* Wait the end of all runners */
	world_foreach_entity(e->w_goal,it,t)
	g_thread_join(runners[it]);

	/* Re-enable the demo running button */
	world_set_step_delay(e->w_goal,0);
	g_mutex_unlock(demo_runner_running);
	return NULL;
}

void exercise_demo(void* exo) {
	exercise_t e = exo;
	demo_runner_running = (GMutex *)e->demo_runner_running;
	int res = g_mutex_trylock(demo_runner_running);
	if (!res) {
		printf("Not restarting the demo (it's already running)\n");
		return;
	}

	/* Launch the demo (in a separate thread waiting for the completion of all entities before re-enabling the button) */
	g_thread_create(exercise_demo_runner,e,0,NULL);
}

void exercise_stop(void* lesson)
{
  lesson_t l = lesson;
  if(exercise_demo_is_running(l->e_curr))
  {
      exercise_demo_stop(l->e_curr);
  }
  else
      exercise_run_stop(l->e_curr);
}


int exercise_demo_is_running(void* exo) {
	exercise_t e = exo;
	demo_runner_running = (GMutex *)e->demo_runner_running;
	int res = g_mutex_trylock(demo_runner_running);
	if (res)
		g_mutex_unlock(demo_runner_running);

	printf("Demo is %srunning\n",(!res?"":"NOT "));
	return !res;
}
void exercise_demo_stop(void* ex) {
	/* Actually, we don't stop the demo since we *need* it to compute the goal world.
	 * Instead, we stop the animation and get it computing as fast as possible.
	 * That's not what we want to do for exercise_run_stop (or whatever you call it). Instead we want to kill the child doing it.
	 */
	exercise_t e = ex;
	world_set_step_delay(e->w_goal,0);
}


/* Small thread in charge of listening everything that the user's entity printf()s,
 * and add it to the log console */
void *exercise_run_log_listener(void *pipe) {
	int fd = *(int*)pipe;
    char buff[1024];
    int got;
    while ((got = read(fd,&buff,1023))>0) {
      buff[got] = '\0';
      CLE_log_append(strdup(buff));
    }
    return NULL;
}


/* Function in charge of running a particular entity */
void exercise_run_one_entity(entity_t t) {
	//printf("Run entity %p\n",t);
	int f2c[2]; // father to child
	int c2f[2]; // child to father
	int cmd_f2c[2]; // father to child
	int cmd_c2f[2]; // child to father
	pipe(f2c);
	pipe(c2f);
	pipe(cmd_f2c);
	pipe(cmd_c2f);
	int pid = fork();
	int status;
	if (pid < 0) {
		CLE_log_append(strdup("Error while forking the entity runner"));
		CLE_log_append(strdup(strerror(errno)));
		return;
	}
	if (pid == 0) { // Child: run the entity
		close(f2c[1]);
		close(c2f[0]);
		close(cmd_f2c[1]);
		close(cmd_c2f[0]);
		close(0);
		close(1);
		close(2);
		dup2(f2c[0],0);
		dup2(c2f[1],1);
		dup2(c2f[1],2);
		dup2(cmd_f2c[0],3);
		dup2(cmd_c2f[1],4);
		close(f2c[0]);
		close(c2f[1]);
		close(cmd_f2c[0]);
		close(cmd_c2f[1]);
		execl(entity_get_binary(t),"child",NULL);
		printf("OUCH execl failed!\n");
		exit(2);
	}// Father: listen what the child has to tell

	printf("Turtle rank %d running child %s\n",entity_get_rank(t),binary);
	pids[entity_get_rank(t)] = pid;
	close(f2c[0]);
	close(c2f[1]);
	close(cmd_f2c[0]);
	close(cmd_c2f[1]);

	/* Main interaction loop with the child */
	GThread * log_listener = g_thread_create(exercise_run_log_listener,&(c2f[0]),1,NULL);
	FILE *fromchild=fdopen(cmd_c2f[0],"r");
	FILE *tochild=fdopen(cmd_f2c[1],"w");
	char *buff=NULL;
	size_t len=0;
	int got;
	while ((got = getline(&buff, &len, fromchild)) != -1) {
		int cmd=-1;
		double arg=-1;
		sscanf(buff, "%d %lf", &cmd,&arg);
		switch(cmd){
		case 100:
			fprintf(tochild,"%lf\n",entity_get_x(t));
			break;
		case 101:
			fprintf(tochild,"%lf\n",entity_get_y(t));
			break;
		case 102:
			fprintf(tochild,"%lf\n",entity_get_heading(t));
			break;
		case 103:
			entity_forward(t,arg);
			break;
		case 104:
			entity_backward(t,arg);
			break;
		case 105:
			entity_left(t,arg);
			break;
		case 106:
			entity_right(t,arg);
			break;
		case 107:
			entity_pen_up(t);
			break;
		case 108:
			entity_pen_down(t);
			break;
		default:
			CLE_log_append(strdup("Oops, unknown order from child: "));
			CLE_log_append(strdup(buff));
		}
	}
	/* the child is done. Cleaning */
	fclose(fromchild);
	fclose(tochild);
	waitpid(pid,&status,0);
	if (WIFSIGNALED(status)) {
		if (WTERMSIG(status)==SIGTERM) {
			CLE_log_append(strdup("(execution aborded on user request)\n"));
		} else if (WTERMSIG(status)==SIGSEGV) {
			CLE_log_append(strdup("The your code in a SIGSEGV. Check your pointers.\n"));
		} else {
			CLE_log_append(strdup("The child running the entity got signaled!\n"));
		}
	} else if (WIFEXITED(status) && WEXITSTATUS(status) !=0) {
		CLE_log_append(strdup("The child running the entity returned with abnormal return value!\n"));
	}
	g_thread_join(log_listener);
	//printf("Done running entity %p\n",t);
}
/* Function launched in a separate thread to run the demo without locking the UI
 * It is in charge of starting a thread for each entity to animate, and wait for their completion
 */
void* exercise_run_runner(void *exo) {
	int it;
	entity_t t;

	exercise_t e = exo;
	run_runner_running = (GMutex *)e->run_runner_running;
	GThread **runners = malloc(sizeof(GThread*)*(world_get_amount_entity(e->w_curr)));

	/* Reset the goal world */
	world_free(e->w_curr);
	e->w_curr = world_copy(e->w_init);
	world_set_step_delay(e->w_curr,50); /* FIXME: should be configurable from UI */
	world_foreach_entity(e->w_curr,it,t){
	entity_set_code(t,exercise_run_one_entity);
	entity_set_binary(t, exercise_get_binary(e));
	}

	if (pids)
		free(pids);
	pids=malloc (sizeof(pid_t)*world_get_amount_entity(e->w_curr));

	printf("Launch all entities\n");
	/* Launch all the runners */
	world_foreach_entity(e->w_curr,it,t)
	runners[it] = g_thread_create(entity_run,t,1,NULL);

	/* Wait the end of all runners */
	world_foreach_entity(e->w_curr,it,t)
	g_thread_join(runners[it]);

	/* Re-enable the run running button */
	world_set_step_delay(e->w_curr,0);
	printf("End of execution\n");

	if (world_eq(e->w_curr,e->w_goal))
		CLE_dialog_success();
	else
		CLE_dialog_failure("Your world differs from the goal");
	g_mutex_unlock(run_runner_running);
	free(pids);
	pids=NULL;
	unlink(binary);
	free(binary);
	return NULL;


}
void exercise_run_stop(void* ex) {
	/* actually kill all the processes */
	int it;
	exercise_t e = ex;
	if (pids)
		for (it=0;it< world_get_amount_entity(e->w_curr); it++)
			kill(pids[it],SIGTERM);
}


void exercise_run(void* ex, char *source) {
	// BEGINKILL
	int status; // test whether they were compilation errors
	exercise_t e = ex;
	
	run_runner_running = (GMutex *)e->run_runner_running;
	int res = g_mutex_trylock(run_runner_running);
	
	if (!res) {
		printf("Not restarting the execution (it's already running)\n");
		return;
	}

	/* clear the logs */
	CLE_log_clear();

	/* create 2 filenames */
	char *filename= strdup("/tmp/CLEs.XXXXXX");
	binary = strdup("/tmp/CLEb.XXXXXX");
	int ignored =mkstemp(binary); // avoid the useless warning on mktemp dangerousness
	close(ignored);
	int fd = mkstemp(filename);

	/* Copy stringified version of userside to file */
	char *p = userside;
	/*"#include <stdio.h>\n"
			"double get_x(void);\n"
			"double get_y(void);\n"
			"double get_heading(void);\n"
			"void forward(double steps);\n"
			"void backward(double steps);\n"
			"void left(double angle);\n"
			"void right(double angle);\n"
			"void pen_up(void);\n"
			"void pen_down(void);\n"
			"#line 1 \"yourcode\"\n";*/
	int todo = strlen(p);
	while (todo>0)
		todo -= write(fd,p,todo);

	p = source;
	todo = strlen(source);
	while (todo>0)
		todo -= write(fd,p,todo);

	close(fd);

	/* Fork a process to compile it */
	int gcc[2];
	pipe(gcc);
	int pid = fork();
	switch (pid) {
	case -1:
		CLE_log_append(strdup("Error while forking the compiler"));
		CLE_log_append(strdup(strerror(errno)));
		break;
	case 0: // Child: run gcc
		close(gcc[0]);
		close(1);
		close(2);
		dup2(gcc[1],1);
		dup2(gcc[1],2);
		execlp("gcc","gcc","-g","-x","c",filename, "-o",binary,"-Wall",NULL);
	default: // Father: listen what gcc has to tell
		close(gcc[1]);
		char buff[1024];
		int got;
		while ((got = read(gcc[0],&buff,1023))>0) {
			buff[got] = '\0';
			CLE_log_append(strdup(buff));
		}
		waitpid(pid,&status,0);
	}
	exercise_set_binary(e, binary);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
		/* Launch the exercise (in a separate thread waiting for the completion of all entities before re-enabling the button) */
		g_thread_create(exercise_run_runner,e,0,NULL);
	} else  {
		g_mutex_unlock(run_runner_running);

		if (WIFEXITED(status)) {
			CLE_log_append(strdup("Compilation error. Abording code execution\n"));
		} else if (WIFSIGNALED(status)) {
			CLE_log_append(strdup("The compiler got a signal. Weird.\n"));
		}
	}

	unlink(filename);
	free(filename);
	//ENDKILL
}


exercise_t exercise_new(const char *mission, const char *template,
		const char *prof_solution, void* wo) {
	world_t w = (world_t)wo;
	exercise_t result = malloc(sizeof(struct s_exercise));
	result->mission = mission;
	result->template = template;
	result->prof_solution = prof_solution;
	result->s_filename = NULL;
	result->binary=NULL;
	result->demo_runner_running = g_mutex_new ();
	result->run_runner_running = g_mutex_new ();
	result->w_init = (void*)w;
	result->w_curr = (void*)world_copy(w);
	result->w_goal = world_copy(w);
	(*(global_data->lesson->exercise_demo))(result);
	return result;
}

void exercise_free(exercise_t e) {
	world_free(e->w_init);
	world_free(e->w_curr);
	world_free(e->w_goal);
	free(e);
}
