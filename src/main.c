#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "screenshot.h"
#include "syntax_highlighting.h"

// Helper function to detect the programming language from the filename extension.
static LanguageType get_language_from_filename(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return LANG_UNKNOWN;
    if (strcmp(dot, ".c") == 0) return LANG_C;
    if (strcmp(dot, ".py") == 0) return LANG_PYTHON;
    return LANG_UNKNOWN;
}

// Helper function to draw the window header with properly rounded bottom corners.
void draw_header(cairo_t *cr, double x, double y, double width, double height, double radius, gboolean use_gradient) {
    cairo_new_sub_path(cr);
    cairo_arc(cr, x + width - radius, y + height - radius, radius, 0, M_PI / 2);
    cairo_arc(cr, x + radius, y + height - radius, radius, M_PI / 2, M_PI);
    cairo_line_to(cr, x, y);
    cairo_line_to(cr, x + width, y);
    cairo_close_path(cr);

    if (use_gradient) {
        cairo_pattern_t *header_pat = cairo_pattern_create_linear(0, y, 0, y + height);
        cairo_pattern_add_color_stop_rgb(header_pat, 0, 0.18, 0.19, 0.25);
        cairo_pattern_add_color_stop_rgb(header_pat, 1, 0.141, 0.157, 0.231);
        cairo_set_source(cr, header_pat);
        cairo_pattern_destroy(header_pat);
    } else {
        cairo_set_source_rgb(cr, 0.141, 0.157, 0.231); // Solid color
    }
    cairo_fill(cr);
}

