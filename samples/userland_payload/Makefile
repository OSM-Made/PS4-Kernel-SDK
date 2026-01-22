SDK = ../..

# Use loader.mk for common definitions
include $(SDK)/loader/loader.mk

# Kernel module to embed
KERNEL_MODULE     = hello_world.elf
KERNEL_MODULE_DIR = ../hello_world

# Source files
CFILES := $(wildcard $(SDIR)/*.c)

# Find all resource files
RFILES := $(shell find $(RDIR) -type f 2>/dev/null)

# Object files (C sources + embedded resources)
OBJS := $(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(CFILES)) \
        $(patsubst $(RDIR)/%, $(ODIR)/resources_%.o, $(RFILES))
		
LFLAGS := -Xlinker -T Linker -Wl,--build-id=none -Wl,--gc-sections

# Targets
TARGET = payload.bin
ELF_TARGET = payload.elf
TARGET_PATH := $(OUTPUT_DIR)/$(TARGET)
ELF_TARGET_PATH := $(OUTPUT_DIR)/$(ELF_TARGET)

.PHONY: all clean rebuild prepare_resources

all: $(TARGET_PATH)

# Prepare resources: build kernel module and copy to resources
prepare_resources:
	@mkdir -p $(RDIR)
	@echo "Building hello_world kernel module..."
	@$(MAKE) -C $(KERNEL_MODULE_DIR) OUTPUT_DIR=$(shell realpath $(OUTPUT_DIR))
	@echo "Copying kernel module to resources..."
	@cp $(OUTPUT_DIR)/$(KERNEL_MODULE) $(RDIR)/$(KERNEL_MODULE)

$(TARGET_PATH): $(ELF_TARGET_PATH)
	@echo "Converting to binary..."
	@$(OBJCOPY) -O binary $(ELF_TARGET_PATH) $(TARGET_PATH)
	@echo "Payload built: $(TARGET_PATH) (size: $$(stat -c%s $(TARGET_PATH) 2>/dev/null || stat -f%z $(TARGET_PATH)) bytes)"

$(ELF_TARGET_PATH): prepare_resources $(ODIR) $(OBJS)
	@mkdir -p $(OUTPUT_DIR)
	@echo "Linking payload ELF..."
	@$(CC) crt0.s $(OBJS) -o $(ELF_TARGET_PATH) $(CFLAGS) $(LFLAGS) $(LIBS)

# Compile C sources
$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling: $<"
	@$(CC) -c -o $@ $< $(CFLAGS)

# Embed resource files as binary data
$(ODIR)/resources_%.o: $(RDIR)/%
	@mkdir -p $(dir $@)
	@echo "Embedding resource: $<"
	@$(LD) -r -b binary -o "$@" "$<"
	@$(OBJCOPY) --rename-section .data=.rodata,alloc,load,readonly,data,contents \
	            --set-section-alignment .rodata=4096 "$@"
	@$(OBJCOPY) --add-section .note.GNU-stack=/dev/null \
	            --set-section-flags .note.GNU-stack=noload,readonly "$@"

$(ODIR):
	@mkdir -p $@

rebuild: clean all

clean:
	@echo "Cleaning project..."
	@rm -f $(TARGET_PATH) $(ELF_TARGET_PATH)
	@rm -rf $(ODIR)

.PHONY: all clean rebuild prepare_resources