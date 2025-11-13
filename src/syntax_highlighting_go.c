#include "syntax_highlighting.h"
#include <ctype.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern GHashTable *keywords_ht;
extern GHashTable *standard_functions_ht;

static gboolean append_and_highlight(GString *highlighted_code,
                                     const char *start_of_plain_text,
                                     const char *current_token_start,
                                     const char *color,
                                     size_t len) {
    if (start_of_plain_text < current_token_start) {
        char *plain_text = g_strndup(start_of_plain_text,
                                     current_token_start - start_of_plain_text);
        if (!plain_text)
            return FALSE;
        char *escaped_plain_text = g_markup_escape_text(plain_text, -1);
        if (!escaped_plain_text) {
            g_free(plain_text);
            return FALSE;
        }
        g_string_append(highlighted_code, escaped_plain_text);
        g_free(escaped_plain_text);
        g_free(plain_text);
    }

    char *token_text = g_strndup(current_token_start, len);
    if (!token_text)
        return FALSE;
    char *escaped_token = g_markup_escape_text(token_text, -1);
    if (!escaped_token) {
        g_free(token_text);
        return FALSE;
    }

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
    return TRUE;
}

void init_syntax_tables_go() {
    keywords_ht = g_hash_table_new(g_str_hash, g_str_equal);
    const char *keywords[] = {// Keywords
                              "break",
                              "case",
                              "chan",
                              "const",
                              "continue",
                              "default",
                              "defer",
                              "else",
                              "fallthrough",
                              "for",
                              "func",
                              "go",
                              "goto",
                              "if",
                              "import",
                              "interface",
                              "map",
                              "package",
                              "range",
                              "return",
                              "select",
                              "struct",
                              "switch",
                              "type",
                              "var",

                              // Built-in Types
                              "string",
                              "int",
                              "int8",
                              "int16",
                              "int32",
                              "int64",
                              "uint",
                              "uint8",
                              "uint16",
                              "uint32",
                              "uint64",
                              "uintptr",
                              "float32",
                              "float64",
                              "complex64",
                              "complex128",
                              "bool",
                              "byte",
                              "rune",
                              "error",
                              NULL};
    for (int i = 0; keywords[i] != NULL; i++) {
        g_hash_table_insert(
            keywords_ht, (gpointer)keywords[i], GINT_TO_POINTER(1));
    }

    standard_functions_ht = g_hash_table_new(g_str_hash, g_str_equal);
    const char *standard_functions[] = {
        // Built-in functions
        "append",
        "cap",
        "close",
        "complex",
        "copy",
        "delete",
        "imag",
        "len",
        "make",
        "new",
        "panic",
        "print",
        "println",
        "real",
        "recover",

        // fmt package
        "Errorf",
        "Fprint",
        "Fprintf",
        "Fprintln",
        "Fscan",
        "Fscanf",
        "Fscanln",
        "Print",
        "Printf",
        "Println",
        "Scan",
        "Scanf",
        "Scanln",
        "Sprint",
        "Sprintf",
        "Sprintln",
        "Sscan",
        "Sscanf",
        "Sscanln",

        // os package
        "Args",
        "Chdir",
        "Chmod",
        "Chown",
        "Chtimes",
        "Clearenv",
        "Create",
        "DevNull",
        "Environ",
        "Executable",
        "Exit",
        "Expand",
        "ExpandEnv",
        "FindProcess",
        "Getegid",
        "Getenv",
        "Geteuid",
        "Getgid",
        "Getgroups",
        "Getpagesize",
        "Getpid",
        "Getppid",
        "Getuid",
        "Getwd",
        "Hostname",
        "IsExist",
        "IsNotExist",
        "IsPathSeparator",
        "Lchown",
        "Link",
        "LookupEnv",
        "Mkdir",
        "MkdirAll",
        "NewFile",
        "NewSyscallError",
        "Open",
        "OpenFile",
        "Readlink",
        "Remove",
        "RemoveAll",
        "Rename",
        "SameFile",
        "Setenv",
        "StartProcess",
        "Symlink",
        "TempDir",
        "Truncate",
        "Unsetenv",

        // strings package
        "Compare",
        "Contains",
        "ContainsAny",
        "ContainsRune",
        "Count",
        "EqualFold",
        "Fields",
        "FieldsFunc",
        "HasPrefix",
        "HasSuffix",
        "Index",
        "IndexAny",
        "IndexByte",
        "IndexFunc",
        "IndexRune",
        "Join",
        "LastIndex",
        "LastIndexAny",
        "LastIndexByte",
        "LastIndexFunc",
        "Map",
        "Repeat",
        "Replace",
        "ReplaceAll",
        "Split",
        "SplitAfter",
        "SplitAfterN",
        "SplitN",
        "ToLower",
        "ToLowerSpecial",
        "ToTitle",
        "ToTitleSpecial",
        "ToUpper",
        "ToUpperSpecial",
        "Trim",
        "TrimFunc",
        "TrimLeft",
        "TrimLeftFunc",
        "TrimPrefix",
        "TrimRight",
        "TrimRightFunc",
        "TrimSpace",

        // strconv package
        "AppendBool",
        "AppendFloat",
        "AppendInt",
        "AppendQuote",
        "AppendQuoteRune",
        "AppendQuoteRuneToASCII",
        "AppendQuoteToASCII",
        "AppendUint",
        "Atoi",
        "CanBackquote",
        "FormatBool",
        "FormatFloat",
        "FormatInt",
        "FormatUint",
        "IsPrint",
        "Itoa",
        "ParseBool",
        "ParseFloat",
        "ParseInt",
        "ParseUint",
        "Quote",
        "QuoteRune",
        "QuoteRuneToASCII",
        "QuoteToASCII",
        "Unquote",
        "UnquoteChar",

        // encoding/json package
        "Marshal",
        "MarshalIndent",
        "NewDecoder",
        "NewEncoder",
        "Unmarshal",
        "Valid",

        // io/ioutil package (and equivalents in io, os)
        "ReadAll",
        "ReadFile",
        "WriteFile",
        "NopCloser",
        "ReadDir",

        NULL};
    for (int i = 0; standard_functions[i] != NULL; i++) {
        g_hash_table_insert(standard_functions_ht,
                            (gpointer)standard_functions[i],
                            GINT_TO_POINTER(1));
    }
}

