#include <cairo.h>
#include <gtk/gtk.h>
#include "drawing.h"
#include "frame_settings.h"
#include "color.h"
#include "logging.h"

#define MAX2(a, b) ((a) > (b) ? (a) : (b))

void rounded_rectangle(cairo_t *cr, double x, double y, double w, double h, double r) {
  const double PI = 3.14159265358979323846;
  const double PI_2 = PI / 2.0;
  cairo_new_sub_path(cr);
  cairo_arc(cr, x + w - r, y + r, r, -PI_2, 0);
  cairo_arc(cr, x + w - r, y + h - r, r, 0, PI_2);
  cairo_arc(cr, x + r, y + h - r, r, PI_2, PI);
  cairo_arc(cr, x + r, y + r, r, PI, 3 * PI_2);
  cairo_close_path(cr);
}

gboolean draw_frame(GtkWidget *widget, cairo_t *cr, gpointer data) {
  FrameSettings *settings = (FrameSettings *)data;
  Color bg, border;
  double effective_alpha;
  get_gtk_colors(settings, &bg, &border, &effective_alpha);

  int width = gtk_widget_get_allocated_width(widget);
  int height = gtk_widget_get_allocated_height(widget);

  cairo_set_source_rgba(cr, 0, 0, 0, 0);
  cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(cr);
  cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

  double inner_x = settings->gap_left;
  double inner_y = settings->gap_top;
  double inner_w = width - settings->gap_left - settings->gap_right;
  double inner_h = height - settings->gap_top - settings->gap_bottom;
  double border_offset = settings->border_width / 2.0;

  cairo_set_antialias(cr, CAIRO_ANTIALIAS_SUBPIXEL);

  cairo_set_source_rgba(cr, bg.r, bg.g, bg.b, bg.a * effective_alpha);
  cairo_rectangle(cr, 0, 0, width, height);

  double adjusted_radius = MAX2(0, settings->radius - border_offset);
  rounded_rectangle(cr, inner_x + border_offset, inner_y + border_offset,
                    inner_w - 2 * border_offset, inner_h - 2 * border_offset,
                    adjusted_radius);

  cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
  cairo_fill(cr);

  cairo_set_source_rgba(cr, bg.r, bg.g, bg.b, bg.a * effective_alpha);
  rounded_rectangle(cr, inner_x, inner_y, inner_w, inner_h, settings->radius);
  rounded_rectangle(cr, inner_x + border_offset, inner_y + border_offset,
                    inner_w - 2 * border_offset, inner_h - 2 * border_offset,
                    adjusted_radius);

  cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
  cairo_fill(cr);

  if (settings->border_width > 0) {
    cairo_set_source_rgba(cr, border.r, border.g, border.b, border.a);
    cairo_set_line_width(cr, settings->border_width);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    rounded_rectangle(cr, inner_x + border_offset, inner_y + border_offset,
                      inner_w - 2 * border_offset, inner_h - 2 * border_offset,
                      adjusted_radius);

    cairo_stroke(cr);
  }

  return FALSE;
}
