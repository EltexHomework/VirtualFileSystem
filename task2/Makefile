CC := gcc
HEADERS_DIR := headers
CFLAGS := -O2 -I$(HEADERS_DIR)

LDFLAGS := -lmenu -lncurses

SRC_DIR := src
BIN_DIR := bin
TARGET := $(BIN_DIR)/main

# Source files and object file paths
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SOURCES))

all: $(BIN_DIR) $(TARGET)

# Create bin directory
$(BIN_DIR):
	@mkdir -p $@

# Link object files to create the executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# Compile source files to object files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean bin folder
clean:
	@rm -rf $(BIN_DIR)

.PHONY: all clean

