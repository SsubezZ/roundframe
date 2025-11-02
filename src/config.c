#include "roundframe.h"

/* Default configurations */
static const EdgeWindowConfig default_edge_cfgs[] = {
    {GTK_LAYER_SHELL_EDGE_TOP, "roundframe-top", 0, TRUE, 0, 0, 0, 0, "top"},
    {GTK_LAYER_SHELL_EDGE_BOTTOM, "roundframe-bottom", 4, TRUE, 0, 0, 0, 0,
     "bottom"},
    {GTK_LAYER_SHELL_EDGE_LEFT, "roundframe-left", 4, TRUE, 0, 0, 0, 0, "left"},
    {GTK_LAYER_SHELL_EDGE_RIGHT, "roundframe-right", 4, TRUE, 0, 0, 0, 0,
     "right"},
};

static const QuadWindowConfig default_quad_cfgs[] = {
    {QUAD_TOP_LEFT, "roundframe-top-left", 12, TRUE, 0, 0, 0, 0, "top-left"},
    {QUAD_TOP_RIGHT, "roundframe-top-right", 12, TRUE, 0, 0, 0, 0, "top-right"},
    {QUAD_BOTTOM_LEFT, "roundframe-bottom-left", 12, TRUE, 0, 0, 0, 0,
     "bottom-left"},
    {QUAD_BOTTOM_RIGHT, "roundframe-bottom-right", 12, TRUE, 0, 0, 0, 0,
     "bottom-right"},
};

gchar *find_config_file(const char *filename) {
  const char *xdg = g_getenv("XDG_CONFIG_HOME");
  const char *home = g_get_home_dir();
  if (!home) {
    g_warning("Home directory not found, checking current directory for %s",
              filename);
  }
  if (xdg && *xdg) {
    gchar *path = g_build_filename(xdg, "roundframe", filename, NULL);
    if (g_file_test(path, G_FILE_TEST_EXISTS))
      return path;
    g_free(path);
  } else if (home) {
    gchar *path =
        g_build_filename(home, ".config", "roundframe", filename, NULL);
    if (g_file_test(path, G_FILE_TEST_EXISTS))
      return path;
    g_free(path);
  }
  if (g_file_test(filename, G_FILE_TEST_EXISTS))
    return g_strdup(filename);
  return NULL;
}

void free_configs(void) {
  if (edge_cfgs) {
    for (unsigned i = 0; i < num_edge_cfgs; ++i) {
      g_free(edge_cfgs[i].name);
    }
    g_free(edge_cfgs);
    edge_cfgs = NULL;
    num_edge_cfgs = 0;
  }
  if (quad_cfgs) {
    for (unsigned i = 0; i < num_quad_cfgs; ++i) {
      g_free(quad_cfgs[i].name);
    }
    g_free(quad_cfgs);
    quad_cfgs = NULL;
    num_quad_cfgs = 0;
  }
  if (enabled_monitors) {
    g_free(enabled_monitors);
    enabled_monitors = NULL;
    num_enabled_monitors = 0;
  }
}

