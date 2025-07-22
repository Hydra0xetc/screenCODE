#ifndef SYNTAX_HIGHLIGHTING_H
#define SYNTAX_HIGHLIGHTING_H

#include <glib.h>

// Defines the supported programming languages for syntax highlighting.
typedef enum {
    LANG_C,
    LANG_PYTHON,
    LANG_UNKNOWN
} LanguageType;

// Declare global hash tables for syntax elements.
// Using 'extern' makes them accessible from other files that include this header.
extern GHashTable *keywords_ht;
extern GHashTable *preprocessor_directives_ht;
extern GHashTable *standard_functions_ht;

// --- Function Prototypes ---

// C-specific syntax highlighting functions.
void init_syntax_tables_c();
void free_syntax_tables_c();
char* highlight_c_syntax(const char* code, gboolean show_line_numbers);

// Python-specific syntax highlighting functions.
void init_syntax_tables_python();
void free_syntax_tables_python();
char* highlight_python_syntax(const char* code, gboolean show_line_numbers);

// The main function that dispatches to the correct language highlighter.
char* highlight_syntax(const char* code, LanguageType lang, gboolean show_line_numbers);

#endif // SYNTAX_HIGHLIGHTING_H
