CC = gcc
CFLAGS = -Isrc/include `pkg-config --cflags gtk+-3.0 gtk-layer-shell-0 cairo gio-2.0`
LDFLAGS = `pkg-config --libs gtk+-3.0 gtk-layer-shell-0 cairo gio-2.0`
SOURCES = src/main.c src/config.c src/logging.c src/window.c src/drawing.c src/lockfile.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = roundframe

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
