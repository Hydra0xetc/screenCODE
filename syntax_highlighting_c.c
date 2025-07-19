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

// C Language Syntax Highlighting
void init_syntax_tables_c() {
    keywords_ht = g_hash_table_new(g_str_hash, g_str_equal);
    const char* keywords[] = {
        "auto", "break", "case", "char", "const", "continue", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if",
        "int", "long", "register", "return", "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while",
        "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary",
        "_Noreturn", "_Static_assert", "_Thread_local", "alignas", "alignof", "and",
        "and_eq", "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept", "bitand",
        "bitor", "bool", "catch", "char16_t", "char32_t", "class", "compl", "concept",
        "const_cast", "constexpr", "decltype", "delete", "dynamic_cast", "explicit",
        "export", "false", "friend", "inline", "mutable", "namespace", "new", "noexcept",
        "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private", "protected",
        "public", "reinterpret_cast", "requires", "restrict", "static_assert",
        "static_cast", "template", "this", "thread_local", "throw", "true", "try",
        "typeid", "typename", "using", "virtual", "wchar_t", "xor", "xor_eq",
        NULL
    };
    for (int i = 0; keywords[i] != NULL; i++) {
        g_hash_table_insert(keywords_ht, (gpointer)keywords[i], GINT_TO_POINTER(1));
    }

    preprocessor_directives_ht = g_hash_table_new(g_str_hash, g_str_equal);
    const char* preprocessor_directives[] = {
        "#include", "#define", "#undef", "#if", "#ifdef", "#ifndef", "#else", "#elif", "#endif", "#error", "#pragma", NULL
    };
    for (int i = 0; preprocessor_directives[i] != NULL; i++) {
        g_hash_table_insert(preprocessor_directives_ht, (gpointer)preprocessor_directives[i], GINT_TO_POINTER(1));
    }

    standard_functions_ht = g_hash_table_new(g_str_hash, g_str_equal);
    const char* standard_functions[] = {
        "printf", "scanf", "malloc", "free", "strcpy", "strcat", "strlen", "strcmp",
        "abs", "sqrt", "sin", "cos", "tan", "log", "pow", "fopen", "fclose", "fread", "fwrite",
        "exit", "rand", "srand", "time",
        "calloc", "realloc", "memcpy", "memmove", "memset", "strchr", "strrchr",
        "strstr", "strtok", "atoi", "atol", "atoll", "atof", "sprintf", "snprintf",
        "sscanf", "fgets", "fputs", "feof", "ferror", "perror", "remove", "rename",
        "tmpfile", "tmpnam", "setvbuf", "setbuf", "getchar", "putchar", "gets", "puts",
        "ungetc", "fgetc", "fputc", "fgetpos", "fsetpos", "ftell", "fseek", "rewind",
        "clearerr", "feof", "ferror", "qsort", "bsearch", "abort", "atexit", "getenv",
        "system", "bsearch", "qsort", "labs", "llabs", "div", "ldiv", "lldiv",
        "rand", "srand", "calloc", "realloc", "_Exit", "at_quick_exit", "quick_exit",
        "strtod", "strtof", "strtold", "strtol", "strtoll", "strtoul", "strtoull",
        "memcpy", "memmove", "memset", "strcoll", "strxfrm", "gmtime", "localtime",
        "mktime", "asctime", "ctime", "strftime", "wctomb", "mbstowcs", "wcstombs",
        NULL
    };
    for (int i = 0; standard_functions[i] != NULL; i++) {
        g_hash_table_insert(standard_functions_ht, (gpointer)standard_functions[i], GINT_TO_POINTER(1));
    }
}

void free_syntax_tables_c() {
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
}

// Sorted operators for longest match first (C)
static const char* c_sorted_operators[] = {
    ">>=", "<<=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", // 3 chars
    "==", "!=", "<=", ">=", "&&", "||", "->", "++", "--",         // 2 chars
    "+", "-", "*", "/", "%", "=", "<", ">", "!", "&", "|", "^", "~", "<<", ">>", ".", "?", ":", // 1 char
    NULL
};

