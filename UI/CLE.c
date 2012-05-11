/* CLE.c: main function of the CLE project */

#define READ_BUFFER_SIZE   4096

#define MARK_TYPE   "error"
#define MAX_NB_LOG_ERRORS 10

#include <gtk/gtk.h>

#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#include "CLE.h"
#include "core/lesson.h"
#include "core/exercise.h"

#include <string.h> /* strlen */
#include <stdlib.h> /* free */
#include <unistd.h>/* access */


static listMarks list_marks = NULL;

/* Prototypes of manually connected signals */
G_MODULE_EXPORT void cb_can_redo_changed(GtkButton *button);
G_MODULE_EXPORT void cb_can_undo_changed(GtkButton *button);
G_MODULE_EXPORT void cb_menu_change_lesson(GtkMenuItem *menuitem, gpointer data);
G_MODULE_EXPORT void cb_menu_change_exercise(GtkMenuItem *menuitem, gpointer data);
G_MODULE_EXPORT void world_selection_change(GtkComboBox *arg0, gpointer   user_data);

static gchar * mark_tooltip_func (GtkSourceMark *mark, gpointer user_data) {
  GtkTextBuffer *buf;
  GtkTextIter iter;
  gint line;
  
  buf = gtk_text_mark_get_buffer (GTK_TEXT_MARK (mark));
  
  gtk_text_buffer_get_iter_at_mark (buf, &iter, GTK_TEXT_MARK (mark));
  line = gtk_text_iter_get_line (&iter) + 1;
  
  return exercice_get_log(global_data->lesson->e_curr,line);
}

