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
  pkg install clang make pkg-config libcairo pango glib
  ```

- **Arch Linux**:
  ```bash
  sudo pacman -S base-devel pkgconf cairo pango glib2
  ```

- **Debian/Ubuntu**:
  ```bash
  sudo apt install build-essential pkg-config libcairo2-dev libpango1.0-dev libpangocairo-1.0-0 libglib2.0-dev
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

## Cleaning the Project

To remove compiled object files and the executable, run:

```bash
cd ~/screenCODE
make clean
```
