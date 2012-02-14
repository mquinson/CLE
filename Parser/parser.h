#ifndef PARSER_H
#define PARSER_H 
 
 
typedef struct world{
	int x;
	int y;
}world;

typedef struct entity{
	int x;
	int y;
	double ang;
}entity;

typedef struct exercise_desc{
  char* exerciseName;
  char* exerciseConstructor;
}exercise_desc;

typedef struct exo_content{
	world w;
	entity e;
	char* lesson_name;
	char* exercise_name;
	int descriptionSize;
	char** description;
	int codeEleveSize;
	char** codeEleve;
	int codeProfSize;
	char** codeProf;
}exo_content;

typedef struct lesson_content{
	char* lesson_name;
	int amount;
	exercise_desc** exercises;
}lesson_content;

#endif