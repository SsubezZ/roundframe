#ifndef ROUNDFRAME_H
#define ROUNDFRAME_H

#include "gtk-layer-shell.h"
#include <cairo.h>
#include <glib-unix.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <math.h>

/* Quadrant enum for identifying corners */
typedef enum {
  QUAD_TOP_LEFT,
  QUAD_TOP_RIGHT,
  QUAD_BOTTOM_LEFT,
  QUAD_BOTTOM_RIGHT
} QuadType;

/* Edge window configuration structure */
typedef struct {
  GtkLayerShellEdge edge;
  char *name; /* allocated string; must be freed */
  int thickness;
  gboolean exclusive;
  int margin_top, margin_bottom, margin_left, margin_right;
  const char *css; /* points to static class name (don't free) */
} EdgeWindowConfig;

/* Corner window configuration structure */
typedef struct {
  QuadType quad;
  char *name; /* allocated string; must be freed */
  int radius; /* Size of circle and half of square side */
  gboolean exclusive;
  int margin_top, margin_bottom, margin_left, margin_right;
  const char *css; /* points to static class name (don't free) */
} QuadWindowConfig;

/* Data for drawing corners */
typedef struct {
  QuadType quad;
  int radius;
  int horizontal_thickness;
  int vertical_thickness;
} QuadData;

/* Global lists for windows */
extern GList *edge_windows;
extern GList *quad_windows;

/* List of monitor indices to apply the frame to (NULL for all monitors) */
extern gint *enabled_monitors;
extern gsize num_enabled_monitors;
extern gboolean g_unique_names;

/* Default configurations */
extern EdgeWindowConfig *edge_cfgs;
extern unsigned num_edge_cfgs;
extern QuadWindowConfig *quad_cfgs;
extern unsigned num_quad_cfgs;

/* Global CSS provider */
extern GtkCssProvider *global_css;

/* Function prototypes */
gchar *find_config_file(const char *filename);
void free_configs(void);
void load_config(void);
void load_css(void);
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data);
void destroy_all_windows(void);
void create_edge_window(GtkApplication *app, const EdgeWindowConfig *cfg, GdkMonitor *monitor);
void create_quad_window(GtkApplication *app, const QuadWindowConfig *cfg, GdkMonitor *monitor);
void create_all_windows(GtkApplication *app);
void on_monitor_changed(GdkDisplay *display, GdkMonitor *monitor, GtkApplication *app);
void on_theme_changed(GtkSettings *settings, GParamSpec *pspec, gpointer user_data);
gboolean reload_config_and_style(gpointer user_data);
void activate(GtkApplication *app, gpointer user_data);

#endif /* ROUNDFRAME_H */
