#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>
#include "drawing.h"
#include "logging.h"

gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data) {
  gint width = gtk_widget_get_allocated_width(widget);
  gint height = gtk_widget_get_allocated_height(widget);

  LOG("Window dimensions: %d x %d\n", width, height);

  cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint(cr);
  cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
  LOG("Area cleared with transparency\n");

  Config *config = (Config *)data;
  gint gap_left = config->gap_left;
  gint gap_right = config->gap_right;
  gint gap_top = config->gap_top;
  gint gap_bottom = config->gap_bottom;
  gdouble inner_radius = config->inner_radius;
  gboolean exclusive = config->exclusive;
  LOG("Using gaps: left=%d, right=%d, top=%d, bottom=%d, inner_radius=%f\n",
      gap_left, gap_right, gap_top, gap_bottom, inner_radius);
  LOG("Using exclusive: %s\n", exclusive ? "true" : "false");

  gint inner_width = width - gap_left - gap_right;
  gint inner_height = height - gap_top - gap_bottom;
  if (inner_radius < 0)
    inner_radius = 0;
  if (inner_radius > MIN(inner_width, inner_height) / 2)
    inner_radius = MIN(inner_width, inner_height) / 2;
  LOG("Using inner_radius: %f, inner dimensions: %d x %d\n", inner_radius,
      inner_width, inner_height);

  cairo_region_t *input_region = cairo_region_create();
  cairo_rectangle_int_t top = {0, 0, width, gap_top};
  if (gap_top > 0)
    cairo_region_union_rectangle(input_region, &top);
  cairo_rectangle_int_t bottom = {0, height - gap_bottom, width, gap_bottom};
  if (gap_bottom > 0)
    cairo_region_union_rectangle(input_region, &bottom);
  cairo_rectangle_int_t left = {0, gap_top, gap_left, height - gap_top - gap_bottom};
  if (gap_left > 0)
    cairo_region_union_rectangle(input_region, &left);
  cairo_rectangle_int_t right = {width - gap_right, gap_top, gap_right, height - gap_top - gap_bottom};
  if (gap_right > 0)
    cairo_region_union_rectangle(input_region, &right);
  if (inner_radius > 0) {
    cairo_rectangle_int_t corners[] = {
        {gap_left, gap_top, (int)inner_radius, (int)inner_radius},
        {width - gap_right - (int)inner_radius, gap_top, (int)inner_radius, (int)inner_radius},
        {width - gap_right - (int)inner_radius, height - gap_bottom - (int)inner_radius, (int)inner_radius, (int)inner_radius},
        {gap_left, height - gap_bottom - (int)inner_radius, (int)inner_radius, (int)inner_radius}};
    for (int i = 0; i < 4; i++) {
      cairo_region_union_rectangle(input_region, &corners[i]);
    }
  }
  gtk_widget_input_shape_combine_region(widget, input_region);
  cairo_region_destroy(input_region);
  LOG("Input shape region set for mouse passthrough\n");

  GtkStyleContext *outer_context = gtk_widget_get_style_context(widget);
  gtk_style_context_add_class(outer_context, "roundframe");
  gtk_render_background(outer_context, cr, 0, 0, width, height);
  LOG("Outer background rendered\n");
  gtk_render_frame(outer_context, cr, 0, 0, width, height);
  LOG("Outer frame rendered\n");

  GdkRGBA bg_color;
  if (gtk_style_context_lookup_color(outer_context, "theme_bg_color", &bg_color)) {
    LOG("Background color (theme_bg_color): r=%f, g=%f, b=%f, a=%f\n",
        bg_color.red, bg_color.green, bg_color.blue, bg_color.alpha);
  } else {
    LOG("Failed to lookup theme_bg_color\n");
  }

  cairo_new_path(cr);
  cairo_arc(cr, gap_left + inner_radius, gap_top + inner_radius, inner_radius, M_PI, 1.5 * M_PI);
  LOG("Top-left arc drawn for clear\n");
  cairo_arc(cr, width - gap_right - inner_radius, gap_top + inner_radius, inner_radius, 1.5 * M_PI, 2 * M_PI);
  LOG("Top-right arc drawn for clear\n");
  cairo_arc(cr, width - gap_right - inner_radius, height - gap_bottom - inner_radius, inner_radius, 0, 0.5 * M_PI);
  LOG("Bottom-right arc drawn for clear\n");
  cairo_arc(cr, gap_left + inner_radius, height - gap_bottom - inner_radius, inner_radius, 0.5 * M_PI, M_PI);
  LOG("Bottom-left arc drawn for clear\n");
  cairo_close_path(cr);
  LOG("Inner clear path closed\n");
  cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
  cairo_fill(cr);
  cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
  LOG("Inner area cleared\n");

  return FALSE;
}

void on_theme_changed(GObject *settings, GParamSpec *pspec, gpointer user_data) {
  GtkApplication *app = GTK_APPLICATION(user_data);
  GtkCssProvider *provider = g_object_get_data(G_OBJECT(app), "css-provider");

  GError *error_css = NULL;
  gchar *css_path = g_build_filename(g_get_user_config_dir(), "roundframe", "style.css", NULL);
  gtk_css_provider_load_from_path(provider, css_path, &error_css);
  if (error_css) {
    ERROR_LOG("Failed to reload CSS from %s: %s\n", css_path, error_css->message);
    g_error_free(error_css);
    gtk_css_provider_load_from_data(provider,
                                    ".roundframe { background-color: @theme_bg_color; border: 2px solid @theme_fg_color; }",
                                    -1, &error_css);
    if (error_css) {
      ERROR_LOG("Failed to load fallback CSS: %s\n", error_css->message);
      g_error_free(error_css);
    }
  } else {
    LOG("CSS reloaded due to theme change\n");
  }
  g_free(css_path);

  GList *windows = gtk_application_get_windows(app);
  for (GList *l = windows; l != NULL; l = l->next) {
    gtk_widget_queue_draw(GTK_WIDGET(l->data));
  }
}
