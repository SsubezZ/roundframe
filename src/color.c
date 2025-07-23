#include <gtk/gtk.h>
#include "color.h"
#include "frame_settings.h"
#include "logging.h"

void get_gtk_colors(FrameSettings *settings, Color *bg, Color *border, double *effective_alpha) {
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_realize(window);
  GtkStyleContext *context = gtk_widget_get_style_context(window);

  GdkRGBA gtk_bg, gtk_border;

  if (!gtk_style_context_lookup_color(context, "theme_bg_color", &gtk_bg)) {
    log_debug("WARNING: Using fallback bg color\n");
    gtk_bg.red = gtk_bg.green = gtk_bg.blue = 0.1;
    gtk_bg.alpha = 1.0;
  }

  if (!gtk_style_context_lookup_color(context, "borders", &gtk_border)) {
    log_debug("WARNING: Using fallback border color\n");
    gtk_border.red = gtk_border.green = gtk_border.blue = 0.8;
    gtk_border.alpha = 1.0;
  }

  bg->r = gtk_bg.red;
  bg->g = gtk_bg.green;
  bg->b = gtk_bg.blue;
  bg->a = gtk_bg.alpha;

  border->r = gtk_border.red;
  border->g = gtk_border.green;
  border->b = gtk_border.blue;
  border->a = gtk_border.alpha;

  double luminance = 0.2126 * bg->r + 0.7152 * bg->g + 0.0722 * bg->b;
  *effective_alpha = (luminance < 0.5) ? settings->alpha_dark : settings->alpha_light;

  log_debug("GTK Colors - BG: (%.2f,%.2f,%.2f,%.2f) Border: (%.2f,%.2f,%.2f,%.2f)\n",
            bg->r, bg->g, bg->b, bg->a, border->r, border->g, border->b, border->a);
  log_debug("Theme luminance: %.2f, using alpha: %.2f\n", luminance, *effective_alpha);

  gtk_widget_destroy(window);
}
