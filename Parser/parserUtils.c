#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include "parser.h"
#include "parserUtils.h"
#include "structUtility.h"
#include "baliseProcess.h"
#include "generate.h"


static char* begin_template_balise = "/*BEGIN TEMPLATE*/";
static char* end_template_balise="/*END TEMPLATE*/";
static char* begin_solution_balise="/*BEGIN SOLUTION*/";
static char* end_solution_balise="/*END SOLUTION*/";

static char* world_balise="@world";
static char* entity_balise="@entity";
static char* lesson_balise="@lesson";
static char* exercise_balise="@exercise";
static char* description_balise="@description";

static int template=0;
static int solution=0;
static int description=0;
static int lineCount=0;
static int exercise_description=0;



int parseBalise(char* line);
int parseBaliseDescription(char* line);
void parseDescription(exo_content *exoText, char* line);
void parseLine(exo_content* exoText, char* line);
char* extractLessonMain(int fd);
char** parseToArglist(char* arg);
void constructLessonContent(lesson_content* lesson, char** arg);


int parseBalise(char* line)
{
    if(!strncmp(begin_template_balise, line, strlen(begin_template_balise)))
    {
      template=1;
      return 1;
    }
    else if(!strncmp(end_template_balise, line, strlen(end_template_balise)))
    {
      template=0;
      return 1;
    }
    else if(!strncmp(begin_solution_balise, line, strlen(begin_solution_balise)))
    {
      solution=1;
      return 1;
    }
    else if(!strncmp(end_solution_balise, line, strlen(end_solution_balise)))
    {
      solution=0;
      return 1;
    }
    return 0;
}

int parseBaliseDescription(char* line)
{
    if(!strncmp(world_balise, line, strlen(world_balise)))
      return WORLD_BALISE;
    else if(!strncmp(entity_balise, line, strlen(entity_balise)))
      return ENTITY_BALISE;
    else if(!strncmp(lesson_balise, line, strlen(lesson_balise)))
      return LESSON_BALISE;
    else if(!strncmp(exercise_balise, line, strlen(exercise_balise)))
      return EXERCISE_BALISE;
    else if(!strncmp(description_balise, line, strlen(description_balise)))
      return DESCRIPTION_BALISE;
    else
      return WRONG_BALISE;
}


void parseDescription(exo_content *exoText, char* line)
{
   int decalage=0;
   while((line[decalage] == '\t' || line[decalage] == ' ') && line[decalage]!='\n')
    ++decalage;
   
  if(exercise_description)
  {
    if(parseBaliseDescription(line+decalage) != WRONG_BALISE)
      exercise_description=0;
    else
    {
      line[strlen(line)-1]='\0';
      addDescription(exoText, line);
      return;
    }
  }
 
   if(line[decalage]=='\n')
   {
     return;
   }
   else
   switch(parseBaliseDescription(line+decalage))
   {
     case WORLD_BALISE:
       processWorldBalise(line+decalage+strlen(world_balise), exoText);
       break;
       
     case ENTITY_BALISE:
       processEntityBalise(line+decalage+strlen(entity_balise), exoText);
       break;
       
     case LESSON_BALISE:
       processLessonBalise(line+decalage+strlen(lesson_balise), exoText);
       break;
       
     case EXERCISE_BALISE:
       processExerciseBalise(line+decalage+strlen(exercise_balise), exoText);
       break;
       
     case DESCRIPTION_BALISE:
       line+=decalage+strlen(description_balise);
       if(*line == ' ')
	 ++line;
       if(strlen(line)==0)
	  break;
       line[strlen(line)-1]='\0';
       addDescription(exoText, line);
       exercise_description=1;
       break;
       
     case WRONG_BALISE:
       printf("Erreur dans la description\n");
       break;
   };
}


void parseLine(exo_content *exoText, char* line)
{
  int decalage=0;
  while(line[decalage] == '\t' || line[decalage] == ' ')
    ++decalage;
  if(!description)
  {
    if(line[decalage]=='/')
    {
     if(line[1+decalage]=='*')
     {
       if(line[2+decalage]=='*')
	 description=1;
       else
       {
	 if(parseBalise(line+decalage))
	  line[0]='\0';
       }
     }
    }
  }
  else
  {
    if(line[decalage]=='*')
    {
      if(line[decalage+1]=='/')
      {
	line[decalage]='\0';
	description=0;
      }
      else
	parseDescription(exoText, line+decalage+1);
    }
  }
  
}


void parseFile(exo_content* exoText, const char* filename)
{
  FILE* file = fopen(filename, "r");
  char* buff=NULL;
  size_t len=0;
  int got;
  
  while((got = getline(&buff, &len, file))!=-1)
  {
    ++lineCount;
    if(buff[0]=='\n')
      continue;
    
    parseLine(exoText, buff);
    if(!description)
    {
      if(buff[0]=='\0')
	continue;
      buff[strlen(buff)-1]='\0';
      if(template && !solution)
      {
	addCodeEleve(exoText, buff);
      }
      addCodeProf(exoText, buff);
    }
  }
  fclose(file);
}

