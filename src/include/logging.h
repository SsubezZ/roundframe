#ifndef LOGGING_H
#define LOGGING_H

#include <glib.h>

extern gboolean logging_enabled;

#define LOG(...) if (logging_enabled) g_print(__VA_ARGS__)
#define ERROR_LOG(...) do { g_printerr(__VA_ARGS__); if (logging_enabled) g_print(__VA_ARGS__); } while (0)

#endif // LOGGING_H
