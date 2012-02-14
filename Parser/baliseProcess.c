#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

 
void processWorldBalise(char* line, exo_content* ex)
{
  int decalage=0;
     while((line[decalage] == '\t' || line[decalage] == ' ') && line[decalage]!='\n')
	++decalage;
     if(line[decalage]=='\n')
   {
     printf("Erreur lors du décriptage de la balise world\n");
     return;
   }
    sscanf(line+decalage, "%d %d", &(ex->w.x), &(ex->w.y));
}

void processEntityBalise(char* line, exo_content* ex)
{
   int decalage=0;
     while((line[decalage] == '\t' || line[decalage] == ' ') && line[decalage]!='\n')
	++decalage;
     if(line[decalage]=='\n')
   {
     printf("Erreur lors du décriptage de la balise entity\n");
     return;
   }
   sscanf(line+decalage,"%d %d %lf", &(ex->e.x), &(ex->e.y), &(ex->e.ang));
}

void processLessonBalise(char* line, exo_content* ex)
{
  int decalage=0;
  char name[512];
     while((line[decalage] == '\t' || line[decalage] == ' ') && line[decalage]!='\n')
	++decalage;
     if(line[decalage]=='\n')
   {
     printf("Erreur lors du décriptage de la balise entity\n");
     return;
   }
   sscanf(line+decalage, "%s", name);
   ex->lesson_name = malloc(sizeof(char)*strlen(name)+1);
   memcpy(ex->lesson_name, name, sizeof(char)*strlen(name)+1);
}

void processExerciseBalise(char* line, exo_content* ex)
{
  int decalage=0;
  char name[512];
     while((line[decalage] == '\t' || line[decalage] == ' ') && line[decalage]!='\n')
	++decalage;
     if(line[decalage]=='\n')
   {
     printf("Erreur lors du décriptage de la balise entity\n");
     return;
   }
   sscanf(line+decalage, "%s", name);
   ex->exercise_name = malloc(sizeof(char)*strlen(name)+1);
   memcpy(ex->exercise_name, name, sizeof(char)*strlen(name)+1);
}
