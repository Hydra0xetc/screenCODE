# screenCODE

This is a command-line tool written in C that generates a screenshot of source code files with syntax highlighting. It supports both C and Python syntax highlighting and allows for customization of the window header.

**Important Note:** This is the first version of screenCODE. It may contain bugs, and its current functionality is limited to C and Python syntax highlighting, as these are the primary languages used by me.

## Features

- Syntax highlighting for C and Python.
- Customizable window header (with or without gradient).
- Generates PNG image output.

## Dependencies

To build this project, you first need to install essential build tools like `gcc`, `make`, and `pkg-config`. After that, you'll need the development libraries for Cairo, Pango, and GLib 2.0.

Here are some examples of how to install these dependencies on various systems:

- **Termux (Android)**:
  ```bash
  pkg install git clang make pkg-config libcairo pango glib
  ```

- **Arch Linux**:
  ```bash
  sudo pacman -S git base-devel pkgconf cairo pango glib2
  ```

- **Debian/Ubuntu**:
  ```bash
  sudo apt install git build-essential pkg-config libcairo2-dev libpango1.0-dev libpangocairo-1.0-0 libglib2.0-dev
  ```

## Build Instructions

To build the project, first clone the repository and then navigate into its directory:

```bash
git clone https://github.com/Hydra0xetc/screenCODE.git
cd screenCODE
make
```

This will compile the source code and create an executable named `screenCODE` in the same directory.

## Usage

```bash
./screenCODE [OPTIONS] <input_file> <output_png>
```

### Options:

- `-lang c|python`: Specify the programming language for syntax highlighting (default: `c`).
- `-no-gradient`: Disable the gradient effect on the window header.

### Arguments:

- `<input_file>`: Path to the source code file to be screenshotted.
- `<output_png>`: Path where the output PNG image will be saved.

### Examples:

**Screenshot a C file with default gradient:**
```bash
./screenCODE -lang c test_c_code.c c_code_screenshot.png
```
![C Code Screenshot](images/c_code_screenshot.png)

**Screenshot a Python file without gradient:**
```bash
./screenCODE -lang python -no-gradient test_python_code.py python_code_screenshot.png
```
![Python Code Screenshot](images/python_code_screenshot.png)

<details>
<summary>## How it Works 👇</summary>

The `screenCODE` program is designed to take a source code file (C or Python), apply syntax highlighting, and then generate a PNG image of the highlighted code, resembling a code editor screenshot.

Here's how it works in detail:

1.  **Parsing Command Line Arguments (`main.c`)**:
    *   The program starts by processing the arguments you provide on the command line.
    *   `input_file` and `output_png` are mandatory arguments specifying the source code file location and the output PNG file name.
    *   The optional `-lang c|python` argument is used to specify the programming language (defaults to C). This is crucial for the program to know which syntax highlighting rules to apply.
    *   The optional `-no-gradient` argument is used to disable the gradient effect on the window header in the output image.

2.  **Reading Code File Content (`main.c`)**:
    *   After arguments are processed, the program reads the entire content of the specified source code file into memory as a string.

3.  **Syntax Highlighting (`syntax_highlighting.c`, `syntax_highlighting_c.c`, `syntax_highlighting_python.c`)**:
    *   This is the core of the program. The `highlight_syntax(const char* code, LanguageType lang)` function in `syntax_highlighting.c` is the main entry point.
    *   Based on the detected `LanguageType` (C or Python), this function will call the appropriate syntax table initialization function (`init_syntax_tables_c()` or `init_syntax_tables_python()`) and the specific syntax highlighting function (`highlight_c_syntax()` or `highlight_python_syntax()`).
    *   **Syntax Table Initialization**: The `init_syntax_tables_c()` and `init_syntax_tables_python()` functions (located in their respective separate `.c` files) populate three global hash tables (`keywords_ht`, `preprocessor_directives_ht`, `standard_functions_ht`). These tables contain lists of keywords, preprocessor directives (for C), and standard functions for each language. The use of hash tables allows for very fast lookups as the program scans the code.
    *   **Highlighting Logic**: The `highlight_c_syntax()` and `highlight_python_syntax()` functions (also in separate `.c` files) perform a character-by-character scan of the source code string.
        *   They identify various "tokens" such as string literals (e.g., `"hello"`), character literals (e.g., `'a'`), comments (block and line), numbers, keywords, preprocessor directives (for C), standard functions, and operators.
        *   For each recognized token, they determine the appropriate color (e.g., green for strings, gray for comments, red for keywords, etc.).
        *   The `append_and_highlight()` function is a helper used to build the output string. It takes plain unhighlighted text and highlighted tokens, escapes HTML special characters (like `<`, `>`, `&`), and wraps the highlighted tokens with `<span foreground='color'>token</span>` tags. This results in a string formatted with Pango markup, which can be interpreted by Cairo/Pango to display colored text.
    *   **Syntax Table Cleanup**: After highlighting is complete, the `free_syntax_tables_c()` or `free_syntax_tables_python()` functions are called to free the memory used by the hash tables. This is crucial to prevent memory leaks, especially if the program is called multiple times.

4.  **Text Measurement and Image Dimensions (`main.c`)**:
    *   Before drawing, the program uses Cairo and Pango to calculate the dimensions (width and height) of the highlighted code text. This is done on a temporary Cairo surface.
    *   These dimensions are then used to determine the total size of the output PNG image, including padding and the window header height.

5.  **PNG Image Drawing (`main.c`, `drawing_utils.c`)**:
    *   The program creates a new Cairo image surface with the calculated dimensions.
    *   **Background**: Draws a background with a color gradient.
    *   **Window Shadow**: Draws a shadow effect behind the code window.
    *   **Window Frame**: Draws the main window frame with rounded corners (using the `draw_rounded_rectangle` function from `drawing_utils.c`).
    *   **Window Header**: Draws the window header section. This can be a solid color or a gradient, depending on the `-no-gradient` argument.
    *   **Window Buttons**: Draws three colored circles (red, yellow, green) resembling window control buttons.
    *   **Drawing Code**: Finally, the Pango-markup-formatted code text is drawn onto the image surface at the correct position below the header.

6.  **Image Saving (`main.c`)**:
    *   Once all elements are drawn, the program saves the Cairo surface as a PNG file to the location specified by `output_png`.

7.  **Memory Cleanup (`main.c`)**:
    *   The program frees all memory allocated during the process, including the code file content, the highlighted code string, and Cairo/Pango resources.

In summary, this program is a combination of a simple syntax parser that uses hash tables for speed, and a graphical rendering engine (Cairo/Pango) to transform formatted text into an aesthetically pleasing visual image.
</details>