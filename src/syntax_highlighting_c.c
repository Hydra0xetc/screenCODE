#include <ctype.h>
#include <glib.h> // Using GLib for efficient hash tables.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syntax_highlighting.h"

// Make the global hash tables available in this file.
extern GHashTable *keywords_ht;
extern GHashTable *preprocessor_directives_ht;
extern GHashTable *standard_functions_ht;

/**
 * @brief A helper function to simplify appending highlighted code.
 *        It takes plain text, escapes it, and wraps it in a Pango span tag with
 * a color.
 * @return TRUE on success, FALSE if memory allocation fails.
 */
static gboolean append_and_highlight(GString *highlighted_code,
                                     const char *start_of_plain_text,
                                     const char *current_token_start,
                                     const char *color,
                                     size_t len) {
    // Append any plain text that came before the token.
    if (start_of_plain_text < current_token_start) {
        char *plain_text = g_strndup(start_of_plain_text,
                                     current_token_start - start_of_plain_text);
        if (!plain_text)
            return FALSE; // Memory allocation failed
        char *escaped_plain_text = g_markup_escape_text(plain_text, -1);
        if (!escaped_plain_text) {
            g_free(plain_text);
            return FALSE;
        } // Memory allocation failed
        g_string_append(highlighted_code, escaped_plain_text);
        g_free(escaped_plain_text);
        g_free(plain_text);
    }

    // Append the highlighted token itself.
    char *token_text = g_strndup(current_token_start, len);
    if (!token_text)
        return FALSE; // Memory allocation failed
    char *escaped_token = g_markup_escape_text(token_text, -1);
    if (!escaped_token) {
        g_free(token_text);
        return FALSE;
    } // Memory allocation failed

    if (color) {
        g_string_append_printf(highlighted_code,
                               "<span foreground='%s'>%s</span>",
                               color,
                               escaped_token);
    } else {
        g_string_append(highlighted_code, escaped_token);
    }

    g_free(escaped_token);
    g_free(token_text);
    return TRUE; // Success
}

/**
 * @brief Initializes hash tables with C keywords, preprocessor directives, and
 * standard functions. This is done once to make syntax lookups much faster.
 */

