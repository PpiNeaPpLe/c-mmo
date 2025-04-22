# Basic Makefile for Simple RPG

# Compiler
CC = gcc

# Compiler flags
# -Wall: Enable all warnings
# -Wextra: Enable extra warnings
# -g: Add debug information
CFLAGS = -Wall -Wextra -g

# Executable name
TARGET = rpg_game

# Source files (add more as needed)
# SRCS = main.c # player.c enemy.c game.c
SRCS = main.c player.c enemy.c game.c

# Object files (derived from source files)
OBJS = $(SRCS:.c=.o)

# Default target: build the executable
all: $(TARGET)

# Rule to link the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile source files into object files
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

# Added header dependencies to the compile rule
# This tells make to recompile a .c file if its corresponding .h file changes
# A more robust way involves auto-dependency generation, but this is simpler

# Clean target: remove executable and object files
clean:
	rm -f $(TARGET) $(OBJS)

# Phony targets (targets that don't represent files)
.PHONY: all clean 