int main(int argc, char *argv[]) {
    LanguageType lang = LANG_UNKNOWN;
    gboolean use_gradient_header = TRUE;
    gboolean lang_option_used = FALSE;
    gboolean show_line_numbers = FALSE; // New flag for line numbers

    const char *input_filename = NULL;
    const char *output_filename = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-lang") == 0) {
            lang_option_used = TRUE;
            if (i + 1 < argc) {
                if (strcmp(argv[i+1], "c") == 0) lang = LANG_C;
                else if (strcmp(argv[i+1], "python") == 0) lang = LANG_PYTHON;
                i++;
            } else {
                fprintf(stderr, "-lang option requires an argument (c or python).\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-no-gradient") == 0) {
            use_gradient_header = FALSE;
        } else if (strcmp(argv[i], "-l") == 0) { // New flag parsing
            show_line_numbers = TRUE;
        } else if (input_filename == NULL) {
            input_filename = argv[i];
        } else if (output_filename == NULL) {
            output_filename = argv[i];
        } else {
            fprintf(stderr, "Too many arguments provided.\n");
            return 1;
        }
    }

    if (input_filename == NULL || output_filename == NULL) {
        fprintf(stderr, "Usage: %s [OPTIONS] <input_file> <output_png>\n\n", "screenCODE");
        fprintf(stderr, "OPTIONS:\n");
        fprintf(stderr, "  -lang c|python    Specify language (default: auto-detect from extension).\n");
        fprintf(stderr, "  -no-gradient      Disable the gradient effect on the window header.\n");
        fprintf(stderr, "  -l                Show line numbers.\n"); // New usage info
        return 1;
    }

    // Auto-detect language from file extension if not specified by the user.
    if (!lang_option_used) {
        lang = get_language_from_filename(input_filename);
    }

    // Validate that the language is supported.
    if (lang == LANG_UNKNOWN) {
        fprintf(stderr, "Error: Unsupported file type or language not specified.\n");
        fprintf(stderr, "This program currently only supports .c and .py files.\n");
        fprintf(stderr, "Please use a supported file or specify the language with -lang c|python.\n");
        return 1;
    }

    // Initialize syntax tables once.
    if (lang == LANG_C) init_syntax_tables_c();
    else if (lang == LANG_PYTHON) init_syntax_tables_python();

    GError *error = NULL;
    char *code_content;
    if (!g_file_get_contents(input_filename, &code_content, NULL, &error)) {
        fprintf(stderr, "Error reading file: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    cairo_surface_t *temp_surface = cairo_image_surface_create(CAIRO_FORMAT_A8, 0, 0);
    cairo_t *temp_cr = cairo_create(temp_surface);
    PangoLayout *layout = pango_cairo_create_layout(temp_cr);
    PangoFontDescription *font_desc = pango_font_description_from_string(FONT);
    pango_layout_set_font_description(layout, font_desc);

    // Pass show_line_numbers to highlight_syntax
    char* highlighted_text = highlight_syntax(code_content, lang, show_line_numbers);
    if (!highlighted_text) { // Check for memory allocation failure from highlight_syntax
        fprintf(stderr, "Error: Failed to highlight syntax due to memory allocation failure.\n");
        g_free(code_content);
        g_object_unref(layout);
        pango_font_description_free(font_desc);
        cairo_destroy(temp_cr);
        cairo_surface_destroy(temp_surface);
        return 1;
    }
    pango_layout_set_markup(layout, highlighted_text, -1);

    int text_width_pixels, text_height_pixels;
    pango_layout_get_pixel_size(layout, &text_width_pixels, &text_height_pixels);

    // Calculate image dimensions based on wrapped text size
    int img_width = text_width_pixels + (2 * PADDING);
    int img_height = HEADER_HEIGHT + text_height_pixels + (3 * PADDING);

    g_object_unref(layout);
    pango_font_description_free(font_desc);
    cairo_destroy(temp_cr);
    cairo_surface_destroy(temp_surface);

    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, img_width, img_height);
    cairo_t *cr = cairo_create(surface);

    cairo_pattern_t *pat = cairo_pattern_create_linear(0, 0, img_width, img_height);
    cairo_pattern_add_color_stop_rgba(pat, 0, 0.102, 0.106, 0.149, 1);
    cairo_pattern_add_color_stop_rgba(pat, 1, 0.141, 0.157, 0.231, 1);
    cairo_rectangle(cr, 0, 0, img_width, img_height);
    cairo_set_source(cr, pat);
    cairo_fill(cr);
    cairo_pattern_destroy(pat);

    cairo_set_source_rgba(cr, 0, 0, 0, 0.4);
    draw_rounded_rectangle(cr, PADDING / 2 + SHADOW_OFFSET, PADDING / 2 + SHADOW_OFFSET,
                           img_width - PADDING, img_height - PADDING, BORDER_RADIUS);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, 0.141, 0.157, 0.231);
    draw_rounded_rectangle(cr, PADDING / 2, PADDING / 2, img_width - PADDING, img_height - PADDING, BORDER_RADIUS);
    cairo_fill(cr);

    // Draw the header using the new, improved function.
    draw_header(cr, PADDING / 2, PADDING / 2, img_width - PADDING, HEADER_HEIGHT, BORDER_RADIUS, use_gradient_header);

    cairo_set_source_rgb(cr, 0.9686, 0.4627, 0.5569); // Red
    cairo_arc(cr, PADDING / 2 + 20, PADDING / 2 + HEADER_HEIGHT / 2, 7, 0, 2 * M_PI);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, 0.8784, 0.6863, 0.4078); // Yellow
    cairo_arc(cr, PADDING / 2 + 45, PADDING / 2 + HEADER_HEIGHT / 2, 7, 0, 2 * M_PI);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, 0.6196, 0.8078, 0.4157); // Green
    cairo_arc(cr, PADDING / 2 + 70, PADDING / 2 + HEADER_HEIGHT / 2, 7, 0, 2 * M_PI);
    cairo_fill(cr);

    layout = pango_cairo_create_layout(cr);
    font_desc = pango_font_description_from_string(FONT);
    pango_layout_set_font_description(layout, font_desc);
    pango_layout_set_markup(layout, highlighted_text, -1);

    cairo_set_source_rgb(cr, 0.6627, 0.6941, 0.8392); // Text color
    cairo_move_to(cr, PADDING, PADDING / 2 + HEADER_HEIGHT + (PADDING / 2));
    pango_cairo_show_layout(cr, layout);

    cairo_status_t status = cairo_surface_write_to_png(surface, output_filename);
    if (status != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "Could not save PNG file: %s\n", cairo_status_to_string(status));
    }

    g_free(code_content);
    g_free(highlighted_text);
    g_object_unref(layout);
    pango_font_description_free(font_desc);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    // Free syntax tables once at the end.
    if (lang == LANG_C) free_syntax_tables_c();
    else if (lang == LANG_PYTHON) free_syntax_tables_python();

    printf("Screenshot saved to %s\n", output_filename);

    return 0;
}