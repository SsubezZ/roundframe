#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>
#include <gtk-layer-shell.h>

GtkWidget *create_window(GdkMonitor *monitor, int width, int height,
                        GtkLayerShellLayer layer, GCallback draw_func,
                        gpointer user_data, const char *name);
void on_realize_non_interactive(GtkWidget *widget, gpointer user_data);

#endif // WINDOW_H
