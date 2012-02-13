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
	int begin;
	int end;
	int wait;
	int pid_father;
	int pid_son;
	char *call;
}action;

/**
Structure linked to a list of lines
*/
typedef struct{
	int size;
	char **lines;
}list_lines;

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
Allocate a list of lines
*/
list_lines *allocate_list_lines();

/**
Free a list of lines
*/
void free_list_lines(list_lines *l);

/**
Add a line into the list of lines
*/
void add_line(list_lines *l,char* line);

/**
Delete a line of the list of lines
*/
void delete_line(list_lines *l, int pos);

/**
Parse a line and create an action
*/
action *parser(char *line);

/**
Duplicate the processus, the son executes a strace of the executable and write the result in a FIFO
the father read the FIFO line by line, parse and write the result in a pipe (fds)
*/
void writing(int fds,char* name_prog);

/**
Creat a list of lines with a string
*/
list_lines *extract_lines(char *lines);

/**
Know if pid_t is in list
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
