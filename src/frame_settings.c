#include <gtk/gtk.h>
#include <gtk-layer-shell.h>
#include <string.h>
#include <stdlib.h>
#include "frame_settings.h"
#include "logging.h"

static int logging_enabled = 0;

LayerType parse_layer_type(const char *str) {
  if (str == NULL)
    return LAYER_TOP;
  if (strcmp(str, "overlay") == 0)
    return LAYER_OVERLAY;
  if (strcmp(str, "bottom") == 0)
    return LAYER_BOTTOM;
  if (strcmp(str, "background") == 0)
    return LAYER_BACKGROUND;
  return LAYER_TOP;
}

GtkLayerShellLayer get_layer_shell_layer(LayerType layer) {
  switch (layer) {
  case LAYER_OVERLAY:
    return GTK_LAYER_SHELL_LAYER_OVERLAY;
  case LAYER_BOTTOM:
    return GTK_LAYER_SHELL_LAYER_BOTTOM;
  case LAYER_BACKGROUND:
    return GTK_LAYER_SHELL_LAYER_BACKGROUND;
  default:
    return GTK_LAYER_SHELL_LAYER_TOP;
  }
}

gboolean parse_arguments(FrameSettings *settings, int *argc, char ***argv) {
  GOptionEntry entries[] = {
      {"radius", 'r', 0, G_OPTION_ARG_INT, &settings->radius, "Corner radius (default: 12)", "N"},
      {"gap", 'g', 0, G_OPTION_ARG_STRING, &settings->gap_str,
       "Gap from screen edges (1 or 4 values: top right bottom left)", "GAP"},
      {"border", 'b', 0, G_OPTION_ARG_INT, &settings->border_width, "Border width (default: 1)", "N"},
      {"alpha", 'a', 0, G_OPTION_ARG_STRING, &settings->alpha_str,
       "Alpha values (1 or 2 values: [light] [dark])", "ALPHA"},
      {"layer", 'l', 0, G_OPTION_ARG_STRING, &settings->layer_str,
       "Layer type: top, overlay, bottom, background (default: top)", "TYPE"},
      {"log", 0, 0, G_OPTION_ARG_NONE, &logging_enabled, "Enable debug logging", NULL},
      {NULL}
  };

  GOptionContext *context = g_option_context_new("- Rounded screen frame");
  g_option_context_add_main_entries(context, entries, NULL);
  g_option_context_add_group(context, gtk_get_option_group(TRUE));

  GError *error = NULL;
  if (!g_option_context_parse(context, argc, argv, &error)) {
    g_printerr("Error parsing arguments: %s\n", error->message);
    g_error_free(error);
    g_option_context_free(context);
    return FALSE;
  }

  if (settings->alpha_str) {
    double alphas[2] = {0.95, 0.85};
    int count = 0;

    char **parts = g_strsplit(settings->alpha_str, " ", 2);
    for (int i = 0; parts[i] && i < 2; i++) {
      alphas[i] = atof(parts[i]);
      count++;
    }
    g_strfreev(parts);

    if (count == 1) {
      settings->alpha_light = settings->alpha_dark = alphas[0];
    } else if (count == 2) {
      settings->alpha_light = alphas[0];
      settings->alpha_dark = alphas[1];
    } else {
      fprintf(stderr, "Invalid --alpha value. Must be 1 or 2 numbers.\n");
      return FALSE;
    }
  }

  if (settings->layer_str) {
    settings->layer = parse_layer_type(settings->layer_str);
  }

  if (settings->gap_str) {
    int gaps[4] = {0};
    int count = 0;

    char **parts = g_strsplit(settings->gap_str, " ", 4);
    for (int i = 0; parts[i] && i < 4; i++) {
      gaps[i] = atoi(parts[i]);
      count++;
    }
    g_strfreev(parts);

    if (count == 1) {
      settings->gap_top = settings->gap_right = settings->gap_bottom = settings->gap_left = gaps[0];
    } else if (count == 4) {
      settings->gap_top = gaps[0];
      settings->gap_right = gaps[1];
      settings->gap_bottom = gaps[2];
      settings->gap_left = gaps[3];
    } else {
      fprintf(stderr, "Invalid --gap value. Must be 1 or 4 integers.\n");
      return FALSE;
    }
  }

  g_option_context_free(context);
  return TRUE;
}
