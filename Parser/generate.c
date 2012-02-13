#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"


void printInclude(exo_content *ex, FILE* src)
{
  fprintf(src, "#include \"core/exercise.h\"\n");
  fprintf(src, "#include \"%s/%s.h\"\n", ex->lesson_name, ex->lesson_name);
  fprintf(src, "#include \"%s/world.h\"\n", ex->lesson_name);
  fprintf(src, "#include \"%s/entity.h\"\n", ex->lesson_name);
  fprintf(src, "\n");
}

void printLine(char** lines, int amount, FILE* src)
{
  int i;
  for(i=0; i<amount; ++i)
  {
    fprintf(src, "\"%s\\n\"\n", lines[i]);
  }
}


void printExercise(exo_content *ex, FILE* src)
{
  fprintf(src, "exercise_t %s_%s_create(void) {\n", ex->lesson_name, ex->exercise_name);
  fprintf(src, "\tworld_t w = world_new(%d,%d);\n", ex->w.x, ex->w.y);
  fprintf(src, "\tworld_entity_add(w, entity_new(%d,%d,%lf));\n", ex->e.x, ex->e.y, ex->e.ang);
  fprintf(src, "\texercise_t res = exercise_new(\n");
  printLine(ex->description, ex->descriptionSize, src);
  fprintf(src, ",");
  printLine(ex->codeEleve, ex->codeEleveSize, src);
  fprintf(src, ",");
  printLine(ex->codeProf, ex->codeProfSize, src);
  fprintf(src, ", w);\n");
  fprintf(src, "\treturn res;\n}\n");
}


int generateExerciseFile(exo_content *ex)
{
  printf("%s\n", ex->exercise_name);
  char* filename = malloc(sizeof(char)*strlen(ex->exercise_name)+strlen(ex->lesson_name)+4);
  sprintf(filename, "%s_%s.c",ex->lesson_name, ex->exercise_name);
  printf("Nom du fichier source a créer %s\n", filename);

  FILE* exerciseSrc = fopen(filename, "w");
  printInclude(ex, exerciseSrc);
  printExercise(ex, exerciseSrc);
  fclose(exerciseSrc);
  return 0;
}