void init_syntax_tables_c() {
    keywords_ht = g_hash_table_new(g_str_hash, g_str_equal);
    const char *keywords[] = {// C Standard Keywords (C89/C90)
                              "auto",
                              "break",
                              "case",
                              "char",
                              "const",
                              "continue",
                              "default",
                              "do",
                              "double",
                              "double_t",
                              "else",
                              "enum",
                              "extern",
                              "float",
                              "float8",
                              "float_t",
                              "for",
                              "goto",
                              "if",
                              "int",
                              "int64_t",
                              "uint_t",
                              "uint64_t",
                              "uint8_t",
                              "bool",
                              "long",
                              "register",
                              "return",
                              "short",
                              "signed",
                              "sizeof",
                              "static",
                              "struct",
                              "switch",
                              "typedef",
                              "union",
                              "unsigned",
                              "void",
                              "volatile",
                              "while",
                              // C99 Keywords
                              "_Bool",
                              "_Complex",
                              "_Imaginary",
                              "inline",
                              "restrict",
                              // C11 Keywords
                              "_Alignas",
                              "_Alignof",
                              "_Atomic",
                              "_Generic",
                              "_Noreturn",
                              "_Static_assert",
                              "_Thread_local",
                              NULL};
    for (int i = 0; keywords[i] != NULL; i++) {
        g_hash_table_insert(
            keywords_ht, (gpointer)keywords[i], GINT_TO_POINTER(1));
    }

    preprocessor_directives_ht = g_hash_table_new(g_str_hash, g_str_equal);
    const char *preprocessor_directives[] = {"#include",
                                             "#define",
                                             "#undef",
                                             "#if",
                                             "#ifdef",
                                             "#ifndef",
                                             "#else",
                                             "#elif",
                                             "#endif",
                                             "#error",
                                             "#pragma",
                                             "#line",
                                             NULL};
    for (int i = 0; preprocessor_directives[i] != NULL; i++) {
        g_hash_table_insert(preprocessor_directives_ht,
                            (gpointer)preprocessor_directives[i],
                            GINT_TO_POINTER(1));
    }

    standard_functions_ht = g_hash_table_new(g_str_hash, g_str_equal);
    const char *standard_functions[] = {
        // <stdio.h>
        "printf",
        "scanf",
        "sprintf",
        "snprintf",
        "sscanf",
        "fprintf",
        "fscanf",
        "vprintf",
        "vfprintf",
        "vsprintf",
        "vsnprintf",
        "fgetc",
        "fputc",
        "fgets",
        "fputs",
        "getc",
        "getchar",
        "gets",
        "putc",
        "putchar",
        "puts",
        "ungetc",
        "fopen",
        "freopen",
        "fclose",
        "fflush",
        "setbuf",
        "setvbuf",
        "fread",
        "fwrite",
        "fseek",
        "ftell",
        "rewind",
        "fgetpos",
        "fsetpos",
        "clearerr",
        "feof",
        "ferror",
        "perror",
        "remove",
        "rename",
        "tmpfile",
        "tmpnam",

        // <stdlib.h>
        "malloc",
        "calloc",
        "realloc",
        "free",
        "atoi",
        "atol",
        "atoll",
        "atof",
        "strtod",
        "strtof",
        "strtold",
        "strtol",
        "strtoll",
        "strtoul",
        "strtoull",
        "rand",
        "srand",
        "abort",
        "exit",
        "atexit",
        "quick_exit",
        "_Exit",
        "getenv",
        "system",
        "bsearch",
        "qsort",
        "abs",
        "labs",
        "llabs",
        "div",
        "ldiv",
        "lldiv",

        // <string.h>
        "strcpy",
        "strncpy",
        "strcat",
        "strncat",
        "strlen",
        "strcmp",
        "strncmp",
        "strchr",
        "strrchr",
        "strstr",
        "strtok",
        "strspn",
        "strcspn",
        "strpbrk",
        "strerror",
        "memset",
        "memcpy",
        "memmove",
        "memcmp",
        "memchr",

        // <math.h>
        "sin",
        "cos",
        "tan",
        "asin",
        "acos",
        "atan",
        "atan2",
        "sinh",
        "cosh",
        "tanh",
        "exp",
        "log",
        "log10",
        "pow",
        "sqrt",
        "ceil",
        "floor",
        "fmod",
        "modf",
        "frexp",
        "ldexp",
        "fabs",
        "hypot",
        "fmax",
        "fmin",
        "fdim",
        "trunc",
        "round",
        "nearbyint",
        "rint",
        "copysign",
        "nan",
        "nextafter",
        "nexttoward",
        "fma",
        "log1p",
        "logb",
        "ilogb",
        "expm1",
        "cbrt",
        "erf",
        "erfc",
        "lgamma",
        "tgamma",

        // <time.h>
        "clock",
        "time",
        "difftime",
        "mktime",
        "gmtime",
        "localtime",
        "asctime",
        "ctime",
        "strftime",

        // <ctype.h>
        "isalnum",
        "isalpha",
        "isblank",
        "iscntrl",
        "isdigit",
        "isgraph",
        "islower",
        "isprint",
        "ispunct",
        "isspace",
        "isupper",
        "isxdigit",
        "tolower",
        "toupper",

        // <locale.h>
        "setlocale",
        "localeconv",

        // <signal.h>
        "signal",
        "raise",

        // <stdarg.h>
        "va_start",
        "va_arg",
        "va_end",
        "va_copy",

        // <wchar.h> (Wide character functions)
        "fgetwc",
        "fputwc",
        "getwc",
        "getwchar",
        "putwc",
        "putwchar",
        "ungetwc",
        "wcscpy",
        "wcsncpy",
        "wcscat",
        "wcsncat",
        "wcslen",
        "wcscmp",
        "wcsncmp",
        "wcschr",
        "wcsrchr",
        "wcswcs",
        "wcstok",
        "wcsftime",
        "wctomb",
        "mbstowcs",
        "wcstombs",

        // <wctype.h> (Wide character classification)
        "iswalnum",
        "iswalpha",
        "iswblank",
        "iswcntrl",
        "iswdigit",
        "iswgraph",
        "iswlower",
        "iswprint",
        "iswpunct",
        "iswspace",
        "iswupper",
        "iswxdigit",
        "towlower",
        "towupper",

        // <fontconfig.h>
        "FcInit",
        "FcFini",
        "FcInitLoadConfig",
        "FcInitLoadConfigAndFonts",
        "FcMatchFont",
        "FcFontMatch",
        "FcFontList",
        "FcPatternCreate",
        "FcPatternAddString",
        "FcPatternAddBool",
        "FcPatternGetConfig",

        NULL};
    for (int i = 0; standard_functions[i] != NULL; i++) {
        g_hash_table_insert(standard_functions_ht,
                            (gpointer)standard_functions[i],
                            GINT_TO_POINTER(1));
    }
}

