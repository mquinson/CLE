/*
 * callbacks.c: defines all the callbacks, ie the functions called when the buttons get clicked
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdk.h>
#include <gtksourceview/gtksourceview.h>

#include "UI/CLE.h"
#include "core/exercise.h"
#include "core/lesson.h"
#include "core/world.h"

G_MODULE_EXPORT void
cb_run_clicked(GtkButton *button) {
	char *source = CLE_get_sourcecode();
	isrunning = 1;
	run = 1;

	/* Switch the notebook to the first page (which is #0), where the student code runs */
	gtk_notebook_set_current_page(global_data->world_views,0);

	// BEGINKILL
	printf("Execution\n");
	(*(global_data->lesson->e_curr->w_curr[0]->exercise_run))(global_data->lesson->e_curr,source);
	// REPLACE
	///* Display what should be complied. You should replace this with your own code */
	//CLE_log_append(strdup("Run clicked. We should compile that code:\n"));
	//CLE_log_append(strdup(source));
	// ENDKILL
	free(source);
}
G_MODULE_EXPORT void
cb_stop_clicked(GtkButton *button) {
	printf("Stop clicked\n");
	isrunning = 0;
	stop=1;
	run =0;
	step_by_step = 0;
	(*(global_data->lesson->e_curr->w_curr[0]->exercise_stop))(global_data->lesson);
}

G_MODULE_EXPORT void
cb_demo_clicked(GtkButton *button) {
	isrunning = 1;
	step_by_step = 0;
	stop = 0;
	run = 1;
	/* Switch the notebook to the second page (which is #1), where the demo is */
	gtk_notebook_set_current_page(global_data->world_views,1);

	(*(global_data->lesson->e_curr->w_curr[0]->exercise_demo))(global_data->lesson->e_curr);
}

G_MODULE_EXPORT void
cb_step_by_step_clicked(GtkButton *button) {
	printf("Step by step clicked\n");
	
	/* Switch the notebook to the first page (which is #0), where the student code runs */
	gtk_notebook_set_current_page(global_data->world_views,0);
	
	step_by_step = 1;
	stop = 0;
	run = 0;
	
	if(!isrunning){
		char *source = CLE_get_sourcecode();

		// BEGINKILL
		isrunning = 1;
		(*(global_data->lesson->e_curr->w_curr[0]->exercise_run))(global_data->lesson->e_curr,source);
	}
	
	// REPLACE
	///* Display what should be complied. You should replace this with your own code */
	//CLE_log_append(strdup("Run clicked. We should compile that code:\n"));
	//CLE_log_append(strdup(source));
	// ENDKILL
	//free(source);
}


G_MODULE_EXPORT void
world_selection_change(GtkComboBox *arg0, gpointer   user_data){
  if(gtk_combo_box_get_active(arg0) ==-1)
    return;
  
  global_data->current_world_expose = gtk_combo_box_get_active(arg0);
  world_ask_repaint(global_data->lesson->e_curr->w_curr[global_data->current_world_expose]);
  world_ask_repaint(global_data->lesson->e_curr->w_goal[global_data->current_world_expose]);
}

/* The about dialog window */
static GtkAboutDialog * diag = NULL;

G_MODULE_EXPORT void
cb_menu_about(GtkButton *button) {

	diag = CH_GET_OBJECT(global_data->builder, aboutdialog, GTK_ABOUT_DIALOG);
	gtk_widget_show(GTK_WIDGET(diag));
}

G_MODULE_EXPORT void
cb_scale_speed_value_changed(GtkAdjustment *adj, gpointer value) {
	//printf("Adj : %f\n",gtk_adjustment_get_value(adj));
	gdouble val = gtk_adjustment_get_value(adj);
	s = (int)val;
}

G_MODULE_EXPORT void
on_aboutdialog_close(GtkDialog *arg0, gpointer   user_data){
	gtk_widget_destroy(GTK_WIDGET(diag));
}



