#include <gtk/gtk.h>

#include <gtksourceview/gtksourceview.h>
#include <string.h>
#include <stdlib.h>

#include "UI/CLE.h"
#include "core/mark.h"



void CLE_clear_mark(int num_world) {
  if(global_data->worlds_mark==NULL || num_world != global_data->current_world_expose)
    return;
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_data->source_view));
  
  int i;
  for (i=0;i<global_data->worlds_mark[num_world]->nb_mark ;i++) {
    /*Removing mark only if there are not already remove*/
    if(!gtk_text_mark_get_deleted(GTK_TEXT_MARK(global_data->worlds_mark[num_world]->marks[i]->mark)))
      gtk_text_buffer_delete_mark(buffer,GTK_TEXT_MARK(global_data->worlds_mark[num_world]->marks[i]->mark));
  }
}

void CLE_add_mark_to_all(char* message, int line, int type)
{
  int i;
  for(i=0; i<global_data->lesson->e_curr->worldAmount; ++i)
  {
    CLE_add_mark_to_world(message,line, type, i);
  }
}



void CLE_add_mark_to_world(char* message, int line, int type, int num_world) {
  GtkSourceMark *mark=NULL;
  char string_line[15];
  sprintf(string_line, "%i",line);
  
  
  if(type == ERROR_LOG)
    mark= gtk_source_mark_new(NULL,"error");
  else if(type == WARNING_LOG)
    mark= gtk_source_mark_new(NULL,"warning");
  else if(type == INFO_LOG)
    mark= gtk_source_mark_new(NULL,"info");
  else
    return;
  if(global_data->current_world_expose == num_world)
    CLE_clear_mark(num_world);
  
  // Save mark pt
    global_data->worlds_mark[num_world]->marks = realloc(global_data->worlds_mark[num_world]->marks,sizeof(mark_data)*(global_data->worlds_mark[num_world]->nb_mark + 1));
    if (global_data->worlds_mark[num_world]->marks==NULL) {
      perror("Realloc failed in CLE_add_mark\n");
      exit(1);
    }
    global_data->worlds_mark[num_world]->marks[global_data->worlds_mark[num_world]->nb_mark] = malloc(sizeof(mark_data));
    global_data->worlds_mark[num_world]->marks[global_data->worlds_mark[num_world]->nb_mark]->mark = mark;
    global_data->worlds_mark[num_world]->marks[global_data->worlds_mark[num_world]->nb_mark]->line = line;
    global_data->worlds_mark[num_world]->marks[global_data->worlds_mark[num_world]->nb_mark]->message = strdup(message);
    ++(global_data->worlds_mark[num_world]->nb_mark);
    
    if(global_data->current_world_expose == num_world)
      CLE_show_mark();
    
}

void CLE_free_mark_of_world(int num_world)
{
  int j;
  CLE_clear_mark(num_world);
  for(j=0; j<global_data->worlds_mark[num_world]->nb_mark; ++j)
  {
    if(global_data->worlds_mark[num_world]->marks[j] == NULL)
      continue;
      
    if(global_data->worlds_mark[num_world]->marks[j]->message)
    {
      free(global_data->worlds_mark[num_world]->marks[j]->message);
    }
    free(global_data->worlds_mark[num_world]->marks[j]);
  }
  global_data->worlds_mark[num_world]->marks=NULL;
  global_data->worlds_mark[num_world]->nb_mark=0;
}



void CLE_clear_worlds_mark()
{
//     printf("CLE_clear_worlds_mark call\n");
  
  int i;
  for(i=0; i<global_data->lesson->e_curr->worldAmount; ++i)
  {
//     printf("Suppress of all mark for world %d %p\n",i, global_data->worlds_mark[i]->marks);
    if(global_data->worlds_mark[i]->marks!=NULL)
      CLE_free_mark_of_world(i);

//     printf("CLE_clear_worlds_mark call...... end %d\n",i);
  }
    
}

char* get_message_for_mark(GtkSourceMark *mark)
{
  GtkTextBuffer *buf;
  GtkTextIter iter;
  gint line;
  
  buf = gtk_text_mark_get_buffer (GTK_TEXT_MARK (mark));
  
  gtk_text_buffer_get_iter_at_mark (buf, &iter, GTK_TEXT_MARK (mark));
  line = gtk_text_iter_get_line (&iter) + 1;
  
  int i=0;
  
  for(i=0; i<global_data->worlds_mark[global_data->current_world_expose]->nb_mark;++i)
  {
    if(global_data->worlds_mark[global_data->current_world_expose]->marks[i]->line == line)
      return strdup(global_data->worlds_mark[global_data->current_world_expose]->marks[i]->message);
  }
  return NULL;
}

void CLE_show_mark()
{
  int i;
  for(i=0; i<global_data->worlds_mark[global_data->current_world_expose]->nb_mark;++i)
  {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_data->source_view));
    
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_line (buffer, &iter, global_data->worlds_mark[global_data->current_world_expose]->marks[i]->line-1);
    
    gtk_text_buffer_add_mark(buffer,GTK_TEXT_MARK(global_data->worlds_mark[global_data->current_world_expose]->marks[i]->mark),&iter);
  }
}