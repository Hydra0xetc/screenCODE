#include "screenshot.h"
#include "syntax_highlighting.h" // New header for declarations
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <glib.h> // For GHashTable and GString

// Global hash tables for faster lookups
extern GHashTable *keywords_ht;
extern GHashTable *preprocessor_directives_ht;
extern GHashTable *standard_functions_ht;

// Helper function to append highlighted text
static void append_and_highlight(
    GString* highlighted_code,
    const char* start_of_plain_text,
    const char* current_token_start,
    const char* color,
    size_t len
) {
    if (start_of_plain_text != current_token_start) {
        char *plain_text = g_strndup(start_of_plain_text, current_token_start - start_of_plain_text);
        char *escaped_plain_text = g_markup_escape_text(plain_text, -1);
        g_string_append(highlighted_code, escaped_plain_text);
        g_free(escaped_plain_text);
        g_free(plain_text);
    }
    char *token_text = g_strndup(current_token_start, len);
    char *escaped_token = g_markup_escape_text(token_text, -1);
    g_string_append_printf(highlighted_code, "<span foreground='%s'>%s</span>", color, escaped_token);
    g_free(escaped_token);
    g_free(token_text);
}

// Python Language Syntax Highlighting
void init_syntax_tables_python() {
    keywords_ht = g_hash_table_new(g_str_hash, g_str_equal);
    const char* keywords[] = {
        "False", "None", "True", "and", "as", "assert", "async", "await", "break",
        "class", "continue", "def", "del", "elif", "else", "except", "finally",
        "for", "from", "global", "if", "import", "in", "is", "lambda", "nonlocal",
        "not", "or", "pass", "raise", "return", "try", "while", "with", "yield",
        NULL
    };
    for (int i = 0; keywords[i] != NULL; i++) {
        g_hash_table_insert(keywords_ht, (gpointer)keywords[i], GINT_TO_POINTER(1));
    }

    // Python doesn't have preprocessor directives like C
    // preprocessor_directives_ht is not used for Python, so no need to initialize it here.

    standard_functions_ht = g_hash_table_new(g_str_hash, g_str_equal);
    const char* standard_functions[] = {
        "print", "input", "len", "range", "sum", "max", "min", "abs", "round",
        "open", "close", "read", "write", "append", "strip", "split", "join",
        "int", "float", "str", "list", "tuple", "dict", "set", "bool", "type",
        "id", "dir", "help", "isinstance", "issubclass", "super", "hasattr", "getattr",
        "setattr", "delattr", "callable", "frozenset", "complex", "divmod", "enumerate",
        "filter", "map", "next", "iter", "pow", "reversed", "slice", "sorted",
        "zip", "__import__",
        NULL
    };
    for (int i = 0; standard_functions[i] != NULL; i++) {
        g_hash_table_insert(standard_functions_ht, (gpointer)standard_functions[i], GINT_TO_POINTER(1));
    }
}

void free_syntax_tables_python() {
    if (keywords_ht) {
        g_hash_table_unref(keywords_ht);
        keywords_ht = NULL;
    }
    if (standard_functions_ht) {
        g_hash_table_unref(standard_functions_ht);
        standard_functions_ht = NULL;
    }
    // preprocessor_directives_ht is not used for Python, so no need to free it here.
}

// Sorted operators for longest match first (Python)
static const char* python_sorted_operators[] = {
    "**=", "//=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", ">>=", "<<=", // 3 chars
    "==", "!=", ">=", "<=", "**", "//", "or", "and", "not", "is", "in", // 2 chars
    "+", "-", "*", "/", "%", "=", ">", "<", "&", "|", "^", "~", ".", ":", "[", "]", "{", "}", "(", ")", // 1 char
    NULL
};

