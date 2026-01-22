SDK_ROOT ?= $(dir $(lastword $(MAKEFILE_LIST)))
CC        := g++
OBJCOPY   := objcopy
STRIP     := strip
ODIR      := build
SDIR      := source
IDIR	  := include
SDK_INCLUDE := $(SDK_ROOT)/include
SDK_LIB     := $(SDK_ROOT)/lib
SDK_LD      := $(SDK_ROOT)/ld
IDIRS     := $(shell find $(SDK_INCLUDE) -type d -printf '-I%p ') $(shell find $(IDIR) -type d -printf '-I%p ')
CFLAGS    := $(IDIRS) -O2 -s -w -std=gnu++11 -ffunction-sections -fdata-sections -fno-builtin -fno-exceptions -fno-asynchronous-unwind-tables -nostdlib -w -masm=intel -march=btver2 -m64 -mabi=sysv -mcmodel=small -mstackrealign -D_KERNEL
CFLAGS_NOSTRIP := $(filter-out -s,$(CFLAGS))
LFLAGS    := -Xlinker -T$(SDK_LD)/module.ld -Wl,--build-id=none -mstackrealign -Wl,--gc-sections -nostdlib
CRTBEGIN  := $(SDK_LIB)/crt0.o $(SDK_LIB)/crt1.o

# Output directory for final artifacts (overrideable)
OUTPUT_DIR ?= .

ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG
endif

CFILES    := $(shell find $(SDIR) -name \*.cpp 2>/dev/null)
OBJS      := $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(CFILES))
TARGET    ?= $(shell basename "$(CURDIR)").elf
TARGET_PATH := $(OUTPUT_DIR)/$(TARGET)
MODULE_NAME    ?= $(basename $(TARGET))
MODULE_VERSION ?= 1.0.0
MODULE_AUTHOR  ?= Unknown
MODULE_DESC    ?= No description provided
MODNAME_OBJ := $(ODIR)/.modinfo.o

# Detect number of CPU cores for parallel compilation
NPROCS := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1)
MAKEFLAGS += -j$(NPROCS)

$(TARGET_PATH): $(PCH_OUTPUT) $(ODIR) $(OBJS) $(MODNAME_OBJ)
	@mkdir -p $(OUTPUT_DIR)
	$(CC) $(CRTBEGIN) $(MODNAME_OBJ) $(OBJS) -o $(TARGET_PATH) $(LFLAGS)

$(ODIR)/%.o: $(SDIR)/%.cpp $(PCH_OUTPUT)
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS) $(PCH_FLAG)

# Generate module metadata object
$(MODNAME_OBJ): | $(ODIR)
	@echo '/* Auto-generated module metadata */' > $(ODIR)/.modinfo.c
	@echo '__attribute__((section(".modinfo"), used))' >> $(ODIR)/.modinfo.c
	@echo 'const char __module_name[] = "$(MODULE_NAME)";' >> $(ODIR)/.modinfo.c
	@echo '__attribute__((section(".modinfo"), used))' >> $(ODIR)/.modinfo.c
	@echo 'const char __module_version[] = "$(MODULE_VERSION)";' >> $(ODIR)/.modinfo.c
	@echo '__attribute__((section(".modinfo"), used))' >> $(ODIR)/.modinfo.c
	@echo 'const char __module_author[] = "$(MODULE_AUTHOR)";' >> $(ODIR)/.modinfo.c
	@echo '__attribute__((section(".modinfo"), used))' >> $(ODIR)/.modinfo.c
	@echo 'const char __module_desc[] = "$(MODULE_DESC)";' >> $(ODIR)/.modinfo.c
	gcc $(CFLAGS_NOSTRIP) -c $(ODIR)/.modinfo.c -o $@
	@rm $(ODIR)/.modinfo.c

$(ODIR):
	@mkdir -p $@

symbols: $(TARGET_PATH)
	@$(OBJCOPY) --only-keep-debug $(TARGET_PATH) $(TARGET_PATH).sym
	@$(STRIP) --strip-all $(TARGET_PATH)
	@$(OBJCOPY) --add-gnu-debuglink=$(TARGET_PATH).sym $(TARGET_PATH)

clean:
	rm -rf $(TARGET_PATH) $(TARGET_PATH).sym $(ODIR)

.PHONY: symbols clean