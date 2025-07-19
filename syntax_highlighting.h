#ifndef SYNTAX_HIGHLIGHTING_H
#define SYNTAX_HIGHLIGHTING_H

#include <glib.h>

// Define LanguageType enum
typedef enum {
    LANG_C,
    LANG_PYTHON,
    LANG_UNKNOWN
} LanguageType;

// Global hash tables for faster lookups
extern GHashTable *keywords_ht;
extern GHashTable *preprocessor_directives_ht;
extern GHashTable *standard_functions_ht;

// Function declarations for C syntax highlighting
void init_syntax_tables_c();
void free_syntax_tables_c();
char* highlight_c_syntax(const char* code);

// Function declarations for Python syntax highlighting
void init_syntax_tables_python();
void free_syntax_tables_python();
char* highlight_python_syntax(const char* code);

// Main syntax highlighting function
char* highlight_syntax(const char* code, LanguageType lang);

#endif // SYNTAX_HIGHLIGHTING_H
