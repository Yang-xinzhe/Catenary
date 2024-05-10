# Compiler and flags
# Compiler and flags
CC = gcc
CFLAGS = -g -Wall
LIBS = -lm -lpthread

# Cross-compiler setup
CROSS_COMPILE = arm-linux-gnueabihf-gcc
CROSS_FLAGS = -g -Wall

# Target executable name
TARGET = app

# Source files and object files
SRC = $(wildcard *.c)
OBJS = $(SRC:.c=.o)
CROSS_OBJS = $(SRC:.c=_arm.o)

# Default target
all: $(TARGET)

# Building the main target
$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LIBS)

# Building object files from C source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Cross-compilation target
arm: $(CROSS_OBJS)
	$(CROSS_COMPILE) $^ -o $(TARGET) $(LIBS)

# Building cross-compiled object files
%_arm.o: %.c
	$(CROSS_COMPILE) $(CROSS_FLAGS) -c $< -o $@

# Clean up
clean:
	rm -f *.o $(TARGET) $(TARGET)_arm

# Phony targets
.PHONY: clean all arm
