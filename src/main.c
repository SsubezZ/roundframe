#include <gtk/gtk.h>
#include "config.h"
#include "logging.h"
#include "window.h"
#include "lockfile.h"
#include "drawing.h"

static void show_help_and_exit(void) {
  printf("Usage: roundframe [OPTIONS]\n");
  printf("Options:\n");
  printf("  -l, --log    Enable logging\n");
  printf("  -h, --help   Show help message\n");
  exit(0);
}

static gint handle_local_options(GApplication *app, GVariantDict *options, gpointer user_data) {
  if (g_variant_dict_contains(options, "help")) {
    show_help_and_exit();
    return 0;
  }

  if (g_variant_dict_contains(options, "log")) {
    logging_enabled = TRUE;
    LOG("Logging enabled\n");
  }

  GError *error = NULL;
  if (!g_application_register(app, NULL, &error)) {
    ERROR_LOG("Failed to register application: %s\n", error->message);
    g_error_free(error);
    if (check_if_running()) {
      return 1;
    }
    return -1;
  }

  if (g_application_get_is_remote(app)) {
    ERROR_LOG("Another instance of roundframe is already running (via D-Bus)\n");
    return 1;
  }

  LOG("Application registered successfully\n");
  g_application_hold(app);
  LOG("Application held\n");
  return -1;
}

static gint command_line_handler(GApplication *app, GApplicationCommandLine *command_line, gpointer user_data) {
  GVariantDict *options = g_application_command_line_get_options_dict(command_line);
  if (g_variant_dict_contains(options, "log")) {
    LOG("Logging enabled via command-line\n");
  }
  g_application_activate(app);
  return 0;
}

static void activate(GtkApplication *app, gpointer user_data) {
  if (check_if_running()) {
    ERROR_LOG("Exiting due to existing instance (lock file check)\n");
    return;
  }

  Config *config = g_new0(Config, 1);
  gchar *config_path = g_build_filename(g_get_user_config_dir(), "roundframe", "config.ini", NULL);
  if (!load_config(config, config_path)) {
    g_free(config);
    g_free(config_path);
    return;
  }
  g_free(config_path);

  GtkCssProvider *provider = gtk_css_provider_new();
  GError *error = NULL;
  gchar *css_path = g_build_filename(g_get_user_config_dir(), "roundframe", "style.css", NULL);
  if (!gtk_css_provider_load_from_path(provider, css_path, &error)) {
    ERROR_LOG("Failed to load CSS from %s: %s\n", css_path, error->message);
    g_error_free(error);
    gtk_css_provider_load_from_data(provider,
                                    ".roundframe { background-color: @theme_bg_color; border: 2px solid @theme_fg_color; }",
                                    -1, &error);
    if (error) {
      ERROR_LOG("Failed to load fallback CSS: %s\n", error->message);
      g_error_free(error);
    }
  }
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider),
                                            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_free(css_path);

  g_object_set_data_full(G_OBJECT(app), "config", config, (GDestroyNotify)free_config);
  g_object_set_data_full(G_OBJECT(app), "css-provider", provider, (GDestroyNotify)g_object_unref);

  GtkSettings *settings = gtk_settings_get_default();
  g_signal_connect(settings, "notify::gtk-theme-name", G_CALLBACK(on_theme_changed), app);

  GdkDisplay *display = gdk_display_get_default();
  g_signal_connect(display, "monitor-added", G_CALLBACK(monitor_added), app);
  g_signal_connect(display, "monitor-removed", G_CALLBACK(monitor_removed), app);

  gint n_monitors = gdk_display_get_n_monitors(display);
  for (gint i = 0; i < n_monitors; i++) {
    GdkMonitor *monitor = gdk_display_get_monitor(display, i);
    create_window_for_monitor(app, monitor, config, i, provider);
  }
}

int main(int argc, char **argv) {
  GtkApplication *app = gtk_application_new("org.subez.roundframe", G_APPLICATION_HANDLES_COMMAND_LINE);
  g_application_add_main_option(G_APPLICATION(app), "log", 'l', 0, G_OPTION_ARG_NONE, "Enable logging", NULL);
  g_application_add_main_option(G_APPLICATION(app), "help", 'h', 0, G_OPTION_ARG_NONE, "Show help message", NULL);

  g_signal_connect(app, "handle-local-options", G_CALLBACK(handle_local_options), NULL);
  g_signal_connect(app, "command-line", G_CALLBACK(command_line_handler), NULL);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  g_signal_connect(app, "shutdown", G_CALLBACK(cleanup_lock_file), NULL);

  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
