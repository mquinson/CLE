/*
 * callbacks.c: defines all the callbacks, ie the functions called when the buttons get clicked
 */

#include <stdlib.h>
#include <string.h>

#include <gdk/gdk.h>
#include <gtksourceview/gtksourceview.h>

#include "UI/CLE.h"
#include "core/exercise.h"
#include "core/lesson.h"

G_MODULE_EXPORT void
cb_run_clicked(GtkButton *button) {
  if(global_data->lesson == NULL)
    return;
	char *source = CLE_get_sourcecode();;

	/* Switch the notebook to the first page (which is #0), where the student code runs */
	gtk_notebook_set_current_page(global_data->world_views,0);

	// BEGINKILL
	(*(global_data->lesson->e_curr->w_curr->exercise_run))(global_data->lesson->e_curr,source);
	// REPLACE
	///* Display what should be complied. You should replace this with your own code */
	//CLE_log_append(strdup("Run clicked. We should compile that code:\n"));
	//CLE_log_append(strdup(source));
	// ENDKILL
	free(source);
}
G_MODULE_EXPORT void
cb_stop_clicked(GtkButton *button) {
  if(global_data->lesson == NULL)
    return;
	printf("Stop clicked\n");
	(*(global_data->lesson->e_curr->w_curr->exercise_stop))(global_data->lesson);
}

G_MODULE_EXPORT void
cb_demo_clicked(GtkButton *button) {
  if(global_data->lesson == NULL)
    return;
	/* Switch the notebook to the second page (which is #1), where the demo is */
	gtk_notebook_set_current_page(global_data->world_views,1);

	(*(global_data->lesson->e_curr->w_curr->exercise_demo))(global_data->lesson->e_curr);
}

/* The about dialog window */
static GtkAboutDialog * diag = NULL;

G_MODULE_EXPORT void
cb_menu_about(GtkButton *button) {

	diag = CH_GET_OBJECT(global_data->builder, aboutdialog, GTK_ABOUT_DIALOG);
	gtk_widget_show(GTK_WIDGET(diag));
}

G_MODULE_EXPORT void
on_aboutdialog_close(GtkDialog *arg0, gpointer   user_data){
	gtk_widget_destroy(GTK_WIDGET(diag));
}

