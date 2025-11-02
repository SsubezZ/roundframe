#include "roundframe.h"

void load_css(void) {
  if (global_css) {
    gtk_style_context_remove_provider_for_screen(
        gdk_screen_get_default(), GTK_STYLE_PROVIDER(global_css));
    g_object_unref(global_css);
    global_css = NULL;
  }
  global_css = gtk_css_provider_new();
  GError *err = NULL;
  gchar *path = find_config_file("style.css");
  if (!path || !gtk_css_provider_load_from_path(global_css, path, &err)) {
    if (err) {
      g_warning("Failed to load style.css from %s: %s", path ? path : "(null)",
                err->message);
      g_clear_error(&err);
    }
    const char *built_in_css =
        ".top { background: alpha(@theme_bg_color, 0.75); }\n"
        ".bottom { background: alpha(@theme_bg_color, 0.75); }\n"
        ".left { background: alpha(@theme_bg_color, 0.75); }\n"
        ".right { background: alpha(@theme_bg_color, 0.75); }\n"
        ".top-left { background: alpha(@theme_bg_color, 0.75); }\n"
        ".top-right { background: alpha(@theme_bg_color, 0.75); }\n"
        ".bottom-left { background: alpha(@theme_bg_color, 0.75); }\n"
        ".bottom-right { background: alpha(@theme_bg_color, 0.75); }\n"
        "@supports not (background: alpha(@theme_bg_color, 0.75)) {\n"
        " .top, .bottom, .left, .right, "
        ".top-left, .top-right, .bottom-left, .bottom-right { "
        "background: rgba(0, 0, 0, 0.75); }\n"
        "}";
    gtk_css_provider_load_from_data(global_css, built_in_css, -1, NULL);
    g_message("Using built-in CSS defaults");
  } else {
    g_message("Loaded CSS from: %s", path);
  }
  gtk_style_context_add_provider_for_screen(
      gdk_screen_get_default(), GTK_STYLE_PROVIDER(global_css),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_free(path);
  for (GList *iter = edge_windows; iter; iter = iter->next) {
    GtkWidget *win = GTK_WIDGET(iter->data);
    GtkWidget *box = gtk_bin_get_child(GTK_BIN(win));
    if (box) {
      GtkStyleContext *ctx = gtk_widget_get_style_context(box);
      const char *css_class = NULL;
      for (unsigned i = 0; i < num_edge_cfgs; ++i) {
        if (g_strcmp0(gtk_widget_get_name(win), edge_cfgs[i].name) == 0) {
          css_class = edge_cfgs[i].css;
          break;
        }
      }
      if (css_class) {
        gtk_style_context_remove_class(ctx, css_class);
        gtk_style_context_add_class(ctx, css_class);
        gtk_widget_queue_draw(box);
      }
    }
  }
  for (GList *iter = quad_windows; iter; iter = iter->next) {
    GtkWidget *win = GTK_WIDGET(iter->data);
    GtkWidget *drawing_area = gtk_bin_get_child(GTK_BIN(win));
    if (drawing_area) {
      GtkStyleContext *ctx = gtk_widget_get_style_context(drawing_area);
      const char *css_class = NULL;
      for (unsigned i = 0; i < num_quad_cfgs; ++i) {
        if (g_strcmp0(gtk_widget_get_name(win), quad_cfgs[i].name) == 0) {
          css_class = quad_cfgs[i].css;
          break;
        }
      }
      if (css_class) {
        gtk_style_context_remove_class(ctx, css_class);
        gtk_style_context_add_class(ctx, css_class);
        gtk_widget_queue_draw(drawing_area);
      }
    }
  }
}
