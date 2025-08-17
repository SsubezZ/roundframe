#ifndef CONFIG_H
#define CONFIG_H

#include <glib.h>

typedef struct {
  const char *layer_type;
  const char *layer_name;
  const char *keyboard_mode;
  gint gap_left;
  gint gap_right;
  gint gap_top;
  gint gap_bottom;
  gdouble inner_radius;
  gboolean exclusive;
} Config;

void free_config(gpointer data);
gboolean load_config(Config *config, const char *config_path);

#endif // CONFIG_H
