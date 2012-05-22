/*
 * CLE.h: public functions of the graphical interface
 */

#ifndef CLE_H_
#define CLE_H_


#define ERROR_LOG 0
#define WARNING_LOG 1
#define INFO_LOG 2

#include <gtk/gtk.h>
#include <gtksourceview/gtksourcemark.h>
#include "core/lesson.h"


typedef struct CLE_data CLE_data_t;
CLE_data_t *global_data;

/* Asks the interface to change the lesson currently displayed by the renders */
void CLE_set_lesson(lesson_t l);
/* Asks the interface to change the exercise currently displayed by the renders */
void CLE_exercise_has_changed();

/* Retrieve the source code entered by the student into the source editor */
char *CLE_get_sourcecode(void);

int CLE_get_sourcecode_size(void);

void CLE_clear_worlds_log();

void CLE_clear_logs_of_world(int num_world);
/*Add text to the log of a specific world*/
void CLE_add_log_for_world(char* text, int world_numero);

void CLE_add_log_to_all(char* text);
/* Add text to the console area (the text is free()d after use, think about strdup()ing it) */
void CLE_log_append(char*text);
/* Clear the console area */
void CLE_log_clear();


/* The following shouldn't be accessed from outside of UI/ code */
/* Convenience macros for obtaining objects from UI file */
#define CH_GET_OBJECT( builder, name, type) \
    type( gtk_builder_get_object( builder, #name ) )
#define CH_GET_WIDGET( builder, name, data ) \
    data->name = CH_GET_OBJECT( builder, name, GTK_WIDGET)

    
    typedef struct {
      int line;
      GtkSourceMark* mark;
      char* message;
    }mark_data;
    
    typedef struct {
      int nb_mark;
      mark_data** marks;
    }listMarks;
    
    
/* Main data structure definition */
struct CLE_data
{
	/* Where to find all the widgets */
    GtkBuilder *builder;

    /* Widgets */
    GtkWidget *main_window;       /* Main application window */
    GtkWidget *drawing_world;     /* Drawing area where we draw the current world */
    GtkWidget *drawing_objective; /* Drawing area where we draw the goal world */
    GtkWidget *mission_view;      /* Text area where the mission is displayed */
    GtkWidget *source_view;       /* Source editor */
    GtkWidget *log_view;          /* Text area serving as a console */
    
    GtkComboBoxText *world_selection;
    GtkTreeStore *world_selection_model;

    GtkNotebook *world_views;

    /*Logs of different world*/
    char** worlds_log;
    /*List of marks for each world*/
    listMarks** worlds_mark;
    
    /* the current lesson */
    lesson_t lesson;
    int current_world_expose;
    
    int speed;
    int isrunning;
    int step_by_step;
    int stop;
    int run;
    int debug;
};



void world_ask_repaint(core_world_t w);


void CLE_dialog_success(void);
void CLE_dialog_failure(char *reason);


void CLE_clear_mark();
void CLE_show_mark();
void CLE_clear_worlds_mark();
void CLE_add_mark_to_world(char* message, int line, int type, int num_world);
void CLE_add_mark_to_all(char* message, int line, int type);

char* get_message_for_mark(GtkSourceMark *mark);

#endif /* CLE_H_ */
