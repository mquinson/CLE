#ifndef TESTSTRACE_H_INCLUDED
#define TESTSTRACE_H_INCLUDED 

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define TRACE_FORK "trace=process"

/**
Structure linked to an action of a processus
*/
typedef struct{
	int begin;			/**0 for */
	int end;			/**0 if run, 1 if end, 2 if zombie*/
	int wait;			/**0 if run, 1 if wait*/
	int pid_father;		/**number of the pid who executes the action*/
	int pid_son;		/**number of the pid who is the result of the action 
						(for a wait it's the pid who is waited, for a fork it's the numbre of the pid son)*/
	char *call;			/**name of the command called by the processus
						(clone for a fork, wait4 for a wait, ...)
						* these names are determined in observing the lines returned by the execution of strace */
}action;

/**
Allocate an action
*/
action *allocate_action();

/**
Free an action
*/
void free_action(action *a);

/**
Build an action with a line
*/
action *build_again_action(char *line);

/**
Parse a line and create an action
* Transform a line of strace in an acyion
*/
action *parser_command(char *line);

/**
Duplicate the processus, the son executes a strace of the executable and write the result in a FIFO
the father read the FIFO line by line, parse and write the result in a pipe (fds)
*/
void writing_command(int fds,char* name_prog);

/**
Read a single line on the file descriptor fd
* it's saved in line
*/
int readline (int fd, char *line, int maxlen);

/**
Know if pid is in list
* 0 if pid is not in the list
* 1 otherwise
*/
int isInListing(int *listing,int size,int pid_f);

/**
Clear the list of numbers listing which has a size of size 
*/
void clear_listing(int *listing,int size);

/**
Parse info to give orders to draw
*/
void read_info(int fdl,int fdw);

/**
Execute the processus and analyse them (param is a reading_exection *)
*/
void execute_proc(char *name_binary,int fd);

#endif // TESTSTRACE_H_INCLUDED 
