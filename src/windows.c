#include "roundframe.h"

void destroy_all_windows(void) {
  while (edge_windows) {
    GtkWidget *win = GTK_WIDGET(edge_windows->data);
    gtk_widget_destroy(win);
    edge_windows = g_list_delete_link(edge_windows, edge_windows);
  }
  while (quad_windows) {
    GtkWidget *win = GTK_WIDGET(quad_windows->data);
    gtk_widget_destroy(win);
    quad_windows = g_list_delete_link(quad_windows, quad_windows);
  }
}

void create_edge_window(GtkApplication *app, const EdgeWindowConfig *cfg,
                               GdkMonitor *monitor) {
  if (cfg->thickness < 0)
    return; // ← Skip zero or negative
  GtkWindow *win = GTK_WINDOW(gtk_application_window_new(app));
  gtk_layer_init_for_window(win);
  gtk_layer_set_namespace(win, cfg->name);
  gtk_layer_set_layer(win, GTK_LAYER_SHELL_LAYER_TOP);
  if (monitor)
    gtk_layer_set_monitor(win, monitor);
  gtk_widget_set_app_paintable(GTK_WIDGET(win), TRUE);
  GdkScreen *screen = gtk_widget_get_screen(GTK_WIDGET(win));
  GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
  if (visual)
    gtk_widget_set_visual(GTK_WIDGET(win), visual);
  gboolean anchors[GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER] = {FALSE};
  anchors[cfg->edge] = TRUE;
  if (cfg->edge == GTK_LAYER_SHELL_EDGE_TOP ||
      cfg->edge == GTK_LAYER_SHELL_EDGE_BOTTOM) {
    anchors[GTK_LAYER_SHELL_EDGE_LEFT] = TRUE;
    anchors[GTK_LAYER_SHELL_EDGE_RIGHT] = TRUE;
  } else {
    anchors[GTK_LAYER_SHELL_EDGE_TOP] = TRUE;
    anchors[GTK_LAYER_SHELL_EDGE_BOTTOM] = TRUE;
  }
  for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++)
    gtk_layer_set_anchor(win, i, anchors[i]);
  if (cfg->edge == GTK_LAYER_SHELL_EDGE_TOP ||
      cfg->edge == GTK_LAYER_SHELL_EDGE_BOTTOM)
    gtk_widget_set_size_request(GTK_WIDGET(win), -1, cfg->thickness);
  else
    gtk_widget_set_size_request(GTK_WIDGET(win), cfg->thickness, -1);
  int exclusive = 0;
  if (cfg->exclusive) {
    exclusive = cfg->thickness;
    if (cfg->edge == GTK_LAYER_SHELL_EDGE_TOP)
      exclusive += cfg->margin_top;
    else if (cfg->edge == GTK_LAYER_SHELL_EDGE_BOTTOM)
      exclusive += cfg->margin_bottom;
    else if (cfg->edge == GTK_LAYER_SHELL_EDGE_LEFT)
      exclusive += cfg->margin_left;
    else if (cfg->edge == GTK_LAYER_SHELL_EDGE_RIGHT)
      exclusive += cfg->margin_right;
  }
  gtk_layer_set_exclusive_zone(win, exclusive);
  int margin_top = cfg->margin_top;
  int margin_bottom = cfg->margin_bottom;
  int margin_left = cfg->margin_left;
  int margin_right = cfg->margin_right;
  switch (cfg->edge) {
  case GTK_LAYER_SHELL_EDGE_TOP:
    if (cfg->thickness > 0) {
      margin_left += (quad_cfgs[0].radius > 0 ? quad_cfgs[0].radius : 0);
      margin_right += (quad_cfgs[1].radius > 0 ? quad_cfgs[1].radius : 0);
    }
    break;
  case GTK_LAYER_SHELL_EDGE_BOTTOM:
    if (cfg->thickness > 0) {
      margin_left += (quad_cfgs[2].radius > 0 ? quad_cfgs[2].radius : 0);
      margin_right += (quad_cfgs[3].radius > 0 ? quad_cfgs[3].radius : 0);
    }
    break;
  case GTK_LAYER_SHELL_EDGE_LEFT:
    if (cfg->thickness > 0) {
      margin_top += (quad_cfgs[0].radius > 0 ? quad_cfgs[0].radius : 0) +
                    (edge_cfgs[0].thickness > 0 ? edge_cfgs[0].thickness : 0);
      margin_bottom +=
          (quad_cfgs[2].radius > 0 ? quad_cfgs[2].radius : 0) +
          (edge_cfgs[1].thickness > 0 ? edge_cfgs[1].thickness : 0);
    }
    break;
  case GTK_LAYER_SHELL_EDGE_RIGHT:
    if (cfg->thickness > 0) {
      margin_top += (quad_cfgs[1].radius > 0 ? quad_cfgs[1].radius : 0) +
                    (edge_cfgs[0].thickness > 0 ? edge_cfgs[0].thickness : 0);
      margin_bottom +=
          (quad_cfgs[3].radius > 0 ? quad_cfgs[3].radius : 0) +
          (edge_cfgs[1].thickness > 0 ? edge_cfgs[1].thickness : 0);
    }
    break;
  default:
    break;
  }
  gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_TOP, margin_top);
  gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_BOTTOM, margin_bottom);
  gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_LEFT, margin_left);
  gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_RIGHT, margin_right);
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(win), box);
  if (cfg->css) {
    GtkStyleContext *ctx = gtk_widget_get_style_context(box);
    gtk_style_context_add_class(ctx, cfg->css);
  }
  gtk_widget_show_all(GTK_WIDGET(win));
  edge_windows = g_list_append(edge_windows, win);
}

