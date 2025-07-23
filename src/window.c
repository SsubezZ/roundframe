#include <gtk/gtk.h>
#include <gtk-layer-shell.h>
#include "window.h"
#include "logging.h"

GtkWidget *create_window(GdkMonitor *monitor, int width, int height,
                        GtkLayerShellLayer layer, GCallback draw_func,
                        gpointer user_data, const char *name) {
  log_debug("Creating window '%s' (%dx%d)\n", name, width, height);

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_app_paintable(window, TRUE);
  gtk_window_set_title(GTK_WINDOW(window), name);

  GdkScreen *screen = gtk_widget_get_screen(window);
  GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
  if (visual && gdk_screen_is_composited(screen)) {
    log_debug("Using RGBA visual\n");
    gtk_widget_set_visual(window, visual);
  } else {
    log_debug("WARNING: No RGBA visual available\n");
  }

  gtk_layer_init_for_window(GTK_WINDOW(window));
  gtk_layer_set_namespace(GTK_WINDOW(window), "roundframe");
  gtk_layer_set_layer(GTK_WINDOW(window), layer);
  gtk_layer_set_monitor(GTK_WINDOW(window), monitor);

  gtk_widget_set_events(window, 0);
  gtk_widget_set_can_focus(window, FALSE);
  g_signal_connect(window, "realize", G_CALLBACK(on_realize_non_interactive), NULL);

  gtk_window_set_default_size(GTK_WINDOW(window), width, height);
  g_signal_connect(window, "draw", draw_func, user_data);

  return window;
}

void on_realize_non_interactive(GtkWidget *widget, gpointer user_data) {
  log_debug("Window realized - setting input region\n");
  GdkWindow *gdk_window = gtk_widget_get_window(widget);
  cairo_region_t *region = cairo_region_create();
  gdk_window_input_shape_combine_region(gdk_window, region, 0, 0);
  cairo_region_destroy(region);
}