void constructLesson(lesson_content* lesson, char** args)
{
  generateLessonFromName(lesson,  args[0]);
  int amount = strtol(args[1], NULL, 10);
  int i;
  for(i=0; i< amount; ++i)
  {
    exercise_desc *temp = newExerciseDescriptor(args[2+ i*2],args[3+2*i]);
    addExerciseToLesson(lesson, temp);
  }
}

void parseLessonFile(lesson_content *lesson, exo_content *exo)
{
  char* filename= malloc(sizeof(char)*(strlen(exo->lesson_name)*2+4));
  sprintf(filename, "%s/%s.c", exo->lesson_name, exo->lesson_name);
  printf("Fichier lesson : %s\n", filename);
  int fd = open(filename, O_RDONLY);
  if(fd==0)
  {
      printf("Impossible de charger le fichier de leçon. Abandon.\n");
      exit(1);
  }
  /*First we extract the arguments of lesson constructor*/
  char *content = extractLessonMain(fd);
  char** args = parseToArglist(content);
  free(content);
  close(fd);
  constructLesson(lesson, args);
}

/*Be carefull, this function change pointer value*/
void goToNextWorld(char** ptr)
{
  while((**ptr) !=' ' && (**ptr) != '\0' && (**ptr) != '\t')
    ++(*ptr);
  while((**ptr) == ' ' || (**ptr) == '\t')
  {
    if((**ptr) == '\0')
      return;
    ++(*ptr);
  }
}

char* extractLessonMain(int fd)
{
  char* pattern = "lesson_new";
  long size = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  char* result = malloc(sizeof(char)*(size+1));
  int i=0;
  char buff;
  for(i=0; i<size; ++i)
  {
     read(fd, &buff, 1);
     if(buff == '\n')
       buff=' ';
     result[i] = buff;
  }
  result[size]='\0';
  char* temp = result;
  i = 2;
    /*We continue with the recognition of lesson_new function. Here, we supposed that the first line of text can't
     start with the lesson_new function*/
  while(1)
  {
    goToNextWorld(&temp);
    if(*temp == '\0')
    {
      /*If we found a null caracter, it means that we are unable to pick up lesson_new function*/
      printf("Lesson_new n'a pu être trouver\n");
      return NULL;
    }
    if(!strncmp(temp, pattern , strlen(pattern)))
    {
      //If we found the term lesson_new we stop the loop.
      break;
    }
  }
  /* Now we extract arguments of lesson_new function*/
  while(*temp != '(')
    ++temp;
  ++temp;
  char* first_bound = temp;
  while(*temp != ')')
    ++temp;
  *temp='\0';
  temp = malloc(sizeof(char)* (strlen(first_bound)+1));
  int indice =0;
  /*Finally, we extract the arguments of lesson_new without space and tabulation*/
  while(*first_bound != '\0')
  {
    if(*first_bound != ' ' && *first_bound !='\t')
    {
       temp[indice] = *first_bound;
       ++indice;
    }
    ++first_bound;
  }
  temp[indice]='\0';
  free(result);
  return temp;
}

void toNextComma(char** ptr)
{
  while(**ptr != ',' && **ptr !='\0')
    ++(*ptr);
}

char** parseToArglist(char* arg)
{
    char* firstBound = arg+1;
    char* lastBound = arg+1;
    /*We start with the extraction of lesson_name*/
    toNextComma(&lastBound);
    *(lastBound-1)='\0';
    char* name = strdup(firstBound);
    
    /*Now we extract the amount of exercise in lesson for extraction*/
    firstBound = ++lastBound;
    toNextComma(&lastBound);
    *lastBound = '\0';
    int amount = strtol(firstBound,NULL, 10);
    
    /*Now we now how many char* are needed for extract all arguments*/
    char** result = malloc(sizeof(char*)*((amount*2)+2));
    result[0]=name;
    result[1] = strdup(firstBound);
    
    /*Now we extract all this lesson*/
    int i=0;
    for(; i< amount; ++i)
    {
      firstBound = (++lastBound)+1;
      toNextComma(&lastBound);
      *(lastBound-1)='\0';
      result[2 + i*2] = strdup(firstBound);
      firstBound = ++lastBound;
      toNextComma(&lastBound);
      *lastBound = '\0';
      result[3 + i*2] = strdup(firstBound);
    }
    return result;
}


exercise_desc *generateExerciseDescriptor(exo_content *ex)
{
  char* constructor = malloc(sizeof(char)*(strlen(ex->lesson_name) + strlen(ex->exercise_name) + sizeof("create")+3));
  sprintf(constructor, "%s_%s_create", ex->lesson_name, ex->exercise_name);
  exercise_desc *result = newExerciseDescriptor(ex->exercise_name, constructor);
  free(constructor);
  return result; 
}
