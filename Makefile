CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags gtk+-3.0 gtk-layer-shell-0`
LDFLAGS = `pkg-config --libs gtk+-3.0 gtk-layer-shell-0`
SRC_DIR = src
OBJ = $(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))
TARGET = roundframe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
