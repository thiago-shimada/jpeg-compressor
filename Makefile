# Makefile for JPEG Compressor/Decompressor

# Compiler settings
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g -I./include
LDFLAGS = -lm  # Math library should be a linker flag

# Directories
SRC_DIR = src
INCLUDE_DIR = include
EXAMPLES_DIR = examples
BIN_DIR = bin
OBJ_DIR = obj

# Create directories if they don't exist
$(shell mkdir -p $(BIN_DIR) $(OBJ_DIR))

# Source files (excluding main.c)
SRC_FILES = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Example source files
EXAMPLE_SOURCES = $(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLE_BINS = $(patsubst $(EXAMPLES_DIR)/%.c, $(BIN_DIR)/%, $(EXAMPLE_SOURCES))

# Future executables
COMPRESSOR = $(BIN_DIR)/compressor
DECOMPRESSOR = $(BIN_DIR)/decompressor

# Default target
all: library examples

# Library target (builds the core functionality)
library: $(OBJ_FILES)

# Examples target
examples: $(EXAMPLE_BINS)

# Rule to build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build example executables
$(BIN_DIR)/%: $(EXAMPLES_DIR)/%.c $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Rule for the compressor
$(COMPRESSOR): $(SRC_DIR)/compressor.c $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Rule for the decompressor
$(DECOMPRESSOR): $(SRC_DIR)/decompressor.c $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build both compressor and decompressor
tools: $(COMPRESSOR) $(DECOMPRESSOR)

# Clean target
clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/*

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build library and examples (default)"
	@echo "  library    - Build only the core library objects"
	@echo "  examples   - Build the example applications"
	@echo "  tools      - Build the compressor and decompressor tools"
	@echo "  clean      - Remove all built files"
	@echo "  help       - Display this help message"

.PHONY: all library examples tools clean help