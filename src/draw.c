#include "roundframe.h"

gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
  QuadData *qd = (QuadData *)user_data;
  const int R = qd->radius;
  const int horizontal_thickness = qd->horizontal_thickness;
  const int vertical_thickness = qd->vertical_thickness;
  const int QW = gtk_widget_get_allocated_width(widget);
  const int QH = gtk_widget_get_allocated_height(widget);
  const int Sx = 2 * R + 2 * horizontal_thickness;
  const int Sy = 2 * R + 2 * vertical_thickness;
  GtkStyleContext *context = gtk_widget_get_style_context(widget);
  cairo_surface_t *surf =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, Sx, Sy);
  cairo_t *c = cairo_create(surf);
  cairo_set_operator(c, CAIRO_OPERATOR_CLEAR);
  cairo_paint(c);
  cairo_set_operator(c, CAIRO_OPERATOR_SOURCE);
  gtk_render_background(context, c, 0, 0, Sx, Sy);
  const int c_x = R + horizontal_thickness;
  const int c_y = R + vertical_thickness;
  if (R > 0) {
    cairo_set_operator(c, CAIRO_OPERATOR_CLEAR);
    cairo_arc(c, c_x, c_y, R, 0, 2 * M_PI);
    cairo_fill(c);
  }
  int center_widget_x =
      (qd->quad == QUAD_TOP_LEFT || qd->quad == QUAD_BOTTOM_LEFT) ? QW : 0;
  int center_widget_y =
      (qd->quad == QUAD_TOP_LEFT || qd->quad == QUAD_TOP_RIGHT) ? QH : 0;
  int sx = c_x - center_widget_x;
  int sy = c_y - center_widget_y;
  if (R > 0) {
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
  }
  cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_set_source_surface(cr, surf, -sx, -sy);
  cairo_rectangle(cr, 0, 0, QW, QH);
  cairo_fill(cr);
  cairo_destroy(c);
  cairo_surface_destroy(surf);
  return TRUE;
}