/**
 * @brief Frees the memory used by the C syntax hash tables.
 */
void free_syntax_tables_c() {
    if (keywords_ht)
        g_hash_table_unref(keywords_ht);
    if (preprocessor_directives_ht)
        g_hash_table_unref(preprocessor_directives_ht);
    if (standard_functions_ht)
        g_hash_table_unref(standard_functions_ht);
    keywords_ht = NULL;
    preprocessor_directives_ht = NULL;
    standard_functions_ht = NULL;
}

// C operators, sorted by length to ensure the longest match is found first
// (e.g., ">>=" before ">>", etc.).
static const char *c_sorted_operators[] = {
    ">>=", "<<=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", // 3 chars
    "==",  "!=",  "<=", ">=", "&&", "||", "->", "++", "--",       // 2 chars
    "+",   "-",   "*",  "/",  "%",  "=",  "<",  ">",  "!",  "&",
    "|",   "^",   "~",  "<<", ">>", ".",  "?",  ":", // 1 char
    NULL};

/**
 * @brief Highlights tokens on a single line of C code, handling multi-line
 * comment state.
 * @param highlighted_line_gstring The GString to append the highlighted tokens
 * to.
 * @param line_content The content of the single line to highlight.
 * @param in_multiline_comment A pointer to a boolean that tracks if we are
 * inside a multi-line comment.
 * @return TRUE on success, FALSE if memory allocation fails.
 */
