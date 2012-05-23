/*
 * exercice.c: the mecanic to declare an exercice
 */
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "core/exercise.h"
#include "core/lesson.h"
#include "UI/CLE.h"
#include "core/mark.h"

#define SOURCEFILE_PREFIX "CLEs."
#define STUDENT_FILENAME "student.c"
#define TEACHER_FILENAME "teacher.c"


void exercise_set_binary(exercise_t e, char* binary){
	if(e->binary)
	  free(e->binary);
	e->binary = strdup(binary);
}


char* exercise_get_binary(exercise_t e){
	return e->binary;
}

void exercise_set_unauthorizedFunction(exercise_t e, char** functionNameList, int listSize)
{
  if(e->unauthorizedFunction)
    free(e->unauthorizedFunction);
  e->unauthorizedNumber = listSize;
  e->unauthorizedFunction = malloc(sizeof(char*)*listSize);
  int i;
  for(i=0; i< listSize ; ++i)
  {
    e->unauthorizedFunction[i] = functionNameList[i];
  }
}


void display_compilation_errors(char* message) {
  
  regex_t preg;
  if ( regcomp (&preg, "^" STUDENT_FILENAME ":[0-9]+:", REG_NOSUB | REG_EXTENDED)) {
    perror("Erreur de compilation d'expression régulière\n");
    exit(1);
  }
  
  char* line = strdup(message);
  /* Replace ultimate \n by \0 */
  *(strchr(line, '\n'))='\0';

  int match = regexec (&preg, line, 0, NULL, 0);
  if (match==0) {
    // Display this line
    char *string_numline = strchr(line,':');
    /* We don't need to extract line because atoi convert the number until the next non digit symbol*/
    int numline = atoi(string_numline+1);
    
    /*We jump to the message type*/
    char *second_2p = strchr(line, ' ')+1;

    if(!strncmp(second_2p, "error", strlen("error")) || !strncmp(second_2p, "erreur", strlen("erreur")))
      CLE_add_mark_to_all(second_2p,numline, ERROR_LOG);
    else if(!strncmp(second_2p, "warning", strlen("warning")) || !strncmp(second_2p, "attention", strlen("attention")))
      CLE_add_mark_to_all(second_2p,numline, WARNING_LOG);
  }
  CLE_add_log_to_all(message);
  free(line);
  regfree(&preg);
}

int display_valgrind_errors(valgrind_log_s *data) {
  
    regex_t preg;
    if ( regcomp (&preg, "^==[0-9]*==", REG_NOSUB | REG_EXTENDED)) {
      perror("cannot compil regular expression\n");
      exit(1);
    }
      char* line = strdup(data->line);
      int match = regexec (&preg, line, 0, NULL, 0);
      if (match==0) {
	char* tmp;
	if((tmp=strstr(line, "Invalid")))
	{
	  
	  data->error_stack =1;
	  data->adress_stack=0;
	  ++(data->num_error);
	  
	  sprintf(data->last_error_message, "error %d : %s", data->num_error, tmp);
	  /*We copy all string without final '\n' */
	  data->last_error_message[strlen(data->last_error_message)-1]='\0';
	  /*Put a space on first caracter of the line*/
	  --tmp;
	  *tmp=' ';
	  CLE_add_log_for_world(tmp, data->world_numero);
	}
	else if((tmp=strstr(line, "at 0x")))
	{
	  if(data->error_stack)
	  {
	    char* number;
	    if(data->source_name!=NULL)
	      number=strstr(tmp, data->source_name);
	    else
	      number=strstr(tmp, STUDENT_FILENAME);
	    if(number)
	    {
	      number=strchr(number, ':');
	      char* temp = strchr(number, ')');
	      number = strndup(number+1, temp - number-1);
	      int numero_line = atoi(number);
	      free(number);
	      if(numero_line < data->source_limit+1)
	      {
		CLE_add_mark_to_world(data->last_error_message,numero_line, ERROR_LOG, 0);
	      }
	    }
	  }
	  /*Put a space on first caracter of the line*/
	  --tmp;
	  *tmp='\t';
	  CLE_add_log_for_world(tmp, data->world_numero);
	}
	else if((tmp=strstr(line, "by 0x")))
	{
	  if(data->adress_stack)
	  {
	    char* number;
	    if(data->source_name!=NULL)
	      number=strstr(tmp, data->source_name);
	    else
	      number=strstr(tmp, STUDENT_FILENAME);
	    if(number)
	    {
	      number=strchr(number, ':');
	      char* temp = strchr(number, ')');
	      number = strndup(number+1, temp - number-1);
	      int numero_line = atoi(number);
	      free(number);
	      if(numero_line < data->source_limit+1)
	      {
		sprintf(data->last_error_message, "error %d : Allocation of data", data->num_error);
		CLE_add_mark_to_world(data->last_error_message,numero_line, INFO_LOG,0);
	      }
	    }
	    data->adress_stack=0;
	  }
	  /*Put a space on first caracter of the line*/
	  --tmp;
	  *tmp='\t';
	  CLE_add_log_for_world(tmp, data->world_numero);
	}
	else if((tmp=strstr(line, "Address 0x")))
	{
	  data->error_stack =0;
	  data->adress_stack=1;
	  
	  /*Put a space on first caracter of the line*/
	  --tmp;
	  *tmp=' ';
	  CLE_add_log_for_world(tmp, data->world_numero);
	}
	return 0;
      }
    regfree(&preg);
    return 1;
  }


