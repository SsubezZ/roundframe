#include <stdio.h>
#include <stdarg.h>
#include "logging.h"

static int logging_enabled = 0;

void log_debug(const char *format, ...) {
  if (!logging_enabled)
    return;
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  fflush(stdout);
}
