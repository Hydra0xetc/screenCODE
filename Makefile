CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_USE_MATH_DEFINES

PKG_CFLAGS = $(shell pkg-config --cflags cairo pango pangocairo glib-2.0)
PKG_LIBS = $(shell pkg-config --libs cairo pango pangocairo glib-2.0)

SRCS = main.c syntax_highlighting.c drawing_utils.c
OBJS = $(SRCS:.c=.o)

TARGET = screenCODE

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(PKG_LIBS)

%.o: %.c screenshot.h
	$(CC) $(CFLAGS) $(PKG_CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
