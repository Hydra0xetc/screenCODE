#include "screenshot.h"
#include "syntax_highlighting.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <glib.h> // For GHashTable and GString

// Global hash tables for faster lookups - DEFINED HERE
GHashTable *keywords_ht = NULL;
GHashTable *preprocessor_directives_ht = NULL;
GHashTable *standard_functions_ht = NULL;

char* highlight_syntax(const char* code, LanguageType lang) {
    // Free existing tables before initializing new ones
    if (keywords_ht) {
        g_hash_table_unref(keywords_ht);
        keywords_ht = NULL;
    }
    if (preprocessor_directives_ht) {
        g_hash_table_unref(preprocessor_directives_ht);
        preprocessor_directives_ht = NULL;
    }
    if (standard_functions_ht) {
        g_hash_table_unref(standard_functions_ht);
        standard_functions_ht = NULL;
    }

    if (lang == LANG_C) {
        init_syntax_tables_c();
        char* highlighted_code = highlight_c_syntax(code);
        free_syntax_tables_c(); // Free C tables after use
        return highlighted_code;
    } else if (lang == LANG_PYTHON) {
        init_syntax_tables_python();
        char* highlighted_code = highlight_python_syntax(code);
        free_syntax_tables_python(); // Free Python tables after use
        return highlighted_code;
    } else {
        // Default to C if language is unknown
        init_syntax_tables_c();
        char* highlighted_code = highlight_c_syntax(code);
        free_syntax_tables_c(); // Free C tables after use
        return highlighted_code;
    }
}
