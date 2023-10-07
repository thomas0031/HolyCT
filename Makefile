# Compiler
CC = gcc

# Directories
SRC_DIR = collections global template scary
INCLUDE_DIR = collections global template scary

# Compiler flags (Add each include directory)
CFLAGS = $(foreach dir, $(INCLUDE_DIR), -I$(dir))

# Target executable
OUT_DIR = out
TARGET = $(OUT_DIR)/main

# Source files
SOURCES = $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.c)) main.c

# Default target
all: $(OUT_DIR) $(TARGET)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)
