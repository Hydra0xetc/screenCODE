CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_USE_MATH_DEFINES -Os -flto -ffunction-sections -fdata-sections
LDFLAGS = $(shell pkg-config --libs cairo pango pangocairo glib-2.0 fontconfig) -lglib-2.0 -flto -Wl,--gc-sections

# Add include path for pkg-config and our src dir
CPPFLAGS = $(shell pkg-config --cflags cairo pango pangocairo glib-2.0) -Isrc

# Source directory
SRC_DIR = src

# Find all .c files in the source directory, excluding test files
SRCS = $(filter-out $(SRC_DIR)/test_c_code.c, $(wildcard $(SRC_DIR)/*.c))

# Create object file names in the root directory
OBJS = $(patsubst $(SRC_DIR)/%.c,%.o,$(SRCS))

TARGET = screenCODE

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule to compile .c files from the src directory into .o files in the root
%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.d *.gch
