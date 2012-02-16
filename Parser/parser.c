#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "generate.h"
#include "parserUtils.h"
#include "structUtility.h"

int main(int argc, char** argv)
{
  exo_content *exoText = newVoidExoContent();
  lesson_content* lesson = newVoidLessonContent();
  
  if(argc < 2)
  {
    printf("Erreur dans l'entree du parametre aucun parametre donne\n");
  }
  
  parseFile(exoText, argv[1]);
  exoText->descriptor = generateExerciseDescriptor(exoText);
  parseLessonFile(lesson, exoText);
  generateExerciseFile(exoText, lesson);
  addExerciseToLesson(lesson, exoText->descriptor);
  generateLessonFile(lesson);
  generateExerciseHeader(lesson);
  return 0;
}