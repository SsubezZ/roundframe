#include <gtk/gtk.h>
#include <gtk-layer-shell.h>
#include "frame_settings.h"
#include "logging.h"
#include "window.h"
#include "drawing.h"

static void activate(GtkApplication *app, gpointer user_data) {
  FrameSettings *settings = (FrameSettings *)user_data;
  GdkDisplay *display = gdk_display_get_default();
  int num_monitors = gdk_display_get_n_monitors(display);
  log_debug("Found %d monitors\n", num_monitors);

  for (int i = 0; i < num_monitors; i++) {
    GdkMonitor *monitor = gdk_display_get_monitor(display, i);
    GdkRectangle geometry;
    gdk_monitor_get_geometry(monitor, &geometry);

    log_debug("Monitor %d: %dx%d at %d,%d\n", i, geometry.width,
              geometry.height, geometry.x, geometry.y);

    GtkLayerShellLayer layer = get_layer_shell_layer(settings->layer);

    GtkWidget *frame_window =
        create_window(monitor, geometry.width, geometry.height, layer,
                      G_CALLBACK(draw_frame), settings, "RoundFrame");
    gtk_layer_set_anchor(GTK_WINDOW(frame_window), GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(frame_window), GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(frame_window), GTK_LAYER_SHELL_EDGE_TOP, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(frame_window), GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);

    gtk_window_set_application(GTK_WINDOW(frame_window), app);
    gtk_widget_show_all(frame_window);
  }
}

int main(int argc, char *argv[]) {
  FrameSettings settings = {
      .radius = 12,
      .gap_top = 1,
      .gap_right = 5,
      .gap_bottom = 5,
      .gap_left = 5,
      .border_width = 0,
      .layer = LAYER_BOTTOM,
      .alpha_light = 1.0,
      .alpha_dark = 0.75,
      .layer_str = NULL,
      .gap_str = NULL,
      .alpha_str = NULL
  };

  if (!parse_arguments(&settings, &argc, &argv)) {
    return 1;
  }

  if (settings.alpha_light < 0.0 || settings.alpha_light > 1.0 ||
      settings.alpha_dark < 0.0 || settings.alpha_dark > 1.0) {
    fprintf(stderr, "Error: Alpha values must be between 0.0 and 1.0\n");
    g_free(settings.layer_str);
    g_free(settings.gap_str);
    g_free(settings.alpha_str);
    return 1;
  }

  if (settings.radius < 0 || settings.border_width < 0) {
    fprintf(stderr, "Error: Radius and border width must be non-negative\n");
    g_free(settings.layer_str);
    g_free(settings.gap_str);
    g_free(settings.alpha_str);
    return 1;
  }

  GtkApplication *app = gtk_application_new("com.subez.roundframe", G_APPLICATION_DEFAULT_FLAGS);
  if (!app) {
    fprintf(stderr, "Error: Failed to create GTK application\n");
    g_free(settings.layer_str);
    g_free(settings.gap_str);
    g_free(settings.alpha_str);
    return 1;
  }

  g_signal_connect(app, "activate", G_CALLBACK(activate), &settings);
  int status = g_application_run(G_APPLICATION(app), argc, argv);

  g_free(settings.layer_str);
  g_free(settings.gap_str);
  g_free(settings.alpha_str);
  g_object_unref(app);
  return status;
}