int main(int argc, char **argv) {
    GError     *error = NULL;

    global_data = NULL;
    /* Init GTK+ */
    gtk_init( &argc, &argv );

    /* we use threads */
    g_thread_init(NULL);

    /* Allocate data structure */
    global_data = g_slice_new( CLE_data_t );
    global_data->lesson = NULL;
    global_data->current_world_expose =0;
    
    global_data->world_selection_model = gtk_tree_store_new(1, G_TYPE_STRING);

    /* Create new GtkBuilder object */
    global_data->builder = gtk_builder_new();
    /* Load UI from file. If error occurs, report it and quit application. */
    if( ! gtk_builder_add_from_file( global_data->builder, "CLE.glade", &error ) ) {
        g_warning( "%s", error->message );
        g_free( error );
        return( 1 );
    }

    /* Get objects from UI and assign them to our data structure */
    CH_GET_WIDGET(global_data->builder, main_window, global_data );
    CH_GET_WIDGET(global_data->builder, drawing_world, global_data );
    CH_GET_WIDGET(global_data->builder, drawing_objective, global_data );
    CH_GET_WIDGET(global_data->builder, mission_view, global_data );
    CH_GET_WIDGET(global_data->builder, source_view, global_data );
    CH_GET_WIDGET(global_data->builder, log_view, global_data );

    global_data->world_views = CH_GET_OBJECT(global_data->builder, world_views, GTK_NOTEBOOK);
    global_data->world_selection = CH_GET_OBJECT(global_data->builder, world_chooser, GTK_COMBO_BOX_TEXT);

    /* Setup the source_view by telling it that we will display C source */
    GtkSourceLanguageManager *lang_manager = gtk_source_language_manager_get_default();
    GtkSourceLanguage *c_lang= gtk_source_language_manager_get_language(lang_manager, "c");
    GtkSourceBuffer *buff= gtk_source_buffer_new_with_language(c_lang);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(global_data->source_view),GTK_TEXT_BUFFER(buff));
    gtk_source_view_set_show_line_marks (GTK_SOURCE_VIEW(global_data->source_view),1);
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(global_data->source_view),1);


    /* Add some accelerators */
    GtkAccelGroup *gag = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(global_data->main_window),gag);

    GtkWidget *item = CH_GET_OBJECT(global_data->builder,menu_quit,GTK_WIDGET);
    gtk_widget_add_accelerator(item,"activate",gag,'Q',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
    item = CH_GET_OBJECT(global_data->builder,menu_run,GTK_WIDGET);
    gtk_widget_add_accelerator(item,"activate",gag,'R',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
    item = CH_GET_OBJECT(global_data->builder,menu_demo,GTK_WIDGET);
    gtk_widget_add_accelerator(item,"activate",gag,'D',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
    item = CH_GET_OBJECT(global_data->builder,menu_stop,GTK_WIDGET);
    gtk_widget_add_accelerator(item,"activate",gag,'S',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
    item = CH_GET_OBJECT(global_data->builder,menu_undo,GTK_WIDGET);
    gtk_widget_add_accelerator(item,"activate",gag,'Z',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
    item = CH_GET_OBJECT(global_data->builder,menu_cut,GTK_WIDGET);
    gtk_widget_add_accelerator(item,"activate",gag,'X',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
    item = CH_GET_OBJECT(global_data->builder,menu_copy,GTK_WIDGET);
    gtk_widget_add_accelerator(item,"activate",gag,'C',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
    item = CH_GET_OBJECT(global_data->builder,menu_paste,GTK_WIDGET);
    gtk_widget_add_accelerator(item,"activate",gag,'V',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

    /* Automatically enable/disable undo/redo in menus */
	GtkSourceBuffer *sb = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_data->source_view)));
    GtkSourceUndoManager *undom = gtk_source_buffer_get_undo_manager(sb);
    g_signal_connect(G_OBJECT(undom),"can-redo-changed",G_CALLBACK(cb_can_redo_changed),NULL);
    g_signal_connect(G_OBJECT(undom),"can-undo-changed",G_CALLBACK(cb_can_undo_changed),NULL);
    item = CH_GET_OBJECT(global_data->builder,menu_redo,GTK_WIDGET);
	gtk_widget_set_sensitive(item, gtk_source_buffer_can_redo(sb));
    item = CH_GET_OBJECT(global_data->builder,menu_undo,GTK_WIDGET);
	gtk_widget_set_sensitive(item, gtk_source_buffer_can_undo(sb));

    /* Connect signals */
    gtk_builder_connect_signals( global_data->builder, global_data );
    
    gtk_combo_box_set_model((GtkComboBox *)global_data->world_selection, (GtkTreeModel *)global_data->world_selection_model);
    
    
    /*Define mark categorie*/
    GdkColor color;
    gdk_color_parse ("red", &color);
    gtk_source_view_set_mark_category_background (GTK_SOURCE_VIEW(global_data->source_view), MARK_TYPE, &color);
    gtk_source_view_set_mark_category_icon_from_stock (GTK_SOURCE_VIEW(global_data->source_view), MARK_TYPE, GTK_STOCK_NO);
    gtk_source_view_set_mark_category_priority (GTK_SOURCE_VIEW(global_data->source_view), MARK_TYPE, 1);
    gtk_source_view_set_mark_category_tooltip_markup_func (GTK_SOURCE_VIEW(global_data->source_view), MARK_TYPE, mark_tooltip_func, NULL, NULL);
    
    list_marks = malloc(sizeof(listMarks));
    list_marks->marks = malloc(sizeof(GtkSourceMark*)*MAX_NB_LOG_ERRORS);
    list_marks->nbMarks = 0;
    
    

    /* load the exercise (must be done before we show the widget) */
      //printf("%s\n", getenv("CD"));
      if(!access("./logo.so", F_OK))
	CLE_set_lesson(lesson_from_file(strdup("./logo.so")));
//    CLE_set_lesson(lesson_from_file(strdup("./recursion.so")));

    /* Show window & start main loop */
    gtk_widget_show( global_data->main_window );
    gtk_main();

    /* Free any allocated data */
    g_object_unref( G_OBJECT( global_data->builder ) );
    g_slice_free( CLE_data_t, global_data );

    return( 0 );
}

void CLE_set_lesson(lesson_t l) {
	if (l==NULL) /* error while loading? */
		return;
	/* Change the data model */
	if(global_data->lesson)
		lesson_free(global_data->lesson);
	global_data->lesson = l;
	lesson_set_exo(global_data->lesson, 0);
	/* Rebuild the menu */
	GtkMenuItem *menu_lesson = CH_GET_OBJECT(global_data->builder,menu_lesson,GTK_MENU_ITEM);
	GtkWidget *submenu = gtk_menu_new();
	GtkWidget *item = gtk_menu_item_new_with_mnemonic("_Change lesson");
    g_signal_connect(G_OBJECT(item),"activate",G_CALLBACK(cb_menu_change_lesson),NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
    gtk_menu_append(GTK_MENU_SHELL(submenu),gtk_separator_menu_item_new());
    // The exercise entries
    int it;
    for (it=0;it<l->amount;it++) {
    	item = gtk_menu_item_new_with_label(l->exos[it].label);
        g_signal_connect(G_OBJECT(item),"activate",G_CALLBACK(cb_menu_change_exercise),
        		&(l->exos[it].rank));
        gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
    }
    // Show it all
    gtk_widget_show_all(submenu);
	gtk_menu_item_set_submenu(menu_lesson,submenu);
}
void CLE_exercise_has_changed() {
	GtkSourceBuffer *sb;

	CLE_log_clear();

	gtk_text_buffer_set_text(
			gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_data->mission_view)),
			global_data->lesson->e_curr->mission,-1);

	sb = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_data->source_view)));
	gtk_source_buffer_begin_not_undoable_action (sb);
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(sb), global_data->lesson->e_curr->template, -1);
	gtk_source_buffer_end_not_undoable_action (sb);
    gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (sb), FALSE);
    
    gtk_tree_store_clear((GtkTreeStore*)gtk_combo_box_get_model((GtkComboBox*)global_data->world_selection));
    
    GtkTreeIter  iter;
    
    char buff[50];
    int i;
    for(i=0; i<global_data->lesson->e_curr->worldAmount; ++i)
    {
      sprintf(buff, "world %d", i);
      gtk_tree_store_append(global_data->world_selection_model,&iter,NULL);
      gtk_tree_store_set(global_data->world_selection_model, &iter ,0,buff,-1);
    }
    gtk_combo_box_set_active((GtkComboBox*)global_data->world_selection, 0);
}