static gboolean highlight_tokens_on_line(GString *highlighted_line_gstring,
                                         const char *line_content,
                                         gboolean *in_multiline_comment) {
    const char *ptr = line_content;
    const char *start_of_plain_text = line_content;

    while (*ptr != '\0') {
        const char *current_token_start = ptr;
        size_t token_len = 0;
        const char *token_color = NULL;

        if (*in_multiline_comment) {
            const char *scan_ptr = ptr;
            while (*scan_ptr != '\0' &&
                   !(*scan_ptr == '*' && *(scan_ptr + 1) == '/')) {
                scan_ptr++;
            }
            if (*scan_ptr == '\0') {
                token_len = strlen(ptr);
                *in_multiline_comment = TRUE;
            } else {
                token_len = (scan_ptr + 2) - ptr;
                *in_multiline_comment = FALSE;
            }
            if (!append_and_highlight(highlighted_line_gstring,
                                      start_of_plain_text,
                                      current_token_start,
                                      "#545c7e",
                                      token_len))
                return FALSE;
            ptr += token_len;
            start_of_plain_text = ptr;
            continue;
        }

        if (*ptr == '#') {
            const char *directive_end = ptr + 1;
            while (g_ascii_isalpha(*directive_end))
                directive_end++;
            char *directive_word = g_strndup(ptr, directive_end - ptr);
            if (!directive_word)
                return FALSE;

            if (strcmp(directive_word, "#include") == 0) {
                if (!append_and_highlight(highlighted_line_gstring,
                                          start_of_plain_text,
                                          ptr,
                                          "#7aa2f7",
                                          directive_end - ptr)) {
                    g_free(directive_word);
                    return FALSE;
                }
                g_free(directive_word);
                ptr = directive_end;
                while (*ptr != '\0' && isspace(*ptr))
                    ptr++;

                char end_char = (*ptr == '<') ? '>' : '"';
                if (*ptr == '<' || *ptr == '"') {
                    const char *header_start = ptr;
                    const char *header_end = header_start + 1;
                    while (*header_end != '\0' && *header_end != end_char)
                        header_end++;
                    if (*header_end == end_char)
                        header_end++;

                    if (!append_and_highlight(highlighted_line_gstring,
                                              directive_end,
                                              header_start,
                                              "#9ece6a",
                                              header_end - header_start))
                        return FALSE;
                    ptr = header_end;
                }
                start_of_plain_text = ptr;
                continue;
            } else if (g_hash_table_lookup(preprocessor_directives_ht,
                                           directive_word)) {
                token_len = directive_end - ptr;
                token_color = "#7aa2f7";
            }
            g_free(directive_word);
        } else if (*ptr == '/' && *(ptr + 1) == '*') {
            const char *scan_ptr = ptr + 2;
            while (*scan_ptr != '\0' &&
                   !(*scan_ptr == '*' && *(scan_ptr + 1) == '/')) {
                scan_ptr++;
            }
            if (*scan_ptr == '\0') {
                *in_multiline_comment = TRUE;
                token_len = strlen(ptr);
            } else {
                *in_multiline_comment = FALSE;
                token_len = (scan_ptr + 2) - ptr;
            }
            token_color = "#545c7e";
        } else if (*ptr == '/' && *(ptr + 1) == '/') {
            token_len = strlen(ptr);
            token_color = "#545c7e";
        } else if (*ptr == '"' || *ptr == '\'') {
            char quote_char = *ptr;
            const char *scan_ptr = ptr + 1;
            while (*scan_ptr != '\0' && *scan_ptr != '\n') {
                if (*scan_ptr == '\\' && *(scan_ptr + 1) != '\0') {
                    scan_ptr++;
                } else if (*scan_ptr == quote_char) {
                    scan_ptr++;
                    break;
                }
                scan_ptr++;
            }
            token_len = scan_ptr - ptr;
            token_color = "#9ece6a";
        } else if (g_ascii_isdigit(*ptr) ||
                   (*ptr == '.' && g_ascii_isdigit(*(ptr + 1)))) {
            const char *num_scan_ptr = ptr;
            while (g_ascii_isdigit(*num_scan_ptr) || *num_scan_ptr == '.' ||
                   g_ascii_isxdigit(*num_scan_ptr) || *num_scan_ptr == 'x' ||
                   *num_scan_ptr == 'X')
                num_scan_ptr++;
            token_len = num_scan_ptr - ptr;

            // Add check for word boundary after number
            if (*num_scan_ptr != '\0' &&
                (g_ascii_isalnum(*num_scan_ptr) || *num_scan_ptr == '_')) {
                // This is part of an identifier, not a standalone number
                token_color = NULL; // Do not highlight
            } else {
                token_color = "#ff9e64"; // Highlight as number
            }
        } else if (g_ascii_isalpha(*ptr) || *ptr == '_') {
            const char *word_scan_ptr = ptr;
            while (g_ascii_isalnum(*word_scan_ptr) || *word_scan_ptr == '_')
                word_scan_ptr++;
            char *word = g_strndup(ptr, word_scan_ptr - ptr);
            if (!word)
                return FALSE;

            // Check if the character after the word is a word boundary
            gboolean is_word_boundary =
                (*word_scan_ptr == '\0' ||
                 (!g_ascii_isalnum(*word_scan_ptr) && *word_scan_ptr != '_'));

            if (is_word_boundary) { // Only highlight if it's a whole word
                if (g_hash_table_lookup(keywords_ht, word)) {
                    token_color = "#f7768e";
                } else if (g_hash_table_lookup(standard_functions_ht, word)) {
                    const char *lookahead = word_scan_ptr;
                    while (*lookahead != '\0' && isspace(*lookahead))
                        lookahead++;
                    if (*lookahead == '(') {
                        token_color = "#7aa2f7";
                    }
                }
            }
            g_free(word);
            token_len = word_scan_ptr - ptr;
        } else { // Operators
            for (int i = 0; c_sorted_operators[i] != NULL; i++) {
                size_t op_len = strlen(c_sorted_operators[i]);
                if (strncmp(ptr, c_sorted_operators[i], op_len) == 0) {
                    token_len = op_len;
                    token_color = "#bb9af7";
                    break;
                }
            }
        }

        if (token_len > 0) {
            if (!append_and_highlight(highlighted_line_gstring,
                                      start_of_plain_text,
                                      current_token_start,
                                      token_color,
                                      token_len))
                return FALSE;
            ptr += token_len;
            start_of_plain_text = ptr;
        } else {
            // No specific token matched, so this single character is plain
            // text. Append it as plain text.
            if (!append_and_highlight(highlighted_line_gstring,
                                      start_of_plain_text,
                                      current_token_start,
                                      NULL,
                                      1))
                return FALSE;
            ptr++;
            start_of_plain_text = ptr; // Crucial: advance start_of_plain_text
        }
    }

    if (start_of_plain_text < ptr) {
        char *plain_text =
            g_strndup(start_of_plain_text, ptr - start_of_plain_text);
        if (!plain_text)
            return FALSE;
        char *escaped_plain_text = g_markup_escape_text(plain_text, -1);
        if (!escaped_plain_text) {
            g_free(plain_text);
            return FALSE;
        }
        g_string_append(highlighted_line_gstring, escaped_plain_text);
        g_free(escaped_plain_text);
        g_free(plain_text);
    }
    return TRUE;
}

