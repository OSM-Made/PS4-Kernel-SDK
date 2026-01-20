# Get the directory of this makefile (loader/)
LOADER_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

# SDK root is one level up from loader/
SDK_ROOT := $(LOADER_DIR)/..

CC        := gcc
OBJCOPY   := objcopy
STRIP     := strip

# Directories
ODIR      := build
SDIR      := source

# Loader library (use absolute path)
KLOADER_LIB := $(LOADER_DIR)/lib/libkloader.a

# Include paths
IDIRS := -I$(SDK_ROOT)/include \
         -I$(LOADER_DIR)/include \
         -Iinclude

# Compiler flags for userland payload
CFLAGS := $(IDIRS) -O2 -std=gnu11 \
          -ffunction-sections -fdata-sections \
          -fno-builtin -nostartfiles -nostdlib \
          -Wall -masm=intel -march=btver2 -mtune=btver2 \
          -m64 -mabi=sysv -mcmodel=small -fpie

# Linker flags
LFLAGS := -Xlinker -T Linker \
          -Wl,--build-id=none \
          -Wl,--gc-sections

# Libraries
LIBS := $(KLOADER_LIB)
