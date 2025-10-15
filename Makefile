# DMOD VFS Makefile
# This Makefile builds the dmod-vfs library as a static library

# Project configuration
PROJECT_NAME = dmod-vfs
VERSION = 1.0.0

# Directories
SRC_DIR = src
INC_DIR = inc
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
LIB_DIR = $(BUILD_DIR)/lib

# DMOD configuration
# Set DMOD_DIR environment variable to point to your DMOD installation
DMOD_DIR ?= ../dmod

# Compiler configuration
CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -O2 -std=c11
CFLAGS += -I$(INC_DIR)
CFLAGS += -I$(DMOD_DIR)/inc
CFLAGS += -I$(DMOD_DIR)/build

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Target library
TARGET = $(LIB_DIR)/lib$(PROJECT_NAME).a

# Default target
all: $(TARGET)

# Create directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

# Build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build static library
$(TARGET): $(OBJS) | $(LIB_DIR)
	$(AR) rcs $@ $(OBJS)
	@echo "Built: $@"

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Install (optional)
install: $(TARGET)
	@echo "Installing $(PROJECT_NAME) to /usr/local"
	@mkdir -p /usr/local/include/$(PROJECT_NAME)
	@mkdir -p /usr/local/lib
	@cp $(INC_DIR)/*.h /usr/local/include/$(PROJECT_NAME)/
	@cp $(TARGET) /usr/local/lib/
	@echo "Installation complete"

# Uninstall (optional)
uninstall:
	@echo "Uninstalling $(PROJECT_NAME)"
	@rm -rf /usr/local/include/$(PROJECT_NAME)
	@rm -f /usr/local/lib/lib$(PROJECT_NAME).a
	@echo "Uninstall complete"

# Help
help:
	@echo "DMOD VFS Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build the library (default)"
	@echo "  clean     - Remove build artifacts"
	@echo "  install   - Install library to /usr/local"
	@echo "  uninstall - Remove installed library"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Configuration:"
	@echo "  DMOD_DIR  - Path to DMOD installation (default: ../dmod)"
	@echo ""
	@echo "Example:"
	@echo "  make DMOD_DIR=/path/to/dmod"

.PHONY: all clean install uninstall help
