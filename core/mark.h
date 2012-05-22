#ifndef INCLUDED_MARK_H
#define INCLUDED_MARK_H

#include "UI/CLE.h"

/* Remove the displayed mark of the world num_world */
void CLE_clear_mark(int num_world);

/*Show the mark of the current world selected*/
void CLE_show_mark();

/*Remove all mrak displayed */
void CLE_clear_worlds_mark();

/* Free all the marks of each world */
void CLE_free_mark_of_world(int num_world);

/* Add a mark associated to a line and a text to the specified world*/
void CLE_add_mark_to_world(char* message, int line, int type, int num_world);

/* Add a mark associated to a line and a text to the each world*/
void CLE_add_mark_to_all(char* message, int line, int type);

/*Return the message associate to the mark*/
char* get_message_for_mark(GtkSourceMark *mark);

#endif