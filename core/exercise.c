/*
 * exercice.c: the mecanic to declare an exercice
 */
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "core/exercise.h"
#include "core/lesson.h"
#include "UI/CLE.h"


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

void exercice_add_log(exercise_t e, int line, char* msg){
  if (e->nb_logs!=0 && e->nb_logs%MAX_NB_LOG_ERRORS==0 ) {
    e->gcc_logs = realloc(e->gcc_logs, e->nb_logs + MAX_NB_LOG_ERRORS);
    if (e->gcc_logs==NULL) {
      perror("Realloc failed in exercice_add_log\n");
      exit(1);
    }
  }
  
  e->gcc_logs[e->nb_logs] = malloc(sizeof(log_error_s));
  if (e->gcc_logs[e->nb_logs]==NULL) {
    perror("Malloc failed on new log_error\n");
    exit(1);
  }
  e->gcc_logs[e->nb_logs]->line = line;
  e->gcc_logs[e->nb_logs]->msg = msg;
  e->nb_logs++;
}

char* exercice_get_log(exercise_t e, int line){
  int i;
  for (i=0;i<e->nb_logs;i++) {
    if (e->gcc_logs[i]->line==line) {
      return strdup(e->gcc_logs[i]->msg);
    }
  }
  
  char * msg_unknow = malloc(sizeof(char)*(6+1));
  strcpy(msg_unknow,"unknow");
  return msg_unknow;
}

void exercise_clear_log(exercise_t e){
  int i;
  for (i=0;i<e->nb_logs;i++) {
    free(e->gcc_logs[i]);
  }
  
  e->nb_logs=0;
}

void display_compilation_errors(exercise_t e) {
  
  CLE_clear_mark();
  exercise_clear_log(e);
  
  if (e->gcc_report != NULL && e->gcc_report_new) {
    char *first_char_pt=e->gcc_report, 
    *last_char_pt = strchr(e->gcc_report,'\n');
    
    regex_t preg;
    if ( regcomp (&preg, "^/tmp/CLEs.[0-9A-Za-z]{6}:[0-9]+:", REG_NOSUB | REG_EXTENDED)) {
      perror("Erreur de compilation d'expression régulière\n");
      exit(1);
    }
    
    while(last_char_pt!=NULL) {
      int length = last_char_pt-first_char_pt;
      char* line = strndup(first_char_pt,length);
      int match = regexec (&preg, line, 0, NULL, 0);
      if (match==0) {
	// Display this line
	char *string_numline = strchr(line,':');
	char *end = strchr(string_numline+1, ':');
	*end ='\0';
	int numline = atoi(string_numline+1);
	*end = ':';
	
	char *second_2p = strchr(end+1, ':');
	if(!second_2p)
	  second_2p = end+1;
	
	exercice_add_log(e,numline,strdup(second_2p+2));
	if(!strncmp(second_2p+2, "error", strlen("error")))
	  CLE_add_mark(numline, ERROR_LOG);
	else if(!strncmp(second_2p+2, "warning", strlen("warning")))
	  CLE_add_mark(numline, WARNING_LOG);
	
      }
      free(line);
      first_char_pt = last_char_pt +1;
      last_char_pt = strchr(first_char_pt,'\n');
    }
    regfree(&preg);
    
    e->gcc_report_new=0;
    free(e->gcc_report);
    e->gcc_report=NULL;
  }
}


int display_valgrind_errors(valgrind_log_s *data) {
  static char* last_error_message = NULL;
  static int error_stack = 0;
  
    regex_t preg;
    if ( regcomp (&preg, "^==[0-9]*==", REG_NOSUB | REG_EXTENDED)) {
      perror("Erreur de compilation d'expression régulière\n");
      exit(1);
    }
      char* line = strdup(data->line);
      int match = regexec (&preg, line, 0, NULL, 0);
      if (match==0) {
	char* tmp;
	if((tmp=strstr(line, "Invalid")))
	{
	  if(last_error_message)
	    free(last_error_message);
	  error_stack =1;
	  last_error_message = strdup(data->header);
	  CLE_log_append(strdup(data->header));
	  CLE_log_append(strdup(tmp));
	}
	else if((tmp=strstr(line, "at 0x")))
	{
	  if(error_stack)
	  {
	    printf("Dans la pile d'erreur");
	    if(strstr(tmp, data->source_name))
	    {
	      printf(" dans le fichier source de l'élève");
	    }
	      
	    printf("\n");
	  }
	  CLE_log_append(strdup(data->header));
	  CLE_log_append(strdup(tmp));
	}
	else if((tmp=strstr(line, "by 0x")))
	{
	  if(error_stack)
	  {
	    printf("Dans la pile d'erreur\n");
	  }
	  if(strstr(tmp, data->source_name))
	  {
	    printf(" dans le fichier source de l'élève");
	  }
	  CLE_log_append(strdup(data->header));
	  CLE_log_append(strdup(tmp));
	}
	else if((tmp=strstr(line, "Address 0x")))
	{
	  error_stack =0;
	  CLE_log_append(strdup(data->header));
	  CLE_log_append(strdup(tmp));
	}
	else
	  printf("ligne de valgrind reçu et non traité : %s", data->line);
	return 0;
      }
    regfree(&preg);
    return 1;
  }











void exercise_append_gcc_log(exercise_t e,char* log, int size)
{    
  int l=0;
  if (!e->gcc_report_new || e->gcc_report == NULL) {
    if (!e->gcc_report_new && e->gcc_report != NULL)
      free(e->gcc_report);
    
    l = size+1;
    
    e->gcc_report = malloc(sizeof(*e->gcc_report)*(l));
    if (e->gcc_report == NULL) {
      perror("Malloc failed on exerise_run for gcc_report's exercise\n");
      exit(1);
    }
    e->gcc_report[0]='\0';
  } else {
    l=strlen(e->gcc_report)+size+1;
    char* temp_pt = realloc(e->gcc_report,sizeof(*e->gcc_report)*(l));
    if (temp_pt == NULL) {
      perror("Realloc failed on exerise_run for gcc_report's exercise\n");
      exit(1);
    }
    e->gcc_report = temp_pt;
  }
  
  strncat(e->gcc_report,log,size);
  e->gcc_report[l-1] = '\0';
  e->gcc_report_new=1;
}