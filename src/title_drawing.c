#include "title_drawing.h"

#include <pango/pangocairo.h>

#include "screenshot.h"

void draw_window_title(cairo_t *cr,
                       const char *title,
                       double img_width,
                       int title_size) {
    if (!title)
        return;

    char font_string[32];
    snprintf(font_string, sizeof(font_string), "Sans Bold %d", title_size);

    PangoLayout *title_layout = pango_cairo_create_layout(cr);
    PangoFontDescription *title_font_desc =
        pango_font_description_from_string(font_string);
    pango_layout_set_font_description(title_layout, title_font_desc);
    pango_layout_set_text(title_layout, title, -1);

    int title_width, title_height;
    pango_layout_get_pixel_size(title_layout, &title_width, &title_height);

    double title_x = (img_width - title_width) / 2.0;
    double title_y = (PADDING / 2 + (HEADER_HEIGHT - title_height) / 2.0);

    cairo_move_to(cr, title_x, title_y);
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9); // Title color
    pango_cairo_show_layout(cr, title_layout);

    pango_font_description_free(title_font_desc);
    g_object_unref(title_layout);
}