void exercise_print_unauthorized(exercise_t e, int fd)
{
  int i;
  for(i=0 ; i<e->unauthorizedNumber ; ++i)
  {
    write(fd, "#define ", strlen("#define "));
    write(fd, e->unauthorizedFunction[i], strlen(e->unauthorizedFunction[i]));
    write(fd, " You_cannot_use_", strlen(" You_cannot_use_"));
    write(fd, e->unauthorizedFunction[i], strlen(e->unauthorizedFunction[i]));
    write(fd, "\n", 1);
  }
}

void print_line_prepocessor_instruction(int fd, int line, char* filename)
{
  char temp[60];
  sprintf(temp, "#line %d \"%s\"\n", line, filename);
  write(fd,temp, strlen(temp));
}

int get_amount_line(const char* text)
{
  int size=0;
  while(*text != '\0')
  {
    if(*text == '\n')
      ++size;
    ++text;
  }
  return size;
}

char* generate_temporary_sourcefile_header(exercise_t e, const char* userside, const char* source)
{
  int todo;
  char *filename= strdup("/tmp/" SOURCEFILE_PREFIX "XXXXXX");
  int fd = mkstemp(filename);
  if(source != NULL)
    print_line_prepocessor_instruction(fd, get_amount_line(source)+2, filename);
  
  if(userside != NULL)
  {
    todo = strlen(userside);
    while (todo>0)
    {
      int i = write(fd,userside,todo);
	if(i<0)
	{
	  perror("cannot write sourcefile");
	  exit(1);
	}
      todo -= i;
    }
  }
  
  
  /*If source != null, that mean that we create a student source file */
  if(source != NULL)
  {
    exercise_print_unauthorized(e, fd);
    print_line_prepocessor_instruction(fd, 1, STUDENT_FILENAME);
    todo = strlen(source);
    while (todo>0)
      todo -= write(fd,source,todo);
  }
  else
  {
    print_line_prepocessor_instruction(fd, 1, TEACHER_FILENAME);
    todo = strlen(e->prof_solution);
    while (todo>0)
      todo -= write(fd,e->prof_solution,todo);
  }
  
  close(fd);
  
  return filename;
}


/* Small thread in charge of listening everything that the user's entity printf()s,
 * and add it to the log console */
void *exercise_run_log_listener(void *d) {
  struct log_listener_data *data  = d;
  valgrind_log_s *vl = data->valgrind_log;
  
  
  if(global_data->debug)
  {
    CLE_clear_logs_of_world(vl->world_numero);
    CLE_free_mark_of_world(vl->world_numero);
  }
  char buff[1024];
  char* tmp = buff;
  int got;
  while ((got = read(data->pipe,tmp,1))>0) {
    if(*tmp=='\n')
    {
      ++tmp;
      *tmp='\0';
    vl->line = buff;
    if(display_valgrind_errors(vl))
      CLE_add_log_for_world(buff, vl->world_numero);
    
    tmp=buff;
    }
    else
      ++tmp;
  }
  if(data->valgrind_log->source_name)
    free(data->valgrind_log->source_name);
  free(data->valgrind_log);
  free(data);
  return NULL;
}


