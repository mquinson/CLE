#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "structUtility.h"

 
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
   double x, y;
    sscanf(line+decalage, "%lf %lf", &x, &y);
    addWorldToExoContent(ex, newWorld(x,y));
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
   int x, y;
   double ang;
   sscanf(line+decalage,"%d %d %lf", &x, &y, &ang);
   addEntityToExoContent(ex, newEntity(x,y,ang));
}

void processLessonBalise(char* line, exo_content* ex)
{
  char* firstChar=line;
     while((*firstChar == '\t' || *firstChar == ' ') && *firstChar!='\n')
	++firstChar;
     if(*firstChar=='\n')
   {
     printf("Erreur lors du décriptage de la balise entity\n");
     return;
   }
   char* end = firstChar;
   while(*end != '\n')
     ++end;
   *end='\0';
   setLessonName(ex, firstChar);
}

void processExerciseBalise(char* line, exo_content* ex)
{
  char* firstChar=line;
     while((*firstChar == '\t' || *firstChar == ' ') && *firstChar!='\n')
	++firstChar;
     if(*firstChar=='\n')
   {
     printf("Erreur lors du décriptage de la balise entity\n");
     return;
   }
   char* end = firstChar;
   while(*end != '\n')
     ++end;
   *end='\0';
   setExerciseName(ex, firstChar);
}
