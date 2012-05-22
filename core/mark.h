#ifndef INCLUDED_MARK_H
#define INCLUDED_MARK_H

#include "UI/CLE.h"


void CLE_clear_mark(int num_world);
void CLE_show_mark();
void CLE_clear_worlds_mark();
void CLE_free_mark_of_world(int num_world);
void CLE_add_mark_to_world(char* message, int line, int type, int num_world);
void CLE_add_mark_to_all(char* message, int line, int type);

char* get_message_for_mark(GtkSourceMark *mark);

#endif