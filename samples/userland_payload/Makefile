SDK = ../..
RDIR := resources

# Use loader.mk for common definitions
include $(SDK)/loader/loader.mk

# Kernel module to embed
KERNEL_MODULE = ../hello_world/hello_world.elf

# Source files
CFILES := $(wildcard $(SDIR)/*.c)

# Object files (C sources + embedded kernel module)
OBJS := $(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(CFILES)) \
        $(ODIR)/kernel_module_embedded.o

# Targets
TARGET = payload.bin
ELF_TARGET = payload.elf

.PHONY: all clean rebuild

all: $(TARGET)

# Build kernel module first if needed
$(KERNEL_MODULE):
	@echo "Building hello_world kernel module..."
	@$(MAKE) -C ../hello_world

$(TARGET): $(ELF_TARGET)
	@echo "Converting to binary..."
	@$(OBJCOPY) -O binary $(ELF_TARGET) $(TARGET)
	@echo "Payload built: $(TARGET) (size: $$(stat -c%s $(TARGET)) bytes)"

$(ELF_TARGET): $(ODIR) $(OBJS)
	@echo "Linking payload ELF..."
	@$(CC) crt0.s $(OBJS) -o $(ELF_TARGET) $(CFLAGS) $(LFLAGS) $(LIBS)

# Compile C sources
$(ODIR)/%.o: $(SDIR)/%.c
	@echo "Compiling: $<"
	@$(CC) -c -o $@ $< $(CFLAGS)

# Embed kernel module as binary data
$(ODIR)/kernel_module_embedded.o: $(KERNEL_MODULE)
	@mkdir -p $(RDIR)
	@echo "Embedding kernel module: $(KERNEL_MODULE)"
	@cp $(KERNEL_MODULE) $(RDIR)/hello_world.elf
	@cd $(RDIR)
	@ld -r -b binary -o "$@" "$(RDIR)/hello_world.elf"
	@$(OBJCOPY) --rename-section .data=.rodata,alloc,load,readonly,data,contents \
	            --set-section-alignment .rodata=4096 "$@"
	@$(OBJCOPY) --add-section .note.GNU-stack=/dev/null \
	            --set-section-flags .note.GNU-stack=noload,readonly "$@"
	@rm -f $(RDIR)/hello_world.elf

$(ODIR):
	@mkdir -p $@

rebuild: clean all

clean:
	@echo "Cleaning project..."
	@rm -f $(TARGET) $(ELF_TARGET)
	@rm -rf $(ODIR)

.PHONY: all clean rebuild