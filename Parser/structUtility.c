#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "parser.h"
#include "structUtility.h"

exercise_desc* newExerciseDescriptor(char* name, char* constructor)
{
  exercise_desc *result = malloc(sizeof(exercise_desc));
  result->exerciseName = strdup(name);
  result->exerciseConstructor = strdup(constructor);
  int i=0;
  for(; i<strlen(result->exerciseConstructor) ; ++i)
  {
    if(isalpha(result->exerciseConstructor[i]))
      result->exerciseConstructor[i] = tolower(result->exerciseConstructor[i]);
  }
  return result;
}

void freeExerciseDescriptor(exercise_desc *ex)
{
  free(ex->exerciseName);
  free(ex->exerciseConstructor);
  free(ex);
}



entity* newEntity(int x, int y, double ang)
{
  entity* result = malloc(sizeof(entity));
  result->x =x;
  result->y=y;
  result->ang=ang;
  return result;
}

void freeEntity(entity* en)
{
  free(en);
}



world* newWorld(double x, double y)
{
  world* result = malloc(sizeof(world));
  result->x = x;
  result->y =y;
  return result;
}

void freeWorld(world* w)
{
  free(w);
}



lesson_content* newVoidLessonContent()
{
  lesson_content* result = malloc(sizeof(lesson_content));
  result->amount =0;
  result->exercises = NULL;
  result->filename=NULL;
  result->lesson_name_file = NULL;
  result->lesson_name=NULL;
  return result;
}

lesson_content* newLessonContent(char* name)
{
  lesson_content* result = malloc(sizeof(lesson_content));
  generateLessonFromName(result, name);
  result->amount=0;
  result->exercises=NULL;
  return result;
}

void generateLessonFromName(lesson_content* lesson, char* name)
{
    if(lesson==NULL)
      return;
    if(lesson->lesson_name_file)
      free(lesson->lesson_name_file);
    if(lesson->lesson_name)
      free(lesson->lesson_name);
    if(lesson->filename)
      free(lesson->filename);
    lesson->lesson_name = strdup(name);
    lesson->lesson_name_file = strdup(name);
    int i=0;
    for(; i<strlen(lesson->lesson_name_file); ++i)
    {
      if(isalpha(lesson->lesson_name_file[i]))
      {
	lesson->lesson_name_file[i] = tolower(lesson->lesson_name_file[i]);
      }
    }
    lesson->filename= malloc(sizeof(char)*(strlen(lesson->lesson_name_file)*2+4));
    sprintf(lesson->filename, "%s/%s.c", lesson->lesson_name_file, lesson->lesson_name_file);
}

void freeLessonContent(lesson_content* lesson)
{
  if(lesson==NULL)
      return;
  if(lesson->lesson_name_file)
    free(lesson->lesson_name_file);
  if(lesson->lesson_name)
    free(lesson->lesson_name);
  if(lesson->filename)
    free(lesson->filename);
  int i=0;
  for(;i<lesson->amount; ++i)
    freeExerciseDescriptor(lesson->exercises[i]);
  if(lesson->exercises)
    free(lesson->exercises);
  free(lesson);  
}

int addExerciseToLesson(lesson_content* lesson, exercise_desc* desc)
{
  int i;
  for(i=0; i< lesson->amount; ++i)
  {
    if(!strcmp(lesson->exercises[i]->exerciseConstructor, desc->exerciseConstructor))
      return 0;
  }
  
  exercise_desc** temp = malloc(sizeof(exercise_desc* )*(lesson->amount+1));
  for(i=0;i<lesson->amount; ++i)
  {
    temp[i] = lesson->exercises[i];
  }
  if(lesson->exercises)
    free(lesson->exercises);
  temp[lesson->amount] = desc;
  lesson->exercises = temp;
  ++(lesson->amount);
  return 1;
}



exo_content* newVoidExoContent()
{
  exo_content *result = malloc(sizeof(exo_content));
  result->w=NULL;
  result->e=NULL;
  result->lesson_name=NULL;
  result->exercise_name=NULL;
  result->exercise_file_name = NULL;
  result->descriptionSize=0;
  result->description=NULL;
  result->codeEleveSize=0;
  result->codeEleve=NULL;
  result->codeProfSize=0;
  result->codeProf=NULL;
  return result;
}

void addWorldToExoContent(exo_content* ex, world* w)
{
  if(ex->w)
    free(ex->w);
  ex->w=w;
}

void addEntityToExoContent(exo_content* ex, entity* e)
{
  if(ex->e)
    free(ex->e);
  ex->e=e;
}

void addDescription(exo_content* ex, char* description)
{
  char** temp = malloc(sizeof(char*)*(ex->descriptionSize+1));
  int i = 0;
  for(; i< ex->descriptionSize; ++i)
    temp[i] = ex->description[i];
  free(ex->description);
  temp[ex->descriptionSize] = strdup(description);
  ex->description = temp;
  ++(ex->descriptionSize);
}

void addCodeEleve(exo_content* ex, char* codeEleve)
{
  char** temp = malloc(sizeof(char*)*(ex->codeEleveSize+1));
  int i = 0;
  for(; i< ex->codeEleveSize; ++i)
    temp[i] = ex->codeEleve[i];
  free(ex->codeEleve);
  temp[ex->codeEleveSize] = strdup(codeEleve);
  ex->codeEleve = temp;
  ++(ex->codeEleveSize);
}


void addCodeProf(exo_content* ex, char* codeProf)
{
  char** temp = malloc(sizeof(char*)*(ex->codeProfSize+1));
  int i = 0;
  for(; i< ex->codeProfSize; ++i)
    temp[i] = ex->codeProf[i];
  free(ex->codeProf);
  temp[ex->codeProfSize] = strdup(codeProf);
  ex->codeProf = temp;
  ++(ex->codeProfSize);
}

void setLessonName(exo_content* ex, char* lesson_name)
{
  if(ex->lesson_name)
    free(ex->lesson_name);
  ex->lesson_name = strdup(lesson_name);
  int i=0;
  for(; i<strlen(ex->lesson_name) ; ++i)
  {
    if(isalpha(ex->lesson_name[i]))
      ex->lesson_name[i] = tolower(ex->lesson_name[i]);
  }
}

void setExerciseName(exo_content* ex, char* exercise_name)
{
  if(ex->exercise_name)
    free(ex->exercise_name);
  if(ex->exercise_file_name)
    free(ex->exercise_file_name);
  ex->exercise_name = strdup(exercise_name);
  ex->exercise_file_name = strdup(exercise_name);
  int i=0;
  for(; i<strlen(ex->exercise_file_name) ; ++i)
  {
    if(isalpha(ex->exercise_file_name[i]))
      ex->exercise_file_name[i] = tolower(ex->exercise_file_name[i]);
  }
}

void freeExerciseContent(exo_content* ex)
{
  if(ex->w)
    free(ex->w);
  if(ex->e)
    free(ex->e);
  if(ex->lesson_name)
    free(ex->lesson_name);
  if(ex->exercise_file_name)
    free(ex->exercise_file_name);
  if(ex->exercise_name)
    free(ex->exercise_name);
  int i;
  for(i=0; i< ex->descriptionSize; ++i)
    free(ex->description[i]);
  if(ex->description)
    free(ex->description);
  for(i=0; i< ex->codeEleveSize; ++i)
    free(ex->codeEleve[i]);
  if(ex->codeEleve)
    free(ex->codeEleve);
  for(i=0; i< ex->codeProfSize; ++i)
    free(ex->codeProf[i]);
  if(ex->codeProf)
    free(ex->codeProf);
  free(ex);
}

























