/* world_view.h: what's needed to display a world */

#ifndef WORLD_VIEW_H_
#define WORLD_VIEW_H_

/* Painting the world */
#include "gdk/gdk.h"
/* recompute what should be displayed (called from expose event) */
void world_redraw(world_t w, cairo_t *cr,int sizeX,int sizeY);
/* ask the widget to repaint itself (makes sure that an expose event will occure soon) */
void world_ask_repaint(world_t w);

#endif /* WORLD_VIEW_H_ */
