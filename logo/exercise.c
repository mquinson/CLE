

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <regex.h>


#include "UI/CLE.h"
#include "core/exercise.h"
#include "core/lesson.h"
#include "core/world.h"
#include "logo/entity_userside.h"
#include "logo/entity.h"
#include "logo/exercise.h"



void* exercise_demo_runner(void* exo);
void exercise_run_one_entity(entity_t t);

/* Function launched in a separate thread to run the demo without locking the UI
 * It is in charge of starting a thread for each entity to animate, and wait for their completion
 */
void* exercise_demo_runner(void* exo) {
    
	int it;
	entity_t t;
	
	exercise_t e = exo;
	if(e->s_filename==NULL)
	{
	  char *filename= generate_temporary_sourcefile_header(e, userside, e->prof_solution);
	  
	  char* binary_t = strdup("/tmp/CLEb.XXXXXX");
	  int ignored =mkstemp(binary_t); // avoid the useless warning on mktemp dangerousness
	  close(ignored);

	  /* Fork a process to compile it */
	  int status;
	  int gcc[2];
	  pipe(gcc);
	  int pid = fork();
	  switch (pid) {
	  case -1:
		  CLE_add_log_to_all(strdup("Error while forking the compiler"));
		  CLE_add_log_to_all(strdup(strerror(errno)));
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
		  char* tmp=buff;
		  int got;
		  while ((got = read(gcc[0],tmp,1))>0) {
		    if(*tmp=='\n')
		    {
		      ++tmp;
		      *tmp='\0';
		      display_compilation_errors(strdup(tmp));
		      tmp=buff;
		    }
		    else
		      ++tmp;
		  }
		  waitpid(pid,&status,0);
		  close(gcc[0]);
	  }
	  e->s_filename = binary_t;
	  unlink(filename);
	  free(filename);
	}
	
	int amount_entity = 0;
	int i;
	for(i=0; i< e->worldAmount; ++i)
	  amount_entity += world_get_amount_entity(e->w_goal[i]);
	
	
	GThread **runners = malloc(sizeof(GThread*)*(amount_entity));

	
	
	  /* Reset all goal world */
	  for(i=0; i< e->worldAmount; ++i)
	  {
	    world_free(e->w_goal[i]);
	    e->w_goal[i] = world_copy(e->w_init[i]);
	    world_set_step_delay(e->w_goal[i],50); /* FIXME: should be configurable from UI */
	    world_foreach_entity(e->w_goal[i],it,t){
	    entity_set_code(t,exercise_run_one_entity);
	    entity_set_binary(t, e->s_filename);
	  }
	}

	/* Launch all the runners */
	int acc_entity=0;;
	for(i=0; i< e->worldAmount; ++i)
	{
	  world_foreach_entity(e->w_goal[i],it,t)
	  runners[it+acc_entity] = g_thread_create(entity_run,t,1,NULL);
	  
	  acc_entity += world_get_amount_entity(e->w_goal[i]);
	}

	/* Wait the end of all runners */
	for(i=0; i< amount_entity; ++i)
	{
	  g_thread_join(runners[i]);
	}
	
	/* Re-enable the demo running button */
	for(i=0; i< e->worldAmount; ++i)
	{
	  world_set_step_delay(e->w_goal[i],0);
	}
	
	g_mutex_unlock(e->demo_runner_running);
	return NULL;
}

void exercise_demo(exercise_t e) {
//   printf("Starting demo\n");
//   int *p=NULL;
//   printf("%d", *p);
	int res = g_mutex_trylock(e->demo_runner_running);
	if (!res) {
		printf("Not restarting the demo (it's already running)\n");
		return;
	}

	/* Launch the demo (in a separate thread waiting for the completion of all entities before re-enabling the button) */
	g_thread_create(exercise_demo_runner,e,0,NULL);
}


void exercise_demo_stop(exercise_t e) {
	/* Actually, we don't stop the demo since we *need* it to compute the goal world.
	 * Instead, we stop the animation and get it computing as fast as possible.
	 * That's not what we want to do for exercise_run_stop (or whatever you call it). Instead we want to kill the child doing it.
	 */
	int i;
	for(i=0; i< e->worldAmount; ++i)
	{
	  world_set_step_delay(e->w_goal[i],0);
	}
	
}

void exercise_run_stop(exercise_t e) {
	/* actually kill all the processes */
	int it;
	int i;
	for(i=0; i< e->worldAmount; ++i)
	{
	  for (it=0;it< world_get_amount_entity(e->w_curr[i]); it++)
	  {
	    if(world_entity_get_pid(e->w_curr[i], it) != 0)
	      kill(world_entity_get_pid(e->w_curr[i], it),SIGTERM);
	  }
	}
}

void exercise_stop(lesson_t lesson)
{
  if(exercise_demo_is_running(lesson->e_curr))
  {
      exercise_demo_stop(lesson->e_curr);
  }
  else
      exercise_run_stop(lesson->e_curr);
}


