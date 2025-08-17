#ifndef LOCKFILE_H
#define LOCKFILE_H

#include <glib.h>

gboolean check_if_running(void);
void cleanup_lock_file(void);

#endif // LOCKFILE_H
