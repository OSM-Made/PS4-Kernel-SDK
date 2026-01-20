SDK_ROOT = ..
CC = gcc
AR = ar

# Directories
SRC_DIR = source
INC_DIR = include
OBJ_DIR = build
LIB_DIR = lib

# Include paths
INCLUDES = -I$(INC_DIR) -I$(SDK_ROOT)/include

# Compiler flags for userland (position-independent code)
CFLAGS = $(INCLUDES) -O2 -std=c11 -fPIC -fno-stack-protector \
         -Wall -Wno-unused-function

# Automatically find all .c files in source directory
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Generate corresponding .o file names in build directory
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Output library
LIB = $(LIB_DIR)/libkloader.a

.PHONY: all clean install info

all: $(LIB)

# Build library
$(LIB): $(OBJS)
	@mkdir -p $(LIB_DIR)
	@echo "Creating library: $@"
	@$(AR) rcs $@ $^

# Pattern rule to compile .c files to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Install headers to SDK
install: all
	@echo "Installing headers to SDK..."
	@mkdir -p $(SDK_ROOT)/include/loader
	@cp -f $(INC_DIR)/*.h $(SDK_ROOT)/include/loader/
	@echo "Installing library to SDK..."
	@mkdir -p $(SDK_ROOT)/lib
	@cp -f $(LIB) $(SDK_ROOT)/lib/
	@echo "Installation complete"

clean:
	@rm -rf $(OBJ_DIR) $(LIB_DIR)

# Show build info
info:
	@echo "PS4 Kernel Loader Build System"
	@echo "=============================="
	@echo "Source files: $(SRCS)"
	@echo "Object files: $(OBJS)"
	@echo "Library: $(LIB)"
	@echo ""
	@echo "Targets:"
	@echo "  make all     - Build library"
	@echo "  make install - Install to SDK"
	@echo "  make clean   - Remove build artifacts"
	@echo "  make info    - Show this information"

.PHONY: all clean install info