int exercise_demo_is_running(void* exo) {
	exercise_t e = exo;
	int res = g_mutex_trylock(e->demo_runner_running);
	if (res)
		g_mutex_unlock(e->demo_runner_running);

	printf("Demo is %srunning\n",(!res?"":"NOT "));
	return !res;
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
	  CLE_add_log_for_world(strdup("Error while forking the entity runner"), world_get_rank(entity_get_world(t)));
	  CLE_add_log_for_world(strdup(strerror(errno)), world_get_rank(entity_get_world(t)));
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
		
		char exec_name[50];
		sprintf(exec_name, ".%s", entity_get_binary(t));
		
		if(CLE_is_debug_mode())
		{
		  if(!access("/usr/bin/valgrind", F_OK))
		    execl("/usr/bin/valgrind","/usr/bin/valgrind", entity_get_binary(t),NULL);
		  else
		    fprintf(stderr, "Cannot run on debug mode : valgrind can't be found on computer\n");
		}
		else
		  execl(entity_get_binary(t),"child",NULL);
		perror("OUCH execl failed!\n");
		exit(2);
	}// Father: listen what the child has to tell

	printf("Turtle rank %d running child %s\n",entity_get_rank(t),entity_get_binary(t));

	entity_set_pid(t, pid);
	close(f2c[0]);
	close(c2f[1]);
	close(cmd_f2c[0]);
	close(cmd_c2f[1]);

	/*Create structure used by the log listener*/
	struct log_listener_data* data = malloc(sizeof(struct log_listener_data));
	data->pipe = c2f[0];
	data->valgrind_log = malloc(sizeof(valgrind_log_s));
	data->valgrind_log->world_numero = world_get_rank(entity_get_world(t));
	/*A null source_name means that we use global prefix for filename */
	data->valgrind_log->source_name = NULL;
	data->valgrind_log->source_limit = CLE_get_sourcecode_size();
	GThread * log_listener = g_thread_create(exercise_run_log_listener,data,1,NULL);
	
	
	/* Main interaction loop with the child */
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
			fflush(tochild);
			break;
		case 101:
			fprintf(tochild,"%lf\n",entity_get_y(t));
			fflush(tochild);
			break;
		case 102:
			fprintf(tochild,"%lf\n",entity_get_heading(t));
			fflush(tochild);
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
		  CLE_add_log_for_world(strdup("Oops, unknown order from child: "), world_get_rank(entity_get_world(t)));
		  CLE_add_log_for_world(strdup(buff), world_get_rank(entity_get_world(t)));
		}
	}
	/* the child is done. Cleaning */
	fclose(fromchild);
	fclose(tochild);
	waitpid(entity_get_pid(t),&status,0);
	if (WIFSIGNALED(status)) {
		if (WTERMSIG(status)==SIGTERM) {
		  CLE_add_log_for_world(strdup("(execution aborded on user request)\n"), world_get_rank(entity_get_world(t)));
		} else if (WTERMSIG(status)==SIGSEGV) {
		  CLE_add_log_for_world(strdup("The your code in a SIGSEGV. Check your pointers.\n"), world_get_rank(entity_get_world(t)));
		} else {
		  CLE_add_log_for_world(strdup("The child running the entity got signaled!\n"), world_get_rank(entity_get_world(t)));
		}
	} else if (WIFEXITED(status) && WEXITSTATUS(status) !=0) {
	  CLE_add_log_for_world(strdup("The child running the entity returned with abnormal return value!\n"), world_get_rank(entity_get_world(t)));
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

	/* Reset the goal world */
	int amount_entity = 0;
	int i;
	for(i=0; i< e->worldAmount; ++i)
	  amount_entity += world_get_amount_entity(e->w_init[i]);
	
	GThread **runners = malloc(sizeof(GThread*)*(amount_entity));
	
	for(i=0; i< e->worldAmount; ++i)
	{
	  world_free(e->w_curr[i]);
	  e->w_curr[i] = world_copy(e->w_init[i]);
	  world_set_step_delay(e->w_curr[i],50); /* FIXME: should be configurable from UI */
	  world_foreach_entity(e->w_curr[i],it,t){
	    entity_set_code(t,exercise_run_one_entity);
	    entity_set_binary(t, exercise_get_binary(e));
	  }
	}
	

	printf("Launch all entities\n");
	/* Launch all the runners */
	int acc_entity=0;;
	for(i=0; i< e->worldAmount; ++i)
	{
	  world_foreach_entity(e->w_curr[i],it,t)
	  runners[it+acc_entity] = g_thread_create(entity_run,t,1,NULL);
	  
	  acc_entity += world_get_amount_entity(e->w_curr[i]);
	}

	/* Wait the end of all runners */
	for(it = 0; it<amount_entity; ++it)
	  g_thread_join(runners[it]);

	/* Re-enable the run running button */
	for(i=0; i< e->worldAmount; ++i)
	{
	  world_set_step_delay(e->w_curr[i],0);
	}

	int success=1;
	for(i=0; i< e->worldAmount; ++i)
	{
	  if (!world_eq(e->w_curr[i],e->w_goal[i]))
	  {
	    success = 0;
	    break;
	  }
	}
	if (success)
	  CLE_dialog_success();
	else
	  CLE_dialog_failure("One world differs from its goal");
	g_mutex_unlock(e->run_runner_running);
	unlink(e->binary);
	return NULL;
}


