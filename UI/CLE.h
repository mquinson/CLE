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
#include "core/log.h"


typedef struct CLE_data CLE_data_t;
CLE_data_t *global_data;

/* Asks the interface to change the lesson currently displayed by the renders */
void CLE_set_lesson(lesson_t l);
/* Asks the interface to change the exercise currently displayed by the renders */
void CLE_exercise_has_changed();

/* Retrieve the source code entered by the student into the source editor */
char *CLE_get_sourcecode(void);

int CLE_get_sourcecode_size(void);

int CLE_is_debug_mode();


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


#endif /* CLE_H_ */
