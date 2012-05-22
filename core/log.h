#ifndef INCLUDED_LOG_H
#define INCLUDED_LOG_H

#include "UI/CLE.h"

void CLE_clear_worlds_log();

void CLE_clear_logs_of_world(int num_world);
/*Add text to the log of a specific world*/
void CLE_add_log_for_world(char* text, int world_numero);

void CLE_add_log_to_all(char* text);
/* Add text to the console area (the text is free()d after use, think about strdup()ing it) */
void CLE_log_append(char*text);
/* Clear the console area */
void CLE_log_clear();


#endif