void exercise_run(exercise_t e, char *source) {
	int status; // test whether they were compilation errors

	int res = g_mutex_trylock(e->run_runner_running);
	
	if (!res) {
		printf("Not restarting the execution (it's already running)\n");
		return;
	}

	/* clear the logs */
	CLE_log_clear();

	/* create 2 filenames */
	char *filename= generate_temporary_sourcefile_header(e, userside, source);
	
	char *binary = strdup("/tmp/CLEb.XXXXXX");
	int ignored =mkstemp(binary); // avoid the useless warning on mktemp dangerousness
	close(ignored);

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
		char* tmp=buff;
		int got;
		while ((got = read(gcc[0],tmp,1))>0) {
		  if(*tmp=='\n')
		  {
		    ++tmp;
		    *tmp='\0';
		  display_compilation_errors(strdup(buff));
		  tmp=buff;
		  }
		  else
		    ++tmp;
		}
		waitpid(pid,&status,0);
		close(gcc[0]);
	}
	exercise_set_binary(e, binary);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
		/* Launch the exercise (in a separate thread waiting for the completion of all entities before re-enabling the button) */
		g_thread_create(exercise_run_runner,e,0,NULL);
	} else  {
		g_mutex_unlock(e->run_runner_running);

		if (WIFEXITED(status)) {
			CLE_add_log_to_all(strdup("Compilation error. Abording code execution\n"));
		} else if (WIFSIGNALED(status)) {
			CLE_add_log_to_all(strdup("The compiler got a signal. Weird.\n"));
		}
	}

	unlink(filename);
	free(filename);
	free(binary);
}


exercise_t exercise_new(const char *mission, const char *template, const char *prof_solution) {
	exercise_t result = malloc(sizeof(struct s_exercise));
	result->mission = mission;
	result->template = template;
	result->prof_solution = prof_solution;
	result->s_filename = NULL;
	result->binary=NULL;
	result->demo_runner_running = g_mutex_new ();
	result->run_runner_running = g_mutex_new ();
	result->w_init = NULL;
	result->w_curr = NULL;
	result->w_goal = NULL;
	result->worldAmount = 0;
	
	result->gcc_report_new=0;
	result->gcc_report = NULL;
	
	result->nb_logs = 0;
	result->gcc_logs = malloc(sizeof(log_error)*MAX_NB_LOG_ERRORS);
	
	result->exercise_free = exercise_free;
	result->unauthorizedNumber = 0;
	result->unauthorizedFunction = NULL;
	return result;
}

void exercise_add_world(exercise_t e, core_world_t world)
{
  world_set_rank(world, e->worldAmount);
   ++(e->worldAmount);
   int i;
   core_world_t* temp = malloc(sizeof(core_world_t*)*e->worldAmount);
   for(i=0; i<e->worldAmount-1; ++i)
   {
     temp[i] = e->w_init[i];
   }
   temp[e->worldAmount-1] = world;
   if(e->w_init)
     free(e->w_init);
   e->w_init = temp;
   
   
   temp = malloc(sizeof(core_world_t*)*e->worldAmount);
   for(i=0; i<e->worldAmount-1; ++i)
   {
     temp[i] = e->w_curr[i];
   }
   temp[e->worldAmount-1] = world_copy(world);
   if(e->w_curr)
     free(e->w_curr);
   e->w_curr = temp;
   
   
   temp = malloc(sizeof(core_world_t*)*e->worldAmount);
   for(i=0; i<e->worldAmount-1; ++i)
   {
     temp[i] = e->w_goal[i];
   }
   temp[e->worldAmount-1] = world_copy(world);
   (*(temp[e->worldAmount-1]->exercise_demo))(e);
   if(e->w_goal)
     free(e->w_goal);
   e->w_goal = temp;
}


void exercise_free(exercise_t e) {
    int i;
	if(e->unauthorizedFunction)
	{
	  for(i=0; i< e->unauthorizedNumber; ++i)
	  {
	    free(e->unauthorizedFunction[i]);
	  }
	  free(e->unauthorizedFunction);
	}
	for(i=0; i< e->worldAmount ; ++i)
	{
	  world_free(e->w_init[i]);
	  world_free(e->w_curr[i]);
	  world_free(e->w_goal[i]);
	}
	free(e->w_init);
	free(e->w_curr);
	free(e->w_goal);
	free(e);
}
