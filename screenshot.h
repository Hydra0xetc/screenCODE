#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <math.h>
#define _USE_MATH_DEFINES
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

// Enum for language type
typedef enum {
    LANG_C,
    LANG_PYTHON
} LanguageType;

// Global hash tables for faster lookups
extern GHashTable *keywords_ht;
extern GHashTable *preprocessor_directives_ht;
extern GHashTable *standard_functions_ht;

// Function declarations
void init_syntax_tables_c();
void free_syntax_tables_c();
void init_syntax_tables_python();
void free_syntax_tables_python();

char* highlight_syntax(const char* code, LanguageType lang);
void draw_rounded_rectangle(cairo_t *cr, double x, double y, double width, double height, double radius);

#endif // SCREENSHOT_H