char *CLE_get_sourcecode() {
    GtkTextIter start, end;
    char *text;
    GtkTextBuffer *buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_data->source_view));

    gtk_text_buffer_get_bounds (GTK_TEXT_BUFFER(buff), &start, &end);
    text = gtk_text_buffer_get_text (GTK_TEXT_BUFFER (buff), &start, &end, TRUE);

    return text;
}
/*
 * the Gnome UI shouldn't be accessed by more than one thread at the same time.
 * so, requests to the UI comming from the other threads should be executed by
 *   the main thread anyway. This unfortunately forces to copy with strdup every
 *   arguments passed to CLE_log_append. Pitty.
 */
static gboolean dialog_from_main(gpointer data) {
	GtkWidget *dialog;
	if (data) {
		dialog = gtk_message_dialog_new (GTK_WINDOW(global_data->main_window),
		                                 GTK_DIALOG_DESTROY_WITH_PARENT,
		                                 GTK_MESSAGE_ERROR,
		                                 GTK_BUTTONS_CLOSE,
		                                 "You failed the exercise:\n%s",
		                                 (char*)data);
		gtk_window_set_title(GTK_WINDOW(dialog),"Oops");
	} else {
		dialog = gtk_message_dialog_new (GTK_WINDOW(global_data->main_window),
		                                 GTK_DIALOG_DESTROY_WITH_PARENT,
		                                 GTK_MESSAGE_INFO,
		                                 GTK_BUTTONS_CLOSE,
		                                 "You passed the exercise");
		gtk_window_set_title(GTK_WINDOW(dialog),"Congratulations");
	}
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
	return FALSE;
}
void CLE_dialog_success(){
	g_idle_add(dialog_from_main,NULL);
}
void CLE_dialog_failure(char *reason) {
	g_idle_add(dialog_from_main,reason);
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

/* Forces a repaint of the widget presenting the world (either the current or the goal)
 * Actually, we simply invalidate the region, and it will get repainted the next time that
 *   the gtk_main_loop gets idle. So it should be called from an external thread.
 * */
void world_ask_repaint(core_world_t w){
	GtkWidget *widget = NULL;
	if (!global_data || !global_data->lesson || !global_data->lesson->e_curr)
		return; /* not ready yet to repaint stuff: still in init process */

	if (w == global_data->lesson->e_curr->w_curr[global_data->current_world_expose])
		widget = global_data->drawing_world;
	else if (w == global_data->lesson->e_curr->w_goal[global_data->current_world_expose])
		widget = global_data->drawing_objective;
	if (widget) {
		gtk_widget_queue_draw(widget);
		//printf("%p: ask redraw\n",g_thread_self());
	} /* else: ignoring invalid request */
}


/* This signal is called when a world displaying widget gets an expose event.
 * It calls world_redraw() with the right arguments
 */
G_MODULE_EXPORT gboolean
cb_expose_world( GtkWidget      *widget,
		         GdkEventExpose *event,
                 CLE_data_t     *data )
{
  if(global_data->lesson == NULL)
    return( TRUE );
  
    cairo_t *cr;
    core_world_t w;
    int sizeX,sizeY;

	//printf("%p: received an expose event\n",g_thread_self());

    /* Select the right world (current or objective) depending on the widget calling us*/
    if (widget == data->drawing_world)
      w = global_data->lesson->e_curr->w_curr[global_data->current_world_expose];
    else
      w = global_data->lesson->e_curr->w_goal[global_data->current_world_expose];

    /* Create cairo context from GdkWindow */
    cr = gdk_cairo_create( event->window );
    gdk_drawable_get_size(event->window,&sizeX,&sizeY);

    /* Ask the world to redraw as it should */
    w->world_repaint(w,cr,sizeX,sizeY);
    /* Destroy cairo context */
    cairo_destroy( cr );

    /* Return TRUE, since we handled this event */
    return( TRUE );
}

/* Callbacks to the menu that you don't have to change */
G_MODULE_EXPORT void
cb_menu_undo(GtkMenuItem *menuitem, gpointer data) {
	GtkTextView *sv = GTK_TEXT_VIEW(global_data->source_view);
	GtkSourceBuffer *sb = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(sv));
	if (gtk_source_buffer_can_undo(sb))
		gtk_source_buffer_undo(sb);
}
G_MODULE_EXPORT void
cb_menu_redo(GtkMenuItem *menuitem, gpointer data) {
	GtkTextView *sv = GTK_TEXT_VIEW(global_data->source_view);
	GtkSourceBuffer *sb = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(sv));
	if (gtk_source_buffer_can_redo(sb))
		gtk_source_buffer_redo(sb);
}
G_MODULE_EXPORT void
cb_menu_copy(GtkMenuItem *menuitem, gpointer data) {
	if (!gtk_widget_has_focus(global_data->source_view))
		return;

	GtkTextView *tv = GTK_TEXT_VIEW(global_data->source_view);
	GtkTextBuffer *tb = gtk_text_view_get_buffer(tv);
	GtkClipboard *clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);

	gtk_text_buffer_copy_clipboard(tb,clipboard);
}
G_MODULE_EXPORT void
cb_menu_cut(GtkMenuItem *menuitem, gpointer data) {
	if (!gtk_widget_has_focus(global_data->source_view))
		return;

	GtkTextView *tv = GTK_TEXT_VIEW(global_data->source_view);
	GtkTextBuffer *tb = gtk_text_view_get_buffer(tv);
	GtkClipboard *clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);

	gtk_text_buffer_cut_clipboard(tb,clipboard,TRUE);
}
G_MODULE_EXPORT void
cb_menu_paste(GtkMenuItem *menuitem, gpointer data) {
	if (!gtk_widget_has_focus(global_data->source_view))
		return;

	GtkTextView *tv = GTK_TEXT_VIEW(global_data->source_view);
	GtkTextBuffer *tb = gtk_text_view_get_buffer(tv);
	GtkClipboard *clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);

	gtk_text_buffer_paste_clipboard(tb,clipboard,NULL,TRUE);
}
G_MODULE_EXPORT void
cb_menu_delete(GtkMenuItem *menuitem, gpointer data) {
	if (!gtk_widget_has_focus(global_data->source_view))
		return;

	GtkTextView *tv = GTK_TEXT_VIEW(global_data->source_view);
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(tv);
	gtk_text_buffer_delete_selection (buffer, FALSE, TRUE);
}
G_MODULE_EXPORT void
cb_can_redo_changed(GtkButton *button) {
    GtkWidget *item = CH_GET_OBJECT(global_data->builder,menu_redo,GTK_WIDGET);
	GtkTextView *sv = GTK_TEXT_VIEW(global_data->source_view);
	GtkSourceBuffer *sb = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(sv));

	gtk_widget_set_sensitive(item, gtk_source_buffer_can_redo(sb));
}
G_MODULE_EXPORT void
cb_can_undo_changed(GtkButton *button) {
    GtkWidget *item = CH_GET_OBJECT(global_data->builder,menu_undo,GTK_WIDGET);
	GtkTextView *sv = GTK_TEXT_VIEW(global_data->source_view);
	GtkSourceBuffer *sb = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(sv));

	gtk_widget_set_sensitive(item, gtk_source_buffer_can_undo(sb));
}
G_MODULE_EXPORT void
cb_menu_change_lesson(GtkMenuItem *menuitem, gpointer data) {
	char *lesson_filename = NULL;
	/* Get the plugin name */
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new ("Choose your lesson",
					      GTK_WINDOW(global_data->main_window),
					      GTK_FILE_CHOOSER_ACTION_OPEN,
					      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					      NULL);

	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter,"Lesson plugins");
	gtk_file_filter_add_pattern (filter, "*.so");
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog),filter);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
	    lesson_filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	}
	gtk_widget_destroy (dialog);

	/* If the user picked a lesson, use it */
	if (lesson_filename)
		CLE_set_lesson(lesson_from_file(lesson_filename));
}
G_MODULE_EXPORT void
cb_menu_change_exercise(GtkMenuItem *menuitem, gpointer data) {
	int num=*(int*)data;
	lesson_set_exo(global_data->lesson,num);
}