char* highlight_python_syntax(const char* code) {
    GString* highlighted_code = g_string_new("");
    const char *ptr = code;
    const char *start_of_plain_text = code;

    while (*ptr != '\0') {
        const char *current_token_start = ptr;
        gboolean token_processed = FALSE; // Flag to indicate if a token was processed

        // 1. Try to match triple-quoted strings first (Python specific)
        if ((*ptr == '"' && *(ptr + 1) == '"' && *(ptr + 2) == '"') ||
            (*ptr == '\'' && *(ptr + 1) == '\'' && *(ptr + 2) == '\'')) {
            char quote_char = *ptr;
            const char *scan_ptr = ptr + 3;
            while (*scan_ptr != '\0') {
                if (*scan_ptr == '\\' && *(scan_ptr + 1) != '\0') {
                    scan_ptr += 2; // Skip escaped char
                } else if (*scan_ptr == quote_char && *(scan_ptr + 1) == quote_char && *(scan_ptr + 2) == quote_char) {
                    scan_ptr += 3; // Include closing triple quotes
                    break;
                }
                scan_ptr++;
            }
            append_and_highlight(highlighted_code, start_of_plain_text, current_token_start, "#9ece6a", scan_ptr - ptr);
            ptr = scan_ptr; // Advance ptr
            start_of_plain_text = ptr;
            token_processed = TRUE;
        } else if (*ptr == '"' || *ptr == '\'') { // 2. Match single/double quoted strings
            char quote_char = *ptr;
            const char *scan_ptr = ptr + 1;
            while (*scan_ptr != '\0' && *scan_ptr != '\n') { // Single line strings should stop at newline
                if (*scan_ptr == '\\' && *(scan_ptr + 1) != '\0') {
                    scan_ptr += 2; // Skip backslash and escaped char
                } else if (*scan_ptr == quote_char) {
                    scan_ptr++; // Include closing quote
                    break;
                }
                scan_ptr++;
            }
            append_and_highlight(highlighted_code, start_of_plain_text, current_token_start, "#9ece6a", scan_ptr - ptr);
            ptr = scan_ptr; // Advance ptr
            start_of_plain_text = ptr;
            token_processed = TRUE;
        } else if (*ptr == '#') { // 3. Match comments
            const char *scan_ptr = ptr + 1;
            while (*scan_ptr != '\0' && *scan_ptr != '\n') {
                scan_ptr++;
            }
            append_and_highlight(highlighted_code, start_of_plain_text, current_token_start, "#545c7e", scan_ptr - ptr);
            ptr = scan_ptr; // Advance ptr
            start_of_plain_text = ptr;
            token_processed = TRUE;
        } else if (g_ascii_isdigit(*ptr) || (*ptr == '.' && g_ascii_isdigit(*(ptr + 1)))) { // 4. Match numbers
            const char* num_scan_ptr = ptr;
            if (*num_scan_ptr == '0' && (*(num_scan_ptr+1) == 'x' || *(num_scan_ptr+1) == 'X')) { // Hex
                num_scan_ptr += 2;
                while (g_ascii_isxdigit(*num_scan_ptr)) num_scan_ptr++;
            } else if (*num_scan_ptr == '0' && (*(num_scan_ptr+1) == 'o' || *(num_scan_ptr+1) == 'O')) { // Octal
                num_scan_ptr += 2;
                while (*num_scan_ptr >= '0' && *num_scan_ptr <= '7') num_scan_ptr++;
            } else if (*num_scan_ptr == '0' && (*(num_scan_ptr+1) == 'b' || *(num_scan_ptr+1) == 'B')) { // Binary
                num_scan_ptr += 2;
                while (*num_scan_ptr == '0' || *num_scan_ptr == '1') num_scan_ptr++;
            } else { // Decimal or float
                while (g_ascii_isdigit(*num_scan_ptr)) num_scan_ptr++;
                if (*num_scan_ptr == '.') {
                    num_scan_ptr++;
                    while (g_ascii_isdigit(*num_scan_ptr)) num_scan_ptr++;
                }
                if (*num_scan_ptr == 'e' || *num_scan_ptr == 'E') { // Exponent
                    num_scan_ptr++;
                    if (*num_scan_ptr == '+' || *num_scan_ptr == '-') num_scan_ptr++;
                    while (g_ascii_isdigit(*num_scan_ptr)) num_scan_ptr++;
                }
            }
            append_and_highlight(highlighted_code, start_of_plain_text, current_token_start, "#ff9e64", num_scan_ptr - ptr);
            ptr = num_scan_ptr; // Advance ptr
            start_of_plain_text = ptr;
            token_processed = TRUE;
        } else if (g_ascii_isalpha(*ptr) || *ptr == '_') { // 5. Match keywords or functions
            const char *word_scan_ptr = ptr;
            while (g_ascii_isalnum(*word_scan_ptr) || *word_scan_ptr == '_') word_scan_ptr++;
            char *word = g_strndup(ptr, word_scan_ptr - ptr);
            const char* color = NULL;
            if (g_hash_table_lookup(keywords_ht, word)) {
                color = "#f7768e"; // Red for keywords
            } else {
                const char* lookahead = word_scan_ptr;
                while (*lookahead != '\0' && isspace(*lookahead)) lookahead++;
                if (*lookahead == '(' && g_hash_table_lookup(standard_functions_ht, word)) {
                    color = "#7aa2f7"; // Blue for functions
                }
            }
            if (color) { // Only highlight if it's a keyword or function
                append_and_highlight(highlighted_code, start_of_plain_text, current_token_start, color, word_scan_ptr - ptr);
                ptr = word_scan_ptr; // Advance ptr
                start_of_plain_text = ptr;
                token_processed = TRUE;
            }
            g_free(word);
        } else { // 6. Try to match operators
            for (int i = 0; python_sorted_operators[i] != NULL; i++) {
                size_t op_len = strlen(python_sorted_operators[i]);
                if (strncmp(ptr, python_sorted_operators[i], op_len) == 0) {
                    append_and_highlight(highlighted_code, start_of_plain_text, current_token_start, "#bb9af7", op_len);
                    ptr += op_len; // Advance ptr
                    start_of_plain_text = ptr;
                    token_processed = TRUE;
                    break;
                }
            }
        }

        if (!token_processed) {
            // If no special token was found, it's plain text. Just advance ptr.
            ptr++;
        }
    }

    // After the loop, append any remaining plain text
    if (start_of_plain_text != ptr) {
        char *plain_text = g_strndup(start_of_plain_text, ptr - start_of_plain_text);
        char *escaped_plain_text = g_markup_escape_text(plain_text, -1);
        g_string_append(highlighted_code, escaped_plain_text);
        g_free(escaped_plain_text);
        g_free(plain_text);
    }

    return g_string_free(highlighted_code, FALSE);
}