void free_syntax_tables_go() {
    if (keywords_ht)
        g_hash_table_unref(keywords_ht);
    if (standard_functions_ht)
        g_hash_table_unref(standard_functions_ht);
    keywords_ht = NULL;
    standard_functions_ht = NULL;
}

static const char *go_sorted_operators[] = {
    "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=", "&^=", "&&",
    "||", "<-", "++", "--", "==", "!=", "<=", ">=", ":=",  "+",   "-",   "*",
    "/",  "%",  "&",  "|",  "^",  "<<", ">>", "&^", "!",   "<",   ">",   "=",
    "(",  ")",  "[",  "]",  "{",  "}",  ",",  ".",  ";",   ":",   NULL};

static gboolean highlight_tokens_on_line(GString *highlighted_line_gstring,
                                         const char *line_content,
                                         gboolean *in_multiline_comment,
                                         gboolean *next_word_is_func_name) {
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

        if (*ptr == '/' && *(ptr + 1) == '*') {
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
        } else if (*ptr == '"' || *ptr == '`') {
            char quote_char = *ptr;
            const char *scan_ptr = ptr + 1;
            while (*scan_ptr != '\0' && *scan_ptr != '\n') {
                if (quote_char != '`' && *scan_ptr == '\\' &&
                    *(scan_ptr + 1) != '\0') {
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
            if (*num_scan_ptr != '\0' &&
                (g_ascii_isalnum(*num_scan_ptr) || *num_scan_ptr == '_')) {
                token_color = NULL;
            } else {
                token_color = "#ff9e64";
            }
        } else if (g_ascii_isalpha(*ptr) || *ptr == '_') {
            const char *word_scan_ptr = ptr;
            while (g_ascii_isalnum(*word_scan_ptr) || *word_scan_ptr == '_')
                word_scan_ptr++;
            char *word = g_strndup(ptr, word_scan_ptr - ptr);
            if (!word)
                return FALSE;

            gboolean is_word_boundary =
                (*word_scan_ptr == '\0' ||
                 (!g_ascii_isalnum(*word_scan_ptr) && *word_scan_ptr != '_'));

            if (is_word_boundary) {
                if (*next_word_is_func_name) {
                    token_color = NULL; // It's a function name after 'func',
                                        // don't color it.
                    *next_word_is_func_name = FALSE;
                } else if (strcmp(word, "import") == 0) {
                    token_color = "#7aa2f7";
                } else if (strcmp(word, "func") == 0) {
                    token_color = "#f7768e";
                    *next_word_is_func_name = TRUE;
                } else if (g_hash_table_lookup(keywords_ht, word)) {
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
        } else {
            for (int i = 0; go_sorted_operators[i] != NULL; i++) {
                size_t op_len = strlen(go_sorted_operators[i]);
                if (strncmp(ptr, go_sorted_operators[i], op_len) == 0) {
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
            if (!append_and_highlight(highlighted_line_gstring,
                                      start_of_plain_text,
                                      current_token_start,
                                      NULL,
                                      1))
                return FALSE;
            ptr++;
            start_of_plain_text = ptr;
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

char *highlight_go_syntax(const char *code, gboolean show_line_numbers) {
    GString *final_highlighted_code = g_string_new("");
    if (!final_highlighted_code)
        return NULL;

    char **code_lines = g_strsplit(code, "\n", -1);
    if (!code_lines) {
        g_string_free(final_highlighted_code, TRUE);
        return NULL;
    }

    int max_line_number = 0;
    for (char **line_ptr = code_lines; *line_ptr != NULL; line_ptr++) {
        max_line_number++;
    }

    if (max_line_number > 0 && strlen(code_lines[max_line_number - 1]) == 0) {
        max_line_number--;
    }

    int line_number_width = 0;
    if (show_line_numbers) {
        char temp_buf[12];
        snprintf(temp_buf, sizeof(temp_buf), "%d", max_line_number);
        line_number_width = strlen(temp_buf);
    }

    int current_line_num = 1;
    gboolean in_multiline_comment = FALSE;
    gboolean next_word_is_func_name = FALSE;

    for (char **line_ptr = code_lines; *line_ptr != NULL; line_ptr++) {
        if (current_line_num > max_line_number && strlen(*line_ptr) == 0) {
            continue;
        }

        GString *current_line_gstring = g_string_new("");
        if (!current_line_gstring) {
            g_strfreev(code_lines);
            g_string_free(final_highlighted_code, TRUE);
            return NULL;
        }

        if (show_line_numbers) {
            g_string_append_printf(current_line_gstring,
                                   "<span foreground='#545c7e'>%*d </span>",
                                   line_number_width,
                                   current_line_num);
        }

        if (!highlight_tokens_on_line(current_line_gstring,
                                      *line_ptr,
                                      &in_multiline_comment,
                                      &next_word_is_func_name)) {
            g_string_free(current_line_gstring, TRUE);
            g_strfreev(code_lines);
            g_string_free(final_highlighted_code, TRUE);
            return NULL;
        }

        g_string_append(final_highlighted_code, current_line_gstring->str);
        g_string_free(current_line_gstring, TRUE);

        if (line_ptr[1] != NULL) {
            g_string_append_c(final_highlighted_code, '\n');
        }

        current_line_num++;
    }

    g_strfreev(code_lines);

    return g_string_free(final_highlighted_code, FALSE);
}