char* highlight_c_syntax(const char* code) {
    GString* highlighted_code = g_string_new("");
    const char *ptr = code;
    const char *start_of_plain_text = code;

    while (*ptr != '\0') {
        const char *current_token_start = ptr;
        size_t token_len = 0;
        const char* token_color = NULL;

        // 1. Match string literals
        if (*ptr == '"') {
            const char *scan_ptr = ptr + 1;
            while (*scan_ptr != '\0' && *scan_ptr != '\n') {
                if (*scan_ptr == '\\' && *(scan_ptr + 1) != '\0') {
                    scan_ptr++; // Skip escaped char
                } else if (*scan_ptr == '"') {
                    scan_ptr++; // Include closing quote
                    break;
                }
                scan_ptr++;
            }
            token_len = scan_ptr - ptr;
            token_color = "#9ece6a"; // Green for strings
        } else if (*ptr == '\'') { // 2. Match character literals
            const char *scan_ptr = ptr + 1;
            while (*scan_ptr != '\0' && *scan_ptr != '\n') {
                if (*scan_ptr == '\\' && *(scan_ptr + 1) != '\0') {
                    scan_ptr++; // Skip escaped char
                } else if (*scan_ptr == '\'') {
                    scan_ptr++; // Include closing quote
                    break;
                }
                scan_ptr++;
            }
            token_len = scan_ptr - ptr;
            token_color = "#9ece6a"; // Green for chars
        } else if (*ptr == '/' && *(ptr + 1) == '*') { // 3. Match block comments
            const char *scan_ptr = ptr + 2;
            while (*scan_ptr != '\0' && !(*scan_ptr == '*' && *(scan_ptr + 1) == '/')) {
                scan_ptr++;
            }
            if (*scan_ptr != '\0') {
                scan_ptr += 2; // Skip "*/"
            }
            token_len = scan_ptr - ptr;
            token_color = "#545c7e"; // Grey for comments
        } else if (*ptr == '/' && *(ptr + 1) == '/') { // 3. Match line comments
            const char *scan_ptr = ptr + 2;
            while (*scan_ptr != '\0' && *scan_ptr != '\n') {
                scan_ptr++;
            }
            token_len = scan_ptr - ptr;
            token_color = "#545c7e"; // Grey for comments
        } else if (g_ascii_isdigit(*ptr) || (*ptr == '.' && g_ascii_isdigit(*(ptr + 1)))) { // 4. Match numbers
            const char* num_scan_ptr = ptr;
            if (*num_scan_ptr == '0' && (*(num_scan_ptr+1) == 'x' || *(num_scan_ptr+1) == 'X')) {
                num_scan_ptr += 2;
                while (g_ascii_isxdigit(*num_scan_ptr)) num_scan_ptr++;
            } else {
                while (g_ascii_isdigit(*num_scan_ptr)) num_scan_ptr++;
                if (*num_scan_ptr == '.') {
                    num_scan_ptr++;
                    while (g_ascii_isdigit(*num_scan_ptr)) num_scan_ptr++;
                }
            }
            while (*num_scan_ptr == 'u' || *num_scan_ptr == 'U' || *num_scan_ptr == 'l' || *num_scan_ptr == 'L' || *num_scan_ptr == 'f' || *num_scan_ptr == 'F') {
                num_scan_ptr++;
            }
            token_len = num_scan_ptr - ptr;
            token_color = "#ff9e64"; // Orange for numbers
        } else if (g_ascii_isalpha(*ptr) || *ptr == '_' || *ptr == '#') { // 5. Match keywords, preprocessor, functions
            const char *word_scan_ptr = ptr;
            if (*word_scan_ptr == '#') { // Preprocessor
                while (g_ascii_isalnum(*word_scan_ptr) || *word_scan_ptr == '#') word_scan_ptr++;
                char *word = g_strndup(current_token_start, word_scan_ptr - current_token_start);
                if (g_hash_table_lookup(preprocessor_directives_ht, word)) {
                    while (*word_scan_ptr != '\0' && *word_scan_ptr != '\n') word_scan_ptr++;
                    token_len = word_scan_ptr - current_token_start;
                    token_color = "#7aa2f7"; // Blue for preprocessor
                }
                g_free(word);
            } else {
                while (g_ascii_isalnum(*word_scan_ptr) || *word_scan_ptr == '_') word_scan_ptr++;
                char *word = g_strndup(current_token_start, word_scan_ptr - current_token_start);
                if (g_hash_table_lookup(keywords_ht, word)) {
                    token_len = word_scan_ptr - current_token_start;
                    token_color = "#f7768e"; // Red for keywords
                } else {
                    const char* lookahead = word_scan_ptr;
                    while (*lookahead != '\0' && isspace(*lookahead)) lookahead++;
                    if (*lookahead == '(' && g_hash_table_lookup(standard_functions_ht, word)) {
                        token_len = word_scan_ptr - current_token_start;
                        token_color = "#7aa2f7"; // Blue for functions
                    }
                }
                g_free(word);
            }
        } else { // 6. Try to match operators
            for (int i = 0; c_sorted_operators[i] != NULL; i++) {
                size_t op_len = strlen(c_sorted_operators[i]);
                if (strncmp(ptr, c_sorted_operators[i], op_len) == 0) {
                    token_len = op_len;
                    token_color = "#bb9af7"; // Purple for operators
                    break;
                }
            }
        }

        if (token_len > 0) {
            append_and_highlight(highlighted_code, start_of_plain_text, current_token_start, token_color, token_len);
            ptr += token_len;
            start_of_plain_text = ptr;
        } else {
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