#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "screenshot.h"

// Defines the visual style of the screenshot window.
const double PADDING = 50.0;
const double HEADER_HEIGHT = 40.0;
const double SHADOW_OFFSET = 10.0;
const double SHADOW_BLUR = 15.0;
const double BORDER_RADIUS = 8.0;
const char* FONT = "Monospace 12";

/**
 * @brief Helper function to draw a rectangle with rounded corners for the window frame.
 * @param cr The cairo drawing context.
 * @param x Top-left x coordinate.
 * @param y Top-left y coordinate.
 * @param width The width of the rectangle.
 * @param height The height of the rectangle.
 * @param radius The corner radius.
 */
void draw_rounded_rectangle(cairo_t *cr, double x, double y, double width, double height, double radius) {
    cairo_new_sub_path(cr);
    cairo_arc(cr, x + width - radius, y + radius, radius, -M_PI / 2, 0);
    cairo_arc(cr, x + width - radius, y + height - radius, radius, 0, M_PI / 2);
    cairo_arc(cr, x + radius, y + height - radius, radius, M_PI / 2, M_PI);
    cairo_arc(cr, x + radius, y + radius, radius, M_PI, 3 * M_PI / 2);
    cairo_close_path(cr);
}