#ifndef DRAWING_H
#define DRAWING_H

#include <cairo.h>
#include <gtk/gtk.h>

void rounded_rectangle(cairo_t *cr, double x, double y, double w, double h,
                       double r);
gboolean draw_frame(GtkWidget *widget, cairo_t *cr, gpointer data);

#endif // DRAWING_H
