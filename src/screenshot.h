#ifndef SCREENSHOT_H
#define SCREENSHOT_H
#include <math.h>
#include <cairo.h>
#include <pango/pangocairo.h>
#include <glib.h>

// Constants for styling
extern const double PADDING;
extern const double HEADER_HEIGHT;
extern const double SHADOW_OFFSET;
extern const double SHADOW_BLUR;
extern const double BORDER_RADIUS;
extern const char* FONT;
extern const double DEFAULT_CODE_WIDTH;

// Enum for language type


// Global hash tables for faster lookups

void draw_rounded_rectangle(cairo_t *cr, double x, double y, double width, double height, double radius);

#endif // SCREENSHOT_H