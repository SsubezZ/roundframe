#include "roundframe.h"

GList *edge_windows = NULL;
GList *quad_windows = NULL;
gint *enabled_monitors = NULL;
gsize num_enabled_monitors = 0;
gboolean g_unique_names = TRUE;
EdgeWindowConfig *edge_cfgs = NULL;
unsigned num_edge_cfgs = 0;
QuadWindowConfig *quad_cfgs = NULL;
unsigned num_quad_cfgs = 0;
GtkCssProvider *global_css = NULL;
