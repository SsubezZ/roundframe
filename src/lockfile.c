#include <glib/gstdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "lockfile.h"
#include "logging.h"

gboolean check_if_running(void) {
  gchar *lock_path = g_build_filename(g_get_user_config_dir(), "roundframe", "roundframe.lock", NULL);
  if (g_file_test(lock_path, G_FILE_TEST_EXISTS)) {
    gchar *contents = NULL;
    GError *error = NULL;
    if (g_file_get_contents(lock_path, &contents, NULL, &error)) {
      pid_t pid = atoi(contents);
      g_free(contents);
      if (pid > 0 && kill(pid, 0) == 0) {
        ERROR_LOG("Error: roundframe is already running (PID: %d)\n", pid);
        g_free(lock_path);
        return TRUE;
      }
    } else {
      ERROR_LOG("Error reading lock file: %s\n", error->message);
      g_error_free(error);
    }
  }
  FILE *lock_file = g_fopen(lock_path, "w");
  if (lock_file) {
    fprintf(lock_file, "%d", getpid());
    fclose(lock_file);
    LOG("Lock file created at %s\n", lock_path);
  } else {
    ERROR_LOG("Error: Could not create lock file at %s\n", lock_path);
  }
  g_free(lock_path);
  return FALSE;
}

void cleanup_lock_file(void) {
  gchar *lock_path = g_build_filename(g_get_user_config_dir(), "roundframe", "roundframe.lock", NULL);
  if (g_file_test(lock_path, G_FILE_TEST_EXISTS)) {
    g_unlink(lock_path);
    LOG("Lock file removed\n");
  }
  g_free(lock_path);
}
