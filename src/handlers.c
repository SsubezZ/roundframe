#include "roundframe.h"

void on_monitor_changed(GdkDisplay *display, GdkMonitor *monitor,
                               GtkApplication *app) {
  (void)display;
  (void)monitor;
  destroy_all_windows();
  create_all_windows(app);
}

void on_theme_changed(GtkSettings *settings, GParamSpec *pspec,
                             gpointer user_data) {
  (void)settings;
  (void)pspec;
  (void)user_data;
  for (GList *iter = edge_windows; iter; iter = iter->next) {
    GtkWidget *win = GTK_WIDGET(iter->data);
    GtkWidget *box = gtk_bin_get_child(GTK_BIN(win));
    if (box)
      gtk_widget_queue_draw(box);
  }
  for (GList *iter = quad_windows; iter; iter = iter->next) {
    GtkWidget *win = GTK_WIDGET(iter->data);
    GtkWidget *drawing_area = gtk_bin_get_child(GTK_BIN(win));
    if (drawing_area)
      gtk_widget_queue_draw(drawing_area);
  }
}

gboolean reload_config_and_style(gpointer user_data) {
  GtkApplication *app = GTK_APPLICATION(user_data);
  g_message("Reloading config.ini and style.css...");
  destroy_all_windows();
  free_configs();
  load_config();
  load_css();
  create_all_windows(app);
  g_message("Reload complete");
  return G_SOURCE_CONTINUE;
}
