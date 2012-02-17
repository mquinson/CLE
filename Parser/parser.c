#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "generate.h"
#include "parserUtils.h"
#include "structUtility.h"

int main(int argc, char** argv)
{
  exo_content *exercise = newVoidExoContent();
  lesson_content* lesson = newVoidLessonContent();
  
  if(argc < 2)
  {
    printf("Erreur dans l'entree du parametre aucun parametre donne\n");
  }
  
  parseFile(exercise, argv[1]);
  exercise_desc* descrip = generateExerciseDescriptor(exercise);
  parseLessonFile(lesson, exercise);
  generateExerciseFile(exercise, lesson);
  if(!addExerciseToLesson(lesson, descrip))
    freeExerciseDescriptor(descrip);
  generateLessonFile(lesson);
  generateExerciseHeader(lesson);
  freeLessonContent(lesson);
  freeExerciseContent(exercise);
  return 0;
}