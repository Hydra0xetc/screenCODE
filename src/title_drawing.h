#ifndef TITLE_DRAWING_H
#define TITLE_DRAWING_H

#include <cairo.h>

void draw_window_title(cairo_t *cr,
                       const char *title,
                       double img_width,
                       int title_size);

#endif // TITLE_DRAWING_H
