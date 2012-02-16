#ifndef STRUCTUTILITY_H
#define STRUCTUTILITY_H

#include "parser.h"//contains all structures definitions ans typedef

/*exercise_desc section*/
exercise_desc* newExerciseDescriptor(char* name, char* constructor);
void freeExerciseDescriptor(exercise_desc *ex);

/*entity section*/
entity* newEntity(int x, int y, double ang);
void freeEntity(entity* en);

/*world section*/
world* newWorld(double x, double y);
void freeWorld(world* w);

/*lesson_content section*/
lesson_content* newVoidLessonContent();
lesson_content* newLessonContent(char* name);
void generateLessonFromName(lesson_content* lesson, char* name);
void freeLessonContent(lesson_content* lesson);
void addExerciseToLesson(lesson_content* lesson, exercise_desc* desc);

/*exo_content section*/
exo_content* newVoidExoContent();
void addWorldToExoContent(exo_content* ex, world* w);
void addEntityToExoContent(exo_content* ex, entity* e);
void addDescription(exo_content* ex, char* description);
void addCodeEleve(exo_content* ex, char* codeEleve);
void addCodeProf(exo_content* ex, char* codeProf);
void setLessonName(exo_content* ex, char* lesson_name);
void setExerciseName(exo_content* ex, char* exercise_name);


#endif