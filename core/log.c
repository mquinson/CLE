#include "core/log.h"
#include "UI/CLE.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void CLE_clear_worlds_log()
{
//     printf("CLE_clear_worlds_log call\n");
  int i;
  for(i=0; i<global_data->lesson->e_curr->worldAmount; ++i)
  {
    CLE_clear_logs_of_world(i);
  }
}

void CLE_clear_logs_of_world(int num_world)
{
  if(num_world == global_data->current_world_expose)
    CLE_log_clear();
//     printf("CLE_clear_logs_of_world call\n");
  if(global_data->worlds_log[num_world])
    free(global_data->worlds_log[num_world]);
  global_data->worlds_log[num_world]=malloc(sizeof(char)*1);
  global_data->worlds_log[num_world][0]='\0';
}

void CLE_add_log_for_world(char* text, int world_numero)
{
  if(!(global_data->worlds_log[world_numero]=realloc(global_data->worlds_log[world_numero], strlen(global_data->worlds_log[world_numero])+strlen(text)+2)))
  {
    perror("unable to realloc memory for worlds_log\n");
    exit(1);
  }
  strcat(global_data->worlds_log[world_numero], text);
  if(global_data->current_world_expose == world_numero)
  {
    CLE_log_clear();
    CLE_log_append(strdup(global_data->worlds_log[world_numero]));
  }
}

void CLE_add_log_to_all(char* text)
{
  int i;
  for(i=0; i<global_data->lesson->e_curr->worldAmount; ++i)
  {
    CLE_add_log_for_world(text, i);
  }
}


static gboolean log_append_from_main(gpointer data) {
  GtkTextIter end;
  GtkTextBuffer *buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_data->log_view));
  
  gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buff), &end);
  gtk_text_buffer_insert(buff,&end,(char*)data,-1);
  
  //    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buff), &end);
  gtk_text_buffer_place_cursor (buff, &end);
  gtk_widget_queue_draw(global_data->log_view);
  free(data);
  
  return FALSE; // We want to be executed only once.
}

static gboolean log_clear_from_main(gpointer data) {
  GtkTextIter start;
  GtkTextBuffer *buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_data->log_view));
  
  gtk_text_buffer_set_text(buff,"",-1);
  
  /* move cursor to the beginning */
  gtk_text_buffer_get_start_iter (buff, &start);
  gtk_text_buffer_place_cursor (buff, &start);
  gtk_widget_queue_draw(global_data->log_view);
  
  return FALSE;
}

void CLE_log_append(char*text) {
  // do not use directly the UI from external threads. Queue work for the main dude instead.
  g_idle_add(log_append_from_main,(void*)text);
}


void CLE_log_clear() {
  // do not use directly the UI from external threads. Queue work for the main dude instead.
  g_idle_add(log_clear_from_main,NULL);
}