#include <math.h>
#include "screenshot.h"

// Constants for styling
const double PADDING = 50.0;
const double HEADER_HEIGHT = 40.0;
const double SHADOW_OFFSET = 10.0;
const double SHADOW_BLUR = 15.0; // Kept for reference, but not implemented with simple cairo
const double BORDER_RADIUS = 8.0;
const char* FONT = "Monospace 12";

// Helper to draw rounded rectangles
void draw_rounded_rectangle(cairo_t *cr, double x, double y, double width, double height, double radius) {
    cairo_new_sub_path(cr);
    cairo_arc(cr, x + width - radius, y + radius, radius, -M_PI / 2, 0);
    cairo_arc(cr, x + width - radius, y + height - radius, radius, 0, M_PI / 2);
    cairo_arc(cr, x + radius, y + height - radius, radius, M_PI / 2, M_PI);
    cairo_arc(cr, x + radius, y + radius, radius, M_PI, 3 * M_PI / 2);
    cairo_close_path(cr);
}
