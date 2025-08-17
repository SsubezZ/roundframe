#ifndef DRAWING_H
#define DRAWING_H

#include <cairo.h>
#include <gtk/gtk.h>
#include "config.h"

gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data);
void on_theme_changed(GObject *settings, GParamSpec *pspec, gpointer user_data);

#endif // DRAWING_H
