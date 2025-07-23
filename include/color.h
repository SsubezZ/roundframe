#ifndef COLOR_H
#define COLOR_H

#include "frame_settings.h"

typedef struct {
  double r, g, b, a;
} Color;

void get_gtk_colors(FrameSettings *settings, Color *bg, Color *border,
                    double *effective_alpha);

#endif // COLOR_H