void create_quad_window(GtkApplication *app, const QuadWindowConfig *cfg,
                               GdkMonitor *monitor) {
  if (cfg->radius < 0) {
    return;
  }
  int horizontal_thickness = 0;
  int vertical_thickness = 0;
  switch (cfg->quad) {
  case QUAD_TOP_LEFT:
    horizontal_thickness =
        edge_cfgs[2].thickness > 0 ? edge_cfgs[2].thickness : 0;
    vertical_thickness =
        edge_cfgs[0].thickness > 0 ? edge_cfgs[0].thickness : 0;
    break;
  case QUAD_TOP_RIGHT:
    horizontal_thickness =
        edge_cfgs[3].thickness > 0 ? edge_cfgs[3].thickness : 0;
    vertical_thickness =
        edge_cfgs[0].thickness > 0 ? edge_cfgs[0].thickness : 0;
    break;
  case QUAD_BOTTOM_LEFT:
    horizontal_thickness =
        edge_cfgs[2].thickness > 0 ? edge_cfgs[2].thickness : 0;
    vertical_thickness =
        edge_cfgs[1].thickness > 0 ? edge_cfgs[1].thickness : 0;
    break;
  case QUAD_BOTTOM_RIGHT:
    horizontal_thickness =
        edge_cfgs[3].thickness > 0 ? edge_cfgs[3].thickness : 0;
    vertical_thickness =
        edge_cfgs[1].thickness > 0 ? edge_cfgs[1].thickness : 0;
    break;
  }
  int size_h = cfg->radius + horizontal_thickness;
  int size_v = cfg->radius + vertical_thickness;
  GtkWindow *win = GTK_WINDOW(gtk_application_window_new(app));
  gtk_layer_init_for_window(win);
  gtk_layer_set_namespace(win, cfg->name);
  gtk_layer_set_layer(win, GTK_LAYER_SHELL_LAYER_TOP);
  if (monitor)
    gtk_layer_set_monitor(win, monitor);
  gtk_widget_set_app_paintable(GTK_WIDGET(win), TRUE);
  GdkScreen *screen = gtk_widget_get_screen(GTK_WIDGET(win));
  GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
  if (visual)
    gtk_widget_set_visual(GTK_WIDGET(win), visual);
  gboolean anchors[GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER] = {FALSE};
  switch (cfg->quad) {
  case QUAD_TOP_LEFT:
    anchors[GTK_LAYER_SHELL_EDGE_TOP] = TRUE;
    anchors[GTK_LAYER_SHELL_EDGE_LEFT] = TRUE;
    break;
  case QUAD_TOP_RIGHT:
    anchors[GTK_LAYER_SHELL_EDGE_TOP] = TRUE;
    anchors[GTK_LAYER_SHELL_EDGE_RIGHT] = TRUE;
    break;
  case QUAD_BOTTOM_LEFT:
    anchors[GTK_LAYER_SHELL_EDGE_BOTTOM] = TRUE;
    anchors[GTK_LAYER_SHELL_EDGE_LEFT] = TRUE;
    break;
  case QUAD_BOTTOM_RIGHT:
    anchors[GTK_LAYER_SHELL_EDGE_BOTTOM] = TRUE;
    anchors[GTK_LAYER_SHELL_EDGE_RIGHT] = TRUE;
    break;
  }
  for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++)
    gtk_layer_set_anchor(win, i, anchors[i]);
  gtk_widget_set_size_request(GTK_WIDGET(win), size_h, size_v);
  int exclusive = 0;
  if (cfg->exclusive) {
    int ex_h = size_h;
    int ex_v = size_v;
    switch (cfg->quad) {
    case QUAD_TOP_LEFT:
      ex_h += cfg->margin_left;
      ex_v += cfg->margin_top;
      break;
    case QUAD_TOP_RIGHT:
      ex_h += cfg->margin_right;
      ex_v += cfg->margin_top;
      break;
    case QUAD_BOTTOM_LEFT:
      ex_h += cfg->margin_left;
      ex_v += cfg->margin_bottom;
      break;
    case QUAD_BOTTOM_RIGHT:
      ex_h += cfg->margin_right;
      ex_v += cfg->margin_bottom;
      break;
    }
    exclusive = MAX(ex_h, ex_v);
  }
  gtk_layer_set_exclusive_zone(win, exclusive);
  int margin_top = cfg->margin_top;
  int margin_bottom = cfg->margin_bottom;
  int margin_left = cfg->margin_left;
  int margin_right = cfg->margin_right;
  switch (cfg->quad) {
  case QUAD_TOP_LEFT:
    if (edge_cfgs[0].exclusive && edge_cfgs[0].thickness > 0)
      margin_top -= edge_cfgs[0].thickness;
    if (edge_cfgs[2].exclusive && edge_cfgs[2].thickness > 0)
      margin_left -= edge_cfgs[2].thickness;
    break;
  case QUAD_TOP_RIGHT:
    if (edge_cfgs[0].exclusive && edge_cfgs[0].thickness > 0)
      margin_top -= edge_cfgs[0].thickness;
    if (edge_cfgs[3].exclusive && edge_cfgs[3].thickness > 0)
      margin_right -= edge_cfgs[3].thickness;
    break;
  case QUAD_BOTTOM_LEFT:
    if (edge_cfgs[1].exclusive && edge_cfgs[1].thickness > 0)
      margin_bottom -= edge_cfgs[1].thickness;
    if (edge_cfgs[2].exclusive && edge_cfgs[2].thickness > 0)
      margin_left -= edge_cfgs[2].thickness;
    break;
  case QUAD_BOTTOM_RIGHT:
    if (edge_cfgs[1].exclusive && edge_cfgs[1].thickness > 0)
      margin_bottom -= edge_cfgs[1].thickness;
    if (edge_cfgs[3].exclusive && edge_cfgs[3].thickness > 0)
      margin_right -= edge_cfgs[3].thickness;
    break;
  }
  gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_TOP, margin_top);
  gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_BOTTOM, margin_bottom);
  gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_LEFT, margin_left);
  gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_RIGHT, margin_right);
  GtkWidget *drawing_area = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(win), drawing_area);
  QuadData *qd = g_new(QuadData, 1);
  qd->quad = cfg->quad;
  qd->radius = cfg->radius;
  qd->horizontal_thickness = horizontal_thickness;
  qd->vertical_thickness = vertical_thickness;
  g_object_set_data_full(G_OBJECT(drawing_area), "quad-data", qd, g_free);
  g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), qd);
  if (cfg->css) {
    GtkStyleContext *ctx = gtk_widget_get_style_context(drawing_area);
    gtk_style_context_add_class(ctx, cfg->css);
  }
  gtk_widget_show_all(GTK_WIDGET(win));
  quad_windows = g_list_append(quad_windows, win);
}

void create_all_windows(GtkApplication *app) {
  GdkDisplay *display = gdk_display_get_default();
  int n_monitors = gdk_display_get_n_monitors(display);
  if (num_edge_cfgs != 4 || num_quad_cfgs != 4) {
    g_error("Configuration must define exactly 4 edges and 4 corners");
  }
  for (int m = 0; m < n_monitors; ++m) {
    if (num_enabled_monitors > 0) {
      gboolean enabled = FALSE;
      for (gsize i = 0; i < num_enabled_monitors; ++i) {
        if (enabled_monitors[i] == m) {
          enabled = TRUE;
          break;
        }
      }
      if (!enabled)
        continue;
    }
    GdkMonitor *monitor = gdk_display_get_monitor(display, m);
    for (unsigned i = 0; i < num_edge_cfgs; ++i) {
      create_edge_window(app, &edge_cfgs[i], monitor);
    }
    for (unsigned i = 0; i < num_quad_cfgs; ++i) {
      create_quad_window(app, &quad_cfgs[i], monitor);
    }
  }
}
