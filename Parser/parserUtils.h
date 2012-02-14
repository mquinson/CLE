#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

#define WRONG_BALISE 0
#define WORLD_BALISE 1
#define ENTITY_BALISE 2
#define LESSON_BALISE 3
#define EXERCISE_BALISE 4
#define DESCRIPTION_BALISE 5


void parseFile(exo_content* exoText, const char* filename);
void parseLessonFile(lesson_content *lesson, char* filename);


#endif