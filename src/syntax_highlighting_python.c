#include <ctype.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syntax_highlighting.h"

// Make the global hash tables available in this file.
extern GHashTable *keywords_ht;
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
    if (start_of_plain_text != current_token_start) {
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
 * @brief Initializes hash tables with Python keywords and built-in functions.
 */
void init_syntax_tables_python() {
    keywords_ht = g_hash_table_new(g_str_hash, g_str_equal);
    const char *keywords[] = {
        "import", "False", "None",   "True",    "and",      "as",   "assert",
        "async",  "await", "break",  "class",   "continue", "def",  "del",
        "elif",   "else",  "except", "finally", "for",      "from", "global",
        "if",     "in",    "is",     "lambda",  "nonlocal", "not",  "or",
        "pass",   "raise", "return", "try",     "while",    "with", "yield",
        "match",  "case",  NULL};
    for (int i = 0; keywords[i] != NULL; i++) {
        g_hash_table_insert(
            keywords_ht, (gpointer)keywords[i], GINT_TO_POINTER(1));
    }

    standard_functions_ht = g_hash_table_new(g_str_hash, g_str_equal);
    const char *standard_functions[] = {
        "print",      "input",       "len",
        "range",      "sum",         "max",
        "min",        "abs",         "round",
        "open",       "close",       "read",
        "write",      "append",      "strip",
        "split",      "join",        "int",
        "float",      "str",         "list",
        "tuple",      "dict",        "set",
        "bool",       "type",        "id",
        "dir",        "help",        "isinstance",
        "issubclass", "super",       "hasattr",
        "getattr",    "setattr",     "delattr",
        "callable",   "frozenset",   "complex",
        "divmod",     "enumerate",   "filter",
        "map",        "next",        "iter",
        "pow",        "reversed",    "slice",
        "sorted",     "zip",         "__import__",
        "any",        "all",         "chr",
        "ord",        "hex",         "oct",
        "bin",        "classmethod", "staticmethod",
        "property",   "bytearray",   "bytes",
        "memoryview", "system",      "ascii",
        "breakpoint", "compile",     "eval",
        "exec",       "format",      "globals",
        "locals",     "repr",        "vars",
        "aiter",      "anext",       "__build_class__",
        "__debug__",  "__doc__",     "__loader__",
        "__name__",   "__package__", "__spec__",
        "copyright",  "credits",     "exit",
        "license",    "quit",        "acos",
        "acosh",      "asin",        "asinh",
        "atan",       "atan2",       "atanh",
        "ceil",       "comb",        "copysign",
        "cos",        "cosh",        "degrees",
        "dist",       "erf",         "erfc",
        "exp",        "expm1",       "fabs",
        "factorial",  "floor",       "fmod",
        "frexp",      "fsum",        "gamma",
        "gcd",        "hypot",       "isclose",
        "isfinite",   "isinf",       "isnan",
        "isqrt",      "ldexp",       "lgamma",
        "log",        "log10",       "log1p",
        "log2",       "modf",        "perm",
        "pow",        "prod",        "radians",
        "remainder",  "sin",         "sinh",
        "sqrt",       "tan",         "tanh",
        "trunc",      NULL};
    for (int i = 0; standard_functions[i] != NULL; i++) {
        g_hash_table_insert(standard_functions_ht,
                            (gpointer)standard_functions[i],
                            GINT_TO_POINTER(1));
    }
}

/**
 * @brief Frees the memory used by the Python syntax hash tables.
 */
void free_syntax_tables_python() {
    if (keywords_ht)
        g_hash_table_unref(keywords_ht);
    if (standard_functions_ht)
        g_hash_table_unref(standard_functions_ht);
    keywords_ht = NULL;
    standard_functions_ht = NULL;
}

// Python operators, sorted by length to ensure the longest match is found
// first.
static const char *python_sorted_operators[] = {
    "**=", "//=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=",
    ">=>", "<<=",                         // 3 chars
    "==",  "!=",  ">=", "<=", "**", "//", // 2 chars
    "+",   "-",   "*",  "/",  "%",  "=",  ">",  "<",  "&",  "|",
    "^",   "~",   ".",  ":",  "[",  "]",  "{",  "}",  "(",  ")", // 1 char
    NULL};

/**
 * @brief Highlights tokens on a single line of Python code, handling multi-line
 * string/comment state.
 * @param highlighted_line_gstring The GString to append the highlighted tokens
 * to.
 * @param line_content The content of the single line to highlight.
 * @param in_multiline_string A pointer to a gchar that tracks if we are inside
 * a multi-line string, and what quote char it is.
 * @return TRUE on success, FALSE if memory allocation fails.
 */
static gboolean
highlight_tokens_on_line_python(GString *highlighted_line_gstring,
                                const char *line_content,
                                gchar *in_multiline_string) {
    gboolean expect_module_name = FALSE;
    gboolean expect_alias_name = FALSE;
    const char *ptr = line_content;
    const char *start_of_plain_text = line_content;

    // State for f-strings (local to the line)
    static gboolean in_f_string = FALSE;
    static char f_string_quote_char = 0;
    static int f_string_brace_level = 0;

    // Reset f-string state for a new line if not in a multi-line string
    if (*in_multiline_string == 0) {
        in_f_string = FALSE;
        f_string_brace_level = 0;
    }

    while (*ptr != '\0') {
        const char *current_token_start = ptr;
        size_t token_len = 0;
        const char *token_color = NULL;

        // If we are inside a multi-line string, we have special handling.
        if (*in_multiline_string != 0) {
            const char *scan_ptr = ptr;
            char quote_char = *in_multiline_string;
            while (*scan_ptr != '\0' &&
                   !(*scan_ptr == quote_char && *(scan_ptr + 1) == quote_char &&
                     *(scan_ptr + 2) == quote_char)) {
                scan_ptr++;
            }

            if (*scan_ptr == '\0') { // End of line, but not end of string
                token_len = strlen(ptr);
                token_color = "#9ece6a"; // Green
            } else {                     // Found closing triple quotes
                token_len = (scan_ptr + 3) - ptr;
                token_color = "#9ece6a"; // Green
                *in_multiline_string =
                    0; // We are now out of the multi-line string.
            }
        } else if (in_f_string) {
            if (*ptr == f_string_quote_char &&
                f_string_brace_level == 0) { // End of f-string
                in_f_string = FALSE;
                token_len = 1;
                token_color = "#9ece6a"; // Green for closing quote
            } else if (*ptr == '{') {
                if (*(ptr + 1) == '{') { // Escaped brace
                    token_len = 2;
                    token_color = "#9ece6a";
                } else {
                    f_string_brace_level++;
                    token_len = 1;
                    token_color = "#bb9af7"; // Purple for brace
                }
            } else if (*ptr == '}') {
                if (*(ptr + 1) == '}') { // Escaped brace
                    token_len = 2;
                    token_color = "#9ece6a";
                } else {
                    if (f_string_brace_level > 0) {
                        f_string_brace_level--;
                    }
                    token_len = 1;
                    token_color = "#bb9af7"; // Purple for brace
                }
            } else if (f_string_brace_level ==
                       0) { // Inside f-string, outside braces
                const char *scan_ptr = ptr;
                while (*scan_ptr != '\0' && *scan_ptr != f_string_quote_char &&
                       *scan_ptr != '{') {
                    scan_ptr++;
                }
                token_len = scan_ptr - ptr;
                token_color = "#9ece6a"; // Green for string part
            } else {                     // Inside braces, treat as code
                // This part will be handled by the general tokenizing logic
                // below
            }
        }

        if (token_len ==
            0) { // If not handled by f-string or multiline string logic
            // 1. Strings (f-string, triple, double, single quoted)
            if ((*ptr == 'f' || *ptr == 'F') &&
                (*(ptr + 1) == '"' || *(ptr + 1) == '\'')) { // f-string start
                in_f_string = TRUE;
                f_string_quote_char = *(ptr + 1);
                f_string_brace_level = 0;
                token_len = 2;           // f" or f'
                token_color = "#9ece6a"; // Green
            } else if ((*ptr == '"' && *(ptr + 1) == '"' &&
                        *(ptr + 2) == '"') ||
                       (*ptr == '\'' && *(ptr + 1) == '\'' &&
                        *(ptr + 2) == '\'')) {
                char quote_char = *ptr;
                const char *scan_ptr = ptr + 3;
                *in_multiline_string =
                    quote_char; // Enter multi-line string state
                while (*scan_ptr != '\0' && !(*scan_ptr == quote_char &&
                                              *(scan_ptr + 1) == quote_char &&
                                              *(scan_ptr + 2) == quote_char)) {
                    scan_ptr++;
                }
                if (*scan_ptr != '\0') {      // String ends on the same line
                    scan_ptr += 3;            // Skip closing quotes
                    *in_multiline_string = 0; // Exit state immediately
                }
                token_len = scan_ptr - ptr;
                token_color = "#9ece6a"; // Green
            } else if (*ptr == '"' || *ptr == '\'') {
                // Regular single-line strings
                char quote_char = *ptr;
                const char *scan_ptr = ptr + 1;
                while (*scan_ptr != '\0' && *scan_ptr != '\n') {
                    if (*scan_ptr == '\\' &&
                        *(scan_ptr + 1) != '\0') { // Skip escaped character.
                        scan_ptr++;
                    } else if (*scan_ptr == quote_char) {
                        scan_ptr++; // Include closing quote.
                        break;
                    }
                    scan_ptr++;
                }
                token_len = scan_ptr - ptr;
                token_color = "#9ece6a"; // Green
            }
            // 2. Comments
            else if (*ptr == '#') {
                const char *scan_ptr = ptr + 1;
                while (*scan_ptr != '\0' && *scan_ptr != '\n') {
                    scan_ptr++;
                }
                token_len = scan_ptr - ptr;
                token_color = "#545c7e"; // Grey
            }
            // 3. Numbers
            else if (g_ascii_isdigit(*ptr) ||
                     (*ptr == '.' && g_ascii_isdigit(*(ptr + 1)))) {
                const char *num_scan_ptr = ptr;
                if (*num_scan_ptr == '0' &&
                    (*(num_scan_ptr + 1) == 'x' ||
                     *(num_scan_ptr + 1) == 'X')) { // Hex
                    num_scan_ptr += 2;
                    while (g_ascii_isxdigit(*num_scan_ptr))
                        num_scan_ptr++;
                } else if (*num_scan_ptr == '0' &&
                           (*(num_scan_ptr + 1) == 'o' ||
                            *(num_scan_ptr + 1) == 'O')) { // Octal
                    num_scan_ptr += 2;
                    while (*num_scan_ptr >= '0' && *num_scan_ptr <= '7')
                        num_scan_ptr++;
                } else if (*num_scan_ptr == '0' &&
                           (*(num_scan_ptr + 1) == 'b' ||
                            *(num_scan_ptr + 1) == 'B')) { // Binary
                    num_scan_ptr += 2;
                    while (*num_scan_ptr == '0' || *num_scan_ptr == '1')
                        num_scan_ptr++;
                } else { // Decimal or float
                    while (g_ascii_isdigit(*num_scan_ptr))
                        num_scan_ptr++;
                    if (*num_scan_ptr == '.') {
                        num_scan_ptr++;
                        while (g_ascii_isdigit(*num_scan_ptr))
                            num_scan_ptr++;
                    }
                    if (*num_scan_ptr == 'e' ||
                        *num_scan_ptr == 'E') { // Exponent
                        num_scan_ptr++;
                        if (*num_scan_ptr == '+' || *num_scan_ptr == '-')
                            num_scan_ptr++;
                        while (g_ascii_isdigit(*num_scan_ptr))
                            num_scan_ptr++;
                    }
                }
                token_len = num_scan_ptr - ptr;

                // Add check for word boundary after number
                if (*num_scan_ptr != '\0' &&
                    (g_ascii_isalnum(*num_scan_ptr) || *num_scan_ptr == '_')) {
                    // This is part of an identifier, not a standalone number
                    token_color = NULL; // Do not highlight
                } else {
                    token_color = "#ff9e64"; // Highlight as number
                }
            }
            // 4. Keywords and Functions
            else if (g_ascii_isalpha(*ptr) || *ptr == '_') {
                const char *word_scan_ptr = ptr;
                while (g_ascii_isalnum(*word_scan_ptr) || *word_scan_ptr == '_')
                    word_scan_ptr++;
                char *word = g_strndup(current_token_start,
                                       word_scan_ptr - current_token_start);
                if (!word)
                    return FALSE; // Memory allocation failed

                // Check if the character after the word is a word boundary
                gboolean is_word_boundary =
                    (*word_scan_ptr == '\0' ||
                     (!g_ascii_isalnum(*word_scan_ptr) &&
                      *word_scan_ptr != '_'));

                if (is_word_boundary) { // Only highlight if it's a whole word
                    if (expect_module_name) {
                        token_color = "#9ece6a"; // Green for module name
                        expect_module_name = FALSE;
                    } else if (expect_alias_name) {
                        token_color = "#9ece6a"; // Green for alias name
                        expect_alias_name = FALSE;
                    } else if (g_hash_table_lookup(keywords_ht, word)) {
                        if (strcmp(word, "import") == 0 ||
                            strcmp(word, "from") == 0) {
                            token_color =
                                "#7aa2f7"; // Blue for 'import'/'from' keywords
                            expect_module_name = TRUE;
                        } else if (strcmp(word, "as") == 0) {
                            token_color = "#7aa2f7"; // Blue for 'as' keyword
                            expect_alias_name = TRUE;
                        } else {
                            token_color = "#f7768e"; // Red for other keywords
                        }
                    } else {
                        const char *lookahead = word_scan_ptr;
                        while (*lookahead != '\0' && isspace(*lookahead))
                            lookahead++;
                        if (*lookahead == '(' &&
                            g_hash_table_lookup(standard_functions_ht, word)) {
                            token_color = "#7aa2f7"; // Blue for functions
                        }
                    }
                }
                g_free(word);
                token_len = word_scan_ptr - current_token_start;
            }
            // 5. Operators
            else {
                for (int i = 0; python_sorted_operators[i] != NULL; i++) {
                    size_t op_len = strlen(python_sorted_operators[i]);
                    if (strncmp(ptr, python_sorted_operators[i], op_len) == 0) {
                        token_len = op_len;
                        token_color = "#bb9af7"; // Purple
                        break;
                    }
                }
            }
        }

        if (token_len > 0) {
            // If a token was matched, append it with highlighting.
            if (!append_and_highlight(highlighted_line_gstring,
                                      start_of_plain_text,
                                      current_token_start,
                                      token_color,
                                      token_len)) {
                return FALSE; // Memory allocation failed
            }
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

    // Append any remaining plain text at the end of the line
    if (start_of_plain_text != ptr) {
        char *plain_text =
            g_strndup(start_of_plain_text, ptr - start_of_plain_text);
        if (!plain_text)
            return FALSE; // Memory allocation failed
        char *escaped_plain_text = g_markup_escape_text(plain_text, -1);
        if (!escaped_plain_text) {
            g_free(plain_text);
            return FALSE;
        } // Memory allocation failed
        g_string_append(highlighted_line_gstring, escaped_plain_text);
        g_free(escaped_plain_text);
        g_free(plain_text);
    }
    return TRUE;
}

/**
 * @brief Main Python syntax highlighting logic.
 *        It iterates through the code line by line, prepends line numbers if
 * enabled, and then highlights tokens on each line.
 * @return A new string containing the code with Pango markup for highlighting,
 * or NULL on memory allocation failure.
 */
char *highlight_python_syntax(const char *code, gboolean show_line_numbers) {
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
    gchar in_multiline_string = 0; // State tracking for multi-line strings, 0
                                   // means no, '"' or '\'' means yes

    for (char **line_ptr = code_lines; *line_ptr != NULL; line_ptr++) {
        // Skip empty last line if it was a trailing newline
        if (show_line_numbers && current_line_num > max_line_number &&
            strlen(*line_ptr) == 0) {
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
        if (!highlight_tokens_on_line_python(
                current_line_gstring, *line_ptr, &in_multiline_string)) {
            g_string_free(current_line_gstring, TRUE);
            g_strfreev(code_lines);
            g_string_free(final_highlighted_code, TRUE);
            return NULL;
        }

        g_string_append(final_highlighted_code, current_line_gstring->str);
        g_string_free(
            current_line_gstring,
            TRUE); // Free the GString, but not its content as it's appended.

        // Only add newline if it's not the very last line and it's not an empty
        // trailing line
        if (!(*line_ptr == code_lines[max_line_number - 1] &&
              strlen(*line_ptr) == 0 && max_line_number > 0)) {
            g_string_append_c(final_highlighted_code,
                              '\n'); // Add newline back
        }

        current_line_num++;
    }

    g_strfreev(code_lines); // Free the array of strings

    return g_string_free(final_highlighted_code, FALSE);
}
