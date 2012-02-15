#ifndef GENERATE_H
#define GENERATE_H

#include "parser.h"

int generateExerciseFile(exo_content *ex, lesson_content *lesson);
int generateLessonFile(lesson_content *lesson);
int generateExerciseHeader(lesson_content *lesson);


#endif