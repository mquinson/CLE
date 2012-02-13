#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "baliseProcess.h"
#include "parser.h"
#include "generate.h"

#define WRONG_BALISE 0
#define WORLD_BALISE 1
#define ENTITY_BALISE 2
#define LESSON_BALISE 3
#define EXERCISE_BALISE 4
#define DESCRIPTION_BALISE 5

static char* begin_template_balise = "/*BEGIN TEMPLATE*/";
static char* end_template_balise="/*END TEMPLATE*/";
static char* begin_solution_balise="/*BEGIN SOLUTION*/";
static char* end_solution_balise="/*END SOLUTION*/";

static char* world_balise="@world";
static char* entity_balise="@entity";
static char* lesson_balise="@lesson";
static char* exercise_balise="@exercise";
static char* description_balise="@description";

int template=0;
int solution=0;
int description=0;
int lineCount=0;
int exercise_description=0;
static exo_content exoText;

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




void parseDescription(char* line)
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
      exoText.description[exoText.descriptionSize]= strdup(line);
      ++exoText.descriptionSize;
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
       processWorldBalise(line+decalage+strlen(world_balise), &exoText);
       break;
       
     case ENTITY_BALISE:
       processEntityBalise(line+decalage+strlen(entity_balise), &exoText);
       break;
       
     case LESSON_BALISE:
       processLessonBalise(line+decalage+strlen(lesson_balise), &exoText);
       break;
       
     case EXERCISE_BALISE:
       processExerciseBalise(line+decalage+strlen(exercise_balise), &exoText);
       break;
       
     case DESCRIPTION_BALISE:
       if(*(line+decalage+strlen(description_balise)) == ' ')
	 ++decalage;
       line[strlen(line)-1]='\0';
       exoText.description[exoText.descriptionSize]= strdup(line+decalage+strlen(description_balise));
       ++exoText.descriptionSize;
       exercise_description=1;
       break;
       
     case WRONG_BALISE:
       printf("Erreur dans la description\n");
       break;
   };
}

void parseLine(char* line)
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
	parseDescription(line+decalage+1);
    }
  }
  
}

int main(int argc, char** argv)
{
  (exoText.description = malloc(sizeof(char*)*20));
  printf("%p\n", exoText.description);
  exoText.codeEleve = malloc(sizeof(char*)*20);
  exoText.codeProf = malloc(sizeof(char*)*20);
  exoText.descriptionSize = 0;
  exoText.codeEleveSize =0;
  exoText.codeProfSize = 0;
  
  if(argc < 2)
  {
    printf("Erreur dans l'entree du parametre aucun parametre donne\n");
  }
  FILE* file = fopen(argv[1], "r");
  char* buff=NULL;
  size_t len=0;
  int got;
  
  while((got = getline(&buff, &len, file))!=-1)
  {
    ++lineCount;
    if(buff[0]=='\n')
      continue;
    
    parseLine(buff);
    if(!description)
    {
      buff[strlen(buff)-1]='\0';
      if(template && !solution)
      {
	exoText.codeEleve[exoText.codeEleveSize]=strdup(buff);
	++exoText.codeEleveSize;
      }
      exoText.codeProf[exoText.codeProfSize]=strdup(buff);
	++exoText.codeProfSize;
    }
  }
  
  fclose(file);
  generateExerciseFile(&exoText);
  return 0;
}