/**
 * @brief Main C syntax highlighting logic.
 *        It iterates through the code line by line, prepends line numbers if
 * enabled, and then highlights tokens on each line.
 * @return A new string containing the code with Pango markup for highlighting,
 * or NULL on memory allocation failure.
 */
char *highlight_c_syntax(const char *code, gboolean show_line_numbers) {
    GString *final_highlighted_code = g_string_new("");
    if (!final_highlighted_code)
        return NULL; // Memory allocation failed

    char **code_lines = g_strsplit(code, "\n", -1);
    if (!code_lines) {
        g_string_free(final_highlighted_code, TRUE);
        return NULL;
    }

    // Calculate max line number width for consistent padding
    int max_line_number = 0;
    for (char **line_ptr = code_lines; *line_ptr != NULL; line_ptr++) {
        max_line_number++;
    }

    // Adjust max_line_number if the last line is empty due to a trailing
    // newline
    if (max_line_number > 0 && strlen(code_lines[max_line_number - 1]) == 0) {
        max_line_number--;
    }

    int line_number_width = 0;
    if (show_line_numbers) {
        // Use snprintf for safety to prevent buffer overflows.
        // A buffer of 12 is safe for typical 32-bit integer line numbers.
        char temp_buf[12];
        snprintf(temp_buf, sizeof(temp_buf), "%d", max_line_number);
        line_number_width = strlen(temp_buf);
    }

    int current_line_num = 1;
    gboolean in_multiline_comment =
        FALSE; // State tracking for multi-line comments

    for (char **line_ptr = code_lines; *line_ptr != NULL; line_ptr++) {
        // Skip empty last line if it was a trailing newline
        if (current_line_num > max_line_number && strlen(*line_ptr) == 0) {
            continue;
        }

        GString *current_line_gstring = g_string_new("");
        if (!current_line_gstring) {
            g_strfreev(code_lines);
            g_string_free(final_highlighted_code, TRUE);
            return NULL;
        }

        // Prepend line number if enabled
        if (show_line_numbers) {
            // Use a fixed-width format for line numbers to align code nicely
            g_string_append_printf(current_line_gstring,
                                   "<span foreground='#545c7e'>%*d </span>",
                                   line_number_width,
                                   current_line_num);
        }

        // Highlight tokens on the current line
        if (!highlight_tokens_on_line(
                current_line_gstring, *line_ptr, &in_multiline_comment)) {
            g_string_free(current_line_gstring, TRUE);
            g_strfreev(code_lines);
            g_string_free(final_highlighted_code, TRUE);
            return NULL;
        }

        g_string_append(final_highlighted_code, current_line_gstring->str);
        g_string_free(current_line_gstring, TRUE);

        if (line_ptr[1] != NULL) { // Don't add a newline for the very last line
            g_string_append_c(final_highlighted_code, '\n');
        }

        current_line_num++;
    }

    g_strfreev(code_lines); // Free the array of strings

    return g_string_free(final_highlighted_code, FALSE);
}
