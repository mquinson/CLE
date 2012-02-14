#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "generate.h"
#include "parserUtils.h"

int main(int argc, char** argv)
{
  exo_content exoText;
  (exoText.description = malloc(sizeof(char*)*20));
  exoText.codeEleve = malloc(sizeof(char*)*20);
  exoText.codeProf = malloc(sizeof(char*)*20);
  exoText.descriptionSize = 0;
  exoText.codeEleveSize =0;
  exoText.codeProfSize = 0;
  
  lesson_content lesson;
  
  if(argc < 2)
  {
    printf("Erreur dans l'entree du parametre aucun parametre donne\n");
  }
  
  //parseFile(&exoText, argv[1]);
  //generateExerciseFile(&exoText);
  parseLessonFile(&lesson, "../logo/logo.c");
  return 0;
}