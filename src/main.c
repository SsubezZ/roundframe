#include "roundframe.h"

void activate(GtkApplication *app, gpointer user_data) {
  (void)user_data;
  if (edge_windows || quad_windows) {
    for (GList *l = edge_windows; l; l = l->next)
      gtk_window_present(GTK_WINDOW(l->data));
    for (GList *l = quad_windows; l; l = l->next)
      gtk_window_present(GTK_WINDOW(l->data));
    return;
  }
  load_css();
  create_all_windows(app);
  GdkDisplay *display = gdk_display_get_default();
  g_signal_connect(display, "monitor-added", G_CALLBACK(on_monitor_changed),
                   app);
  g_signal_connect(display, "monitor-removed", G_CALLBACK(on_monitor_changed),
                   app);
  g_unix_signal_add(SIGUSR1, reload_config_and_style, app);
  g_unix_signal_add(SIGHUP, reload_config_and_style, app);
  GtkSettings *settings = gtk_settings_get_default();
  g_signal_connect(settings, "notify::gtk-theme-name",
                   G_CALLBACK(on_theme_changed), NULL);
}

int main(int argc, char **argv) {
  load_config();
  GtkApplication *app =
      gtk_application_new("dev.subez.roundframe", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  destroy_all_windows();
  free_configs();
  if (global_css)
    g_object_unref(global_css);
  return status;
}