void load_config(void) {
  GKeyFile *kf = g_key_file_new();
  GError *err = NULL;
  gchar *path = find_config_file("config.ini");
  if (!path || !g_key_file_load_from_file(kf, path, G_KEY_FILE_NONE, &err)) {
    g_warning("Failed to load config.ini: %s",
              err ? err->message : "File not found");
    g_clear_error(&err);
    num_edge_cfgs = G_N_ELEMENTS(default_edge_cfgs);
    edge_cfgs = g_new(EdgeWindowConfig, num_edge_cfgs);
    for (unsigned i = 0; i < num_edge_cfgs; ++i) {
      edge_cfgs[i] = default_edge_cfgs[i];
      edge_cfgs[i].name = g_strdup(default_edge_cfgs[i].name);
    }
    num_quad_cfgs = G_N_ELEMENTS(default_quad_cfgs);
    quad_cfgs = g_new(QuadWindowConfig, num_quad_cfgs);
    for (unsigned i = 0; i < num_quad_cfgs; ++i) {
      quad_cfgs[i] = default_quad_cfgs[i];
      quad_cfgs[i].name = g_strdup(default_quad_cfgs[i].name);
    }
    g_message("Loaded default configuration (no config.ini found)");
    g_key_file_free(kf);
    g_free(path);
    return;
  }
  g_message("Loaded configuration from: %s", path);
  enabled_monitors = g_key_file_get_integer_list(kf, "frame", "monitors",
                                                 &num_enabled_monitors, &err);
  if (err) {
    g_warning("Failed to read monitors list: %s, applying to all monitors",
              err->message);
    g_clear_error(&err);
    enabled_monitors = NULL;
    num_enabled_monitors = 0;
  }
  gboolean unique_names =
      g_key_file_get_boolean(kf, "frame", "unique_names", &err);
  if (err) {
    g_clear_error(&err);
    unique_names = FALSE;
  }
  gchar *base_name = NULL;
  err = NULL;
  base_name = g_key_file_get_string(kf, "frame", "name", &err);
  if (err || !base_name || !*base_name) {
    g_clear_error(&err);
    g_free(base_name);
    base_name = g_strdup("roundframe");
  }
  num_edge_cfgs = 4;
  edge_cfgs = g_new(EdgeWindowConfig, num_edge_cfgs);
  num_quad_cfgs = 4;
  quad_cfgs = g_new(QuadWindowConfig, num_quad_cfgs);
  struct {
    const char *section;
    GtkLayerShellEdge edge;
    const char *cssclass;
  } edges[] = {
      {"top", GTK_LAYER_SHELL_EDGE_TOP, "top"},
      {"bottom", GTK_LAYER_SHELL_EDGE_BOTTOM, "bottom"},
      {"left", GTK_LAYER_SHELL_EDGE_LEFT, "left"},
      {"right", GTK_LAYER_SHELL_EDGE_RIGHT, "right"},
  };
  struct {
    const char *section;
    QuadType quad;
    const char *cssclass;
  } quads[] = {
      {"top-left", QUAD_TOP_LEFT, "top-left"},
      {"top-right", QUAD_TOP_RIGHT, "top-right"},
      {"bottom-left", QUAD_BOTTOM_LEFT, "bottom-left"},
      {"bottom-right", QUAD_BOTTOM_RIGHT, "bottom-right"},
  };
  for (int i = 0; i < 4; i++) {
    const char *sec = edges[i].section;
    edge_cfgs[i].edge = edges[i].edge;
    gchar *name = unique_names ? g_strdup_printf("%s-%s", base_name, sec)
                               : g_strdup(base_name);
    edge_cfgs[i].name = name;
    edge_cfgs[i].thickness = g_key_file_get_integer(kf, sec, "thickness", &err);
    if (err) {
      g_warning("Failed to read thickness for %s: %s, using default %d", sec,
                err->message, default_edge_cfgs[i].thickness);
      g_clear_error(&err);
      edge_cfgs[i].thickness = default_edge_cfgs[i].thickness;
    }
    edge_cfgs[i].exclusive = g_key_file_get_boolean(kf, sec, "exclusive", &err);
    if (err) {
      g_clear_error(&err);
      edge_cfgs[i].exclusive = default_edge_cfgs[i].exclusive;
    }
    edge_cfgs[i].margin_top =
        g_key_file_get_integer(kf, sec, "margin_top", &err);
    if (err) {
      g_clear_error(&err);
      edge_cfgs[i].margin_top = default_edge_cfgs[i].margin_top;
    }
    edge_cfgs[i].margin_bottom =
        g_key_file_get_integer(kf, sec, "margin_bottom", &err);
    if (err) {
      g_clear_error(&err);
      edge_cfgs[i].margin_bottom = default_edge_cfgs[i].margin_bottom;
    }
    edge_cfgs[i].margin_left =
        g_key_file_get_integer(kf, sec, "margin_left", &err);
    if (err) {
      g_clear_error(&err);
      edge_cfgs[i].margin_left = default_edge_cfgs[i].margin_left;
    }
    edge_cfgs[i].margin_right =
        g_key_file_get_integer(kf, sec, "margin_right", &err);
    if (err) {
      g_clear_error(&err);
      edge_cfgs[i].margin_right = default_edge_cfgs[i].margin_right;
    }
    edge_cfgs[i].css = edges[i].cssclass;
  }
  for (int i = 0; i < 4; i++) {
    const char *sec = quads[i].section;
    quad_cfgs[i].quad = quads[i].quad;
    gchar *name = unique_names ? g_strdup_printf("%s-%s", base_name, sec)
                               : g_strdup(base_name);
    quad_cfgs[i].name = name;
    quad_cfgs[i].radius = g_key_file_get_integer(kf, sec, "radius", &err);
    if (err || quad_cfgs[i].radius < 0) {
      g_clear_error(&err);
      quad_cfgs[i].radius = default_quad_cfgs[i].radius;
    }
    quad_cfgs[i].exclusive = g_key_file_get_boolean(kf, sec, "exclusive", &err);
    if (err) {
      g_clear_error(&err);
      quad_cfgs[i].exclusive = default_quad_cfgs[i].exclusive;
    }
    quad_cfgs[i].margin_top =
        g_key_file_get_integer(kf, sec, "margin_top", &err);
    if (err) {
      g_clear_error(&err);
      quad_cfgs[i].margin_top = default_quad_cfgs[i].margin_top;
    }
    quad_cfgs[i].margin_bottom =
        g_key_file_get_integer(kf, sec, "margin_bottom", &err);
    if (err) {
      g_clear_error(&err);
      quad_cfgs[i].margin_bottom = default_quad_cfgs[i].margin_bottom;
    }
    quad_cfgs[i].margin_left =
        g_key_file_get_integer(kf, sec, "margin_left", &err);
    if (err) {
      g_clear_error(&err);
      quad_cfgs[i].margin_left = default_quad_cfgs[i].margin_left;
    }
    quad_cfgs[i].margin_right =
        g_key_file_get_integer(kf, sec, "margin_right", &err);
    if (err) {
      g_clear_error(&err);
      quad_cfgs[i].margin_right = default_quad_cfgs[i].margin_right;
    }
    quad_cfgs[i].css = quads[i].cssclass;
  }
  g_key_file_free(kf);
  g_free(base_name);
  g_free(path);
}
