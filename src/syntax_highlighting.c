#include "syntax_highlighting.h"

#include <glib.h>  // Using GLib for efficient hash tables.

// Use global hash tables for syntax elements (keywords, etc.)
// This allows for quick lookups while parsing the code.
GHashTable *keywords_ht = NULL;
GHashTable *preprocessor_directives_ht = NULL;
GHashTable *standard_functions_ht = NULL;

/**
 * @brief Acts as a dispatcher, selecting the correct syntax highlighter based
 * on the language.
 * @param code The source code to highlight.
 * @param lang The programming language (LANG_C or LANG_PYTHON).
 * @param show_line_numbers Boolean flag to indicate if line numbers should be
 * shown.
 * @return A new string containing the code with Pango markup for highlighting.
 */
char *highlight_syntax(const char *code,
                       LanguageType lang,
                       gboolean show_line_numbers,
                       gboolean no_color) {
    if (no_color) {
        return g_markup_escape_text(code, -1);
    }
    // The initialization and freeing of syntax tables is now handled in main.c.
    // This function now only dispatches to the correct highlighter.

    if (lang == LANG_C) {
        return highlight_c_syntax(code, show_line_numbers);
    } else if (lang == LANG_PYTHON) {
        return highlight_python_syntax(code, show_line_numbers);
    } else if (lang == LANG_GO) {
        return highlight_go_syntax(code, show_line_numbers);
    } else {
        // Fallback for unknown languages: just escape the text without
        // highlighting.
        return g_markup_escape_text(code, -1);
    }
}
