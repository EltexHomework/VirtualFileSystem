CC := gcc
CFLAGS := -Wall -Werror -O2

SRC_DIR := src
BIN_DIR := bin

# Source files and object file paths
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%, $(SOURCES))

all: $(BIN_DIR) $(OBJECTS)

# Create bin directory
$(BIN_DIR):
	@mkdir -p $@

# Compile src files to binary
$(BIN_DIR)/%: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) $< -o $@

# Clean bin folder 
clean:
	@rm -rf $(BIN_DIR)

.PHONY: all clean

