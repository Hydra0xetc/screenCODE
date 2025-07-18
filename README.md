# screenCODE

This is a command-line tool written in C that generates a screenshot of source code files with syntax highlighting. It supports both C and Python syntax highlighting and allows for customization of the window header.

**Important Note:** This is the first version of screenCODE. It may contain bugs, and its current functionality is limited to C and Python syntax highlighting, as these are the primary languages used by me.

## Features

- Syntax highlighting for C and Python.
- Customizable window header (with or without gradient).
- Generates PNG image output.

## Dependencies

This project requires the following libraries:

- **Cairo**: A 2D graphics library.
- **Pango**: A library for internationalized text layout and rendering.
- **Pangocairo**: Pango integration with Cairo.
- **GLib 2.0**: A general-purpose utility library.

These can typically be installed via your system's package manager. Here are some examples:

- **Termux (Android)**:
  ```bash
  pkg install cairo pango glib
  ```

- **Arch Linux**:
  ```bash
  sudo pacman -S cairo pango glib2
  ```

- **Debian/Ubuntu**:
  ```bash
  sudo apt install libcairo2-dev libpango1.0-dev libpangocairo-1.0-0 libglib2.0-dev
  ```

(Ensure you also have `gcc` and `make` installed on your system.)

## Build Instructions

To build the project, navigate to the `screenshot` directory and run `make`:

```bash
cd ~/screenCODE
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
./screenCODE -lang c /data/data/com.termux/files/home/test_c_code.c /data/data/com.termux/files/home/c_code_screenshot.png
```

**Screenshot a Python file without gradient:**
```bash
./screenCODE -lang python -no-gradient /data/data/com.termux/files/home/test_python_code.py /data/data/com.termux/files/home/python_code_screenshot.png
```

## Cleaning the Project

To remove compiled object files and the executable, run:

```bash
cd ~/screenCODE
make clean
```
