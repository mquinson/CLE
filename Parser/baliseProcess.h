#ifndef BALISE_PROCESS_H
#define BALISE_PROCESS_H

#include "parser.h"

void processWorldBalise(char* line, exo_content* ex);
void processEntityBalise(char* line, exo_content* ex);
void processLessonBalise(char* line, exo_content* ex);
void processExerciseBalise(char* line, exo_content* ex);

#endif