void CLE_clear_mark() {
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_data->source_view));
  
  int i;
  for (i=0;i<list_marks->nbMarks;i++) {
    gtk_text_buffer_delete_mark(buffer,GTK_TEXT_MARK(list_marks->marks[i]));
  }
  list_marks->nbMarks=0;
  
}

void CLE_add_mark(int line) {
  char string_line[100];
  printf("add mark to line %d\n", line);
  sprintf(string_line, "%i",line);
  
  GtkSourceMark *mark= gtk_source_mark_new(NULL,MARK_TYPE);
  
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_data->source_view));
  
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_line (buffer, &iter, line-1);
  
  gtk_text_buffer_add_mark(buffer,GTK_TEXT_MARK(mark),&iter);
  
  // Save mark pt
  if (list_marks->nbMarks!=0 && list_marks->nbMarks%MAX_NB_LOG_ERRORS==0 ) {
    list_marks->marks = realloc(list_marks->marks, list_marks->nbMarks + MAX_NB_LOG_ERRORS);
    if (list_marks->marks==NULL) {
      perror("Realloc failed in CLE_add_mark\n");
      exit(1);
    }
  }
  list_marks->marks[list_marks->nbMarks] = mark;
  list_marks->nbMarks++;
  
  //printf("Erreur en ligne %d\n",line);
}
