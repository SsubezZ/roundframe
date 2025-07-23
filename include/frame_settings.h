#ifndef FRAME_SETTINGS_H
#define FRAME_SETTINGS_H

#include <gtk-layer-shell.h>
#include <gtk/gtk.h>

typedef enum {
  LAYER_TOP,
  LAYER_OVERLAY,
  LAYER_BOTTOM,
  LAYER_BACKGROUND
} LayerType;

typedef struct {
  int radius;
  int gap_top;
  int gap_right;
  int gap_bottom;
  int gap_left;
  int border_width;
  LayerType layer;
  double alpha_light;
  double alpha_dark;
  char *layer_str;
  char *gap_str;
  char *alpha_str;
} FrameSettings;

LayerType parse_layer_type(const char *str);
GtkLayerShellLayer get_layer_shell_layer(LayerType layer);
gboolean parse_arguments(FrameSettings *settings, int *argc, char ***argv);

#endif // FRAME_SETTINGS_H
