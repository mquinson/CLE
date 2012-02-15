#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

#define WRONG_BALISE 0
#define WORLD_BALISE 1
#define ENTITY_BALISE 2
#define LESSON_BALISE 3
#define EXERCISE_BALISE 4
#define DESCRIPTION_BALISE 5

#include "parser.h"

void parseFile(exo_content* exoText, const char* filename);
void parseLessonFile(lesson_content *lesson, exo_content *exoText);
void addToLesson(exo_content *exoText, lesson_content *lesson);

exercise_desc* generateExerciseDescriptor(exo_content *ex);

#endif