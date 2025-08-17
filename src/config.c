#include <glib.h>
#include <glib/gstdio.h>
#include "config.h"
#include "logging.h"

void free_config(gpointer data) {
  Config *config = (Config *)data;
  if (config) {
    g_free((gpointer)config->layer_type);
    g_free((gpointer)config->layer_name);
    g_free((gpointer)config->keyboard_mode);
    g_free(config);
    LOG("Config struct freed\n");
  }
}

gboolean load_config(Config *config, const char *config_path) {
  GKeyFile *key_file = g_key_file_new();
  GError *error = NULL;
  if (!g_key_file_load_from_file(key_file, config_path, G_KEY_FILE_NONE, &error)) {
    ERROR_LOG("Failed to load config from %s: %s\n", config_path, error->message);
    g_error_free(error);
    g_key_file_set_string(key_file, "Layer", "type", "BOTTOM");
    g_key_file_set_string(key_file, "Layer", "name", "roundframe");
    g_key_file_set_integer(key_file, "Layout", "gap_left", 5);
    g_key_file_set_integer(key_file, "Layout", "gap_right", 5);
    g_key_file_set_integer(key_file, "Layout", "gap_top", 5);
    g_key_file_set_integer(key_file, "Layout", "gap_bottom", 5);
    g_key_file_set_double(key_file, "Layout", "inner_radius", 12.0);
    g_key_file_set_boolean(key_file, "Layout", "exclusive", FALSE);
  }

  config->layer_type = g_key_file_get_string(key_file, "Layer", "type", NULL);
  config->layer_name = g_key_file_get_string(key_file, "Layer", "name", NULL);
  config->keyboard_mode = g_key_file_get_string(key_file, "Layer", "keyboard_mode", NULL);
  config->gap_left = g_key_file_get_integer(key_file, "Layout", "gap_left", NULL);
  config->gap_right = g_key_file_get_integer(key_file, "Layout", "gap_right", NULL);
  config->gap_top = g_key_file_get_integer(key_file, "Layout", "gap_top", NULL);
  config->gap_bottom = g_key_file_get_integer(key_file, "Layout", "gap_bottom", NULL);
  config->inner_radius = g_key_file_get_double(key_file, "Layout", "inner_radius", NULL);
  config->exclusive = g_key_file_get_boolean(key_file, "Layout", "exclusive", &error);
  if (error) {
    ERROR_LOG("Error reading exclusive from config: %s\n", error->message);
    g_error_free(error);
    config->exclusive = FALSE;
  }
  if (config->gap_left < 0) config->gap_left = 5;
  if (config->gap_right < 0) config->gap_right = 5;
  if (config->gap_top < 0) config->gap_top = 5;
  if (config->gap_bottom < 0) config->gap_bottom = 5;
  if (config->inner_radius < 0) config->inner_radius = 12.0;
  g_key_file_free(key_file);
  return TRUE;
}
