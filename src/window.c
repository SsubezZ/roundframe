#include <gtk/gtk.h>
#include <gtk-layer-shell.h>
#include "window.h"
#include "logging.h"
#include "drawing.h"

void create_window_for_monitor(GtkApplication *app, GdkMonitor *monitor, Config *config, gint monitor_index, GtkCssProvider *provider) {
  Config *config_copy = g_memdup2(config, sizeof(Config));
  config_copy->layer_type = g_strdup(config->layer_type);
  config_copy->layer_name = g_strdup(config->layer_name);
  config_copy->keyboard_mode = g_strdup(config->keyboard_mode);

  GtkLayerShellLayer default_layer = GTK_LAYER_SHELL_LAYER_BOTTOM;
  if (config->layer_type && g_ascii_strcasecmp(config->layer_type, "TOP") == 0) {
    default_layer = GTK_LAYER_SHELL_LAYER_TOP;
  } else if (config->layer_type && g_ascii_strcasecmp(config->layer_type, "OVERLAY") == 0) {
    default_layer = GTK_LAYER_SHELL_LAYER_OVERLAY;
  }
  const char *layer_name = config->layer_name ? config->layer_name : "roundframe";

  GtkLayerShellKeyboardMode kb_mode = GTK_LAYER_SHELL_KEYBOARD_MODE_NONE;
  if (config->keyboard_mode) {
    if (g_ascii_strcasecmp(config->keyboard_mode, "exclusive") == 0) {
      kb_mode = GTK_LAYER_SHELL_KEYBOARD_MODE_EXCLUSIVE;
    } else if (g_ascii_strcasecmp(config->keyboard_mode, "on-demand") == 0) {
      kb_mode = GTK_LAYER_SHELL_KEYBOARD_MODE_ON_DEMAND;
    }
  }
  LOG("Keyboard mode set to %s\n", config->keyboard_mode ? config->keyboard_mode : "none");

  GtkWidget *window = gtk_application_window_new(app);
  gtk_layer_init_for_window(GTK_WINDOW(window));
  if (!gtk_layer_is_layer_window(GTK_WINDOW(window))) {
    ERROR_LOG("Warning: Failed to initialize layer shell for window on monitor %d\n", monitor_index);
  }
  gtk_layer_set_layer(GTK_WINDOW(window), default_layer);
  gtk_layer_set_namespace(GTK_WINDOW(window), layer_name);
  gtk_layer_set_monitor(GTK_WINDOW(window), monitor);
  gtk_layer_set_keyboard_mode(GTK_WINDOW(window), kb_mode);
  LOG("Created window for monitor %d\n", monitor_index);

  gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
  gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);
  gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_TOP, TRUE);
  gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);

  gtk_widget_set_app_paintable(window, TRUE);
  GdkScreen *screen = gdk_screen_get_default();
  GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
  if (visual != NULL && gdk_screen_is_composited(screen)) {
    gtk_widget_set_visual(window, visual);
  }

  gtk_layer_set_margin(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_LEFT, 0);
  gtk_layer_set_margin(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_RIGHT, 0);
  gtk_layer_set_margin(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_TOP, 0);
  gtk_layer_set_margin(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_BOTTOM, 0);

  if (config->exclusive) {
    gint exclusive_zone = 0;
    if (config->gap_top > 0 || config->inner_radius > 0) {
      exclusive_zone = config->gap_top + (gint)config->inner_radius;
      LOG("Exclusive zone set to %d pixels for top edge (gap_top + inner_radius)\n", exclusive_zone);
    } else if (config->gap_bottom > 0 || config->inner_radius > 0) {
      exclusive_zone = config->gap_bottom + (gint)config->inner_radius;
      LOG("Exclusive zone set to %d pixels for bottom edge (gap_bottom + inner_radius)\n", exclusive_zone);
    }
    gtk_layer_set_exclusive_zone(GTK_WINDOW(window), exclusive_zone);
    if (exclusive_zone == 0) {
      LOG("Exclusive zone not set (gaps and inner_radius are zero)\n");
    }
  } else {
    gtk_layer_set_exclusive_zone(GTK_WINDOW(window), 0);
    LOG("Exclusive zone disabled\n");
  }

  GtkWidget *drawing_area = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(window), drawing_area);

  g_signal_connect_data(drawing_area, "draw", G_CALLBACK(draw_callback), config_copy, (GClosureNotify)free_config, 0);

  gtk_widget_show_all(window);
}

void monitor_added(GdkDisplay *display, GdkMonitor *monitor, gpointer user_data) {
  GtkApplication *app = GTK_APPLICATION(user_data);
  Config *config = g_object_get_data(G_OBJECT(app), "config");
  GtkCssProvider *provider = g_object_get_data(G_OBJECT(app), "css-provider");
  gint n_monitors = gdk_display_get_n_monitors(display);
  create_window_for_monitor(app, monitor, config, n_monitors - 1, provider);
  LOG("Monitor added, created window\n");
}

void monitor_removed(GdkDisplay *display, GdkMonitor *monitor, gpointer user_data) {
  GtkApplication *app = GTK_APPLICATION(user_data);
  GList *windows = gtk_application_get_windows(app);
  for (GList *l = windows; l != NULL; l = l->next) {
    GtkWidget *window = GTK_WIDGET(l->data);
    GdkMonitor *window_monitor = gtk_layer_get_monitor(GTK_WINDOW(window));
    if (window_monitor == monitor) {
      gtk_widget_destroy(window);
      LOG("Monitor removed, destroyed window\n");
      break;
    }
  }
}
