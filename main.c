#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screenshot.h"

int main(int argc, char *argv[]) {
    LanguageType lang = LANG_C; // Default language is C
    gboolean use_gradient_header = TRUE; // Default to using gradient header

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-lang") == 0) {
            if (i + 1 < argc) {
                if (strcmp(argv[i+1], "c") == 0) {
                    lang = LANG_C;
                } else if (strcmp(argv[i+1], "python") == 0) {
                    lang = LANG_PYTHON;
                } else {
                    fprintf(stderr, "Unknown language: %s. Using C as default.\n", argv[i+1]);
                }
                i++; // Skip the language argument
            } else {
                fprintf(stderr, "-lang option requires an argument (c or python).\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-no-gradient") == 0) {
            use_gradient_header = FALSE;
        }
    }

    if (argc < 3) {
        fprintf(stderr, "Usage: %s [OPTIONS] <input_file> <output_png>\n\n", "screenCODE");
        fprintf(stderr, "OPTIONS:\n");
        fprintf(stderr, "  -lang c|python    Specify the programming language for syntax highlighting (default: c).\n");
        fprintf(stderr, "  -no-gradient      Disable the gradient effect on the window header.\n\n");
        fprintf(stderr, "Arguments:\n");
        fprintf(stderr, "  <input_file>      Path to the source code file to be screenshotted.\n");
        fprintf(stderr, "  <output_png>      Path where the output PNG image will be saved.\n");
        return 1;
    }

    const char *input_filename = argv[argc - 2]; // Last argument is input file
    const char *output_filename = argv[argc - 1]; // Second to last is output file

    GError *error = NULL;
    char *code_content;
    if (!g_file_get_contents(input_filename, &code_content, NULL, &error)) {
        fprintf(stderr, "Error reading file: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    // Create a temporary surface to calculate text size
    cairo_surface_t *temp_surface = cairo_image_surface_create(CAIRO_FORMAT_A8, 0, 0);
    cairo_t *temp_cr = cairo_create(temp_surface);
    PangoLayout *layout = pango_cairo_create_layout(temp_cr);
    PangoFontDescription *font_desc = pango_font_description_from_string(FONT);
    pango_layout_set_font_description(layout, font_desc);

    char* highlighted_text = highlight_syntax(code_content, lang);
    pango_layout_set_markup(layout, highlighted_text, -1);

    int text_width, text_height;
    pango_layout_get_pixel_size(layout, &text_width, &text_height);

    // Calculate image dimensions
    double code_area_width = text_width + PADDING;
    double code_area_height = text_height + PADDING;
    int img_width = code_area_width + PADDING;
    int img_height = code_area_height + HEADER_HEIGHT + PADDING;

    // Cleanup temporary resources
    g_object_unref(layout);
    pango_font_description_free(font_desc);
    cairo_destroy(temp_cr);
    cairo_surface_destroy(temp_surface);

    // Create the final surface
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, img_width, img_height);
    cairo_t *cr = cairo_create(surface);

    // Background Gradient
    cairo_pattern_t *pat = cairo_pattern_create_linear(0, 0, img_width, img_height);
    cairo_pattern_add_color_stop_rgba(pat, 0, 0.102, 0.106, 0.149, 1); // #1a1b26
    cairo_pattern_add_color_stop_rgba(pat, 1, 0.141, 0.157, 0.231, 1); // #24283b
    cairo_rectangle(cr, 0, 0, img_width, img_height);
    cairo_set_source(cr, pat);
    cairo_fill(cr);
    cairo_pattern_destroy(pat);

    // Window Shadow (simple version)
    cairo_set_source_rgba(cr, 0, 0, 0, 0.4);
    draw_rounded_rectangle(cr, PADDING / 2 + SHADOW_OFFSET, PADDING / 2 + SHADOW_OFFSET,
                           img_width - PADDING, img_height - PADDING, BORDER_RADIUS);
    cairo_fill(cr);

    // Window Frame
    cairo_set_source_rgb(cr, 0.141, 0.157, 0.231); // #24283b
    draw_rounded_rectangle(cr, PADDING / 2, PADDING / 2, img_width - PADDING, img_height - PADDING, BORDER_RADIUS);
    cairo_fill(cr);

    // Window Header
    if (use_gradient_header) {
        cairo_pattern_t *header_pat = cairo_pattern_create_linear(0, PADDING/2, 0, PADDING/2 + HEADER_HEIGHT);
        cairo_pattern_add_color_stop_rgb(header_pat, 0, 0.18, 0.19, 0.25);
        cairo_pattern_add_color_stop_rgb(header_pat, 1, 0.141, 0.157, 0.231);
        draw_rounded_rectangle(cr, PADDING / 2, PADDING / 2, img_width - PADDING, HEADER_HEIGHT, BORDER_RADIUS);
        cairo_set_source(cr, header_pat);
        cairo_fill(cr);
        cairo_pattern_destroy(header_pat);
        // Redraw top edge to remove rounding
        cairo_rectangle(cr, PADDING / 2 + BORDER_RADIUS, PADDING / 2 + HEADER_HEIGHT - BORDER_RADIUS, img_width - PADDING - 2*BORDER_RADIUS, BORDER_RADIUS);
        cairo_fill(cr);
    } else {
        cairo_set_source_rgb(cr, 0.141, 0.157, 0.231); // Solid color #24283b
        draw_rounded_rectangle(cr, PADDING / 2, PADDING / 2, img_width - PADDING, HEADER_HEIGHT, BORDER_RADIUS);
        cairo_fill(cr);
        // Redraw top edge to remove rounding
        cairo_rectangle(cr, PADDING / 2 + BORDER_RADIUS, PADDING / 2 + HEADER_HEIGHT - BORDER_RADIUS, img_width - PADDING - 2*BORDER_RADIUS, BORDER_RADIUS);
        cairo_fill(cr);
    }


    // Window Buttons
    cairo_set_source_rgb(cr, 0.9686, 0.4627, 0.5569); // Red
    cairo_arc(cr, PADDING / 2 + 20, PADDING / 2 + HEADER_HEIGHT / 2, 7, 0, 2 * M_PI);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, 0.8784, 0.6863, 0.4078); // Yellow
    cairo_arc(cr, PADDING / 2 + 45, PADDING / 2 + HEADER_HEIGHT / 2, 7, 0, 2 * M_PI);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, 0.6196, 0.8078, 0.4157); // Green
    cairo_arc(cr, PADDING / 2 + 70, PADDING / 2 + HEADER_HEIGHT / 2, 7, 0, 2 * M_PI);
    cairo_fill(cr);

    // Draw the code
    layout = pango_cairo_create_layout(cr);
    font_desc = pango_font_description_from_string(FONT);
    pango_layout_set_font_description(layout, font_desc);
    pango_layout_set_markup(layout, highlighted_text, -1);

    cairo_set_source_rgb(cr, 0.6627, 0.6941, 0.8392); // Foreground
    cairo_move_to(cr, PADDING, PADDING / 2 + HEADER_HEIGHT + (PADDING - PADDING/2));
    pango_cairo_show_layout(cr, layout);

    // Save to PNG
    cairo_status_t status = cairo_surface_write_to_png(surface, output_filename);
    if (status != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "Could not save PNG file: %s\n", cairo_status_to_string(status));
    }

    // Cleanup
    g_free(code_content);
    g_free(highlighted_text);
    g_object_unref(layout);
    pango_font_description_free(font_desc);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    // Free syntax tables based on the selected language
    if (lang == LANG_C) {
        free_syntax_tables_c();
    } else if (lang == LANG_PYTHON) {
        free_syntax_tables_python();
    }

    printf("Screenshot saved to %s\n", output_filename);

    return 0;
}
