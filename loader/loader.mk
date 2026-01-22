SDK_ROOT ?= $(dir $(lastword $(MAKEFILE_LIST)))
LOADER_DIR := $(SDK_ROOT)

CC       := gcc
OBJCOPY  := objcopy
LD       := ld
ODIR     := build
SDIR     := source
IDIR     := include
RDIR     := resources

SDK_INCLUDE := $(SDK_ROOT)/../include
SDK_LIB     := $(SDK_ROOT)/../lib
SDK_LD      := $(SDK_ROOT)/../ld
LOADER_INCLUDE := $(LOADER_DIR)/include
LOADER_LIB := $(LOADER_DIR)/lib

# Loader library (use absolute path)
KLOADER_LIB := $(LOADER_LIB)/libkloader.a

IDIRS := -I$(SDK_INCLUDE) -I$(LOADER_INCLUDE) -I$(IDIR)

CFLAGS := $(IDIRS) -O2 -std=gnu11 -ffunction-sections -fdata-sections -fno-builtin \
          -nostartfiles -nostdlib -Wall -masm=intel -march=btver2 -mtune=btver2 \
          -m64 -mabi=sysv -mcmodel=small -fpie

# Libraries
LIBS := $(KLOADER_LIB)

# Output directory for final artifacts (overrideable)
OUTPUT_DIR ?= .

# Detect number of CPU cores for parallel compilation
NPROCS := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1)
MAKEFLAGS += -j$(NPROCS)

# Export variables
export CC OBJCOPY LD CFLAGS LFLAGS LIBS OUTPUT_DIR