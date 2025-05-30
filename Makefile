# Makefile for Compiler Project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Target executable
TARGET = code.exe

# Bison and Flex files
BISON_SRC = parser.y
FLEX_SRC = lex.l

# Generated files
BISON_C = parser.tab.c
BISON_H = parser.tab.h
FLEX_C = lex.yy.c

# Object files
OBJS = $(BISON_C:.c=.o) $(FLEX_C:.c=.o)

# Default target
all: $(TARGET)

# Build the compiler
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Generate C code from Bison grammar
$(BISON_C) $(BISON_H): $(BISON_SRC)
	bison -d $(BISON_SRC)

# Generate C code from Flex lexer
$(FLEX_C): $(FLEX_SRC) $(BISON_H)
	flex -o $(FLEX_C) $(FLEX_SRC)

# Compile the generated C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(TARGET) $(OBJS) $(BISON_C) $(BISON_H) $(FLEX_C)

# Run the compiler with input redirected from input.txt
run: $(TARGET)
	./$(TARGET) < input.txt

# Clean and rebuild
rebuild: clean all

.PHONY: all clean run rebuild