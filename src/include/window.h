#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>
#include <gtk-layer-shell.h>
#include "config.h"

void create_window_for_monitor(GtkApplication *app, GdkMonitor *monitor, Config *config, gint monitor_index, GtkCssProvider *provider);
void monitor_added(GdkDisplay *display, GdkMonitor *monitor, gpointer user_data);
void monitor_removed(GdkDisplay *display, GdkMonitor *monitor, gpointer user_data);

#endif // WINDOW_H
