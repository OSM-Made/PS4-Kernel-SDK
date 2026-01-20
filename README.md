# PS4 Kernel SDK

A development kit for building self-contained PlayStation 4 kernel modules with automatic symbol resolution across multiple firmware versions.

## Overview

The PS4 Kernel SDK enables developers to create loadable kernel modules that can run on various PS4 firmware versions without recompilation. The SDK automatically detects the running firmware version and resolves the appropriate kernel symbols at runtime, making kernel development more portable and maintainable.

### Key Features

- **Automatic Symbol Resolution**: Detects firmware version and resolves kernel symbols automatically
- **Multi-Firmware Support**: Compatible with firmware versions 9.00, 12.02, 12.50, and 13.00
- **Self-Contained Modules**: Modules are standalone ELF files that can be loaded from userland
- **Module Metadata System**: Built-in support for module name, version, author, and description
- **Kernel ELF Loader**: Includes a loader library for loading modules from userland payloads
- **Comprehensive Type Definitions**: kernel structure and function definitions

### Current Status

This project is a work in progress. The SDK includes:
- Working hello_world sample demonstrating basic module functionality
- Loader example showing how to load kernel modules from userland
- Complete type definitions for PS4 kernel structures
- CRT (C Runtime) files for proper module initialization

## Project Structure

```
PS4 Kernel SDK/
├── include/              # Header files
│   ├── types/            # Kernel type definitions (Proc, VM, FS, etc.)
│   ├── offsets/          # Firmware-specific symbol offset tables
│   ├── resolver.h        # Kernel symbol resolver
│   └── metadata.h        # Module metadata declarations
├── lib/                  # CRT startup files
│   └── crt1.c            # Module initialization code
├── ld/                   # Linker scripts
│   └── module.ld         # Module linking layout
├── loader/               # Kernel ELF loader library
│   ├── include/          # Loader headers
│   └── source/           # Loader implementation
├── samples/              # Example modules
│   ├── hello_world/      # Basic kernel module example
│   └── userland_payload/ # Userland loader example
├── sdk.mk                # SDK build configuration
└── Makefile              # Main build system
```

## Requirements

### Build Tools

- **GCC/G++**: Compiler with support for btver2 architecture (PS4 CPU)
- **GNU Binutils**: objcopy, strip
- **GNU Make**: Build automation
- **Unix-like environment**: Linux, macOS, or WSL on Windows

### Target Platform

- PlayStation 4 console running firmware version 9.00, 12.02, 12.50, or 13.00
- Jailbroken system capable of executing custom payloads

## Building

### Build All Components

Build the complete SDK including libraries, loader, and samples:

```bash
make all
```

This will compile:
- CRT files (lib/)
- Loader libraries (loader/)
- Sample modules (samples/)

### Build Individual Components

```bash
# Build only CRT files
make lib

# Build only loader libraries
make loader

# Build only samples
make samples

# Build specific sample
make sample_kernel    # Build hello_world kernel module
make sample_payload   # Build userland payload with embedded module
```

### Install SDK

Install the loader to the SDK directory:

```bash
make install
```

### Clean Build Artifacts

```bash
make clean
```

## Creating Your Own Module

### Step 1: Create Module Directory

```bash
mkdir samples/my_module
cd samples/my_module
mkdir source
```

### Step 2: Create Makefile

Create `Makefile`:

```makefile
SDK = ../..
include $(SDK)/sdk.mk

# Module metadata
MODULE_NAME    = my_module
MODULE_VERSION = 1.0.0
MODULE_AUTHOR  = Your Name
MODULE_DESC    = Description of your module

all: $(TARGET)
```

### Step 3: Write Module Code

Create `source/main.cpp`:

```cpp
#include <resolver.h>
#include <metadata.h>

extern "C"
{
    // Called when module is loaded
    int module_init(void)
    {
        printf("My module loaded!\n");
        printf("Kernel Base: %p\n", (void*)g_KernelBase);
        printf("Firmware: %s\n", g_DetectedFirmware);

        // Your initialization code here

        return 0; // Return 0 on success
    }

    // Called when module is unloaded
    void module_fini(void)
    {
        printf("My module unloaded!\n");
    }
}
```

### Step 4: Build Module

```bash
make
```

This produces `my_module.elf` which can be loaded into the PS4 kernel.

## Loading Modules

### Method 1: From Userland Payload

Create a userland payload that embeds and loads your module:

```c
#include <Types/All.h>
#include <kloader.h>

extern unsigned char _binary_resources_my_module_elf_start[];
extern unsigned char _binary_resources_my_module_elf_end[];

int _main(void)
{
    kloader_load(
        _binary_resources_my_module_elf_start,
        (uint64_t)&_binary_resources_my_module_elf_end -
        (uint64_t)&_binary_resources_my_module_elf_start
    );

    return 0;
}
```

The `kloader_load` function will:
1. Detect the current firmware version
2. Jailbreak the calling process
3. Apply necessary kernel patches
4. Load the module into kernel memory
5. Execute the module's `module_init()` function

### Method 2: Direct Kernel Loading

Advanced users can integrate the ELF loader directly into their existing kernel exploits or payloads.

## Available APIs

Modules have access to a wide range of kernel functions through the resolver system:

- **Standard Library**: printf, malloc, free, memcpy, strlen, etc.
- **Process Management**: pfind, proc_rwmem, create_thread
- **Virtual Memory**: vm_map_lock, vm_map_insert, vm_map_protect
- **File System**: kern_open, kern_mkdir, vn_fullpath
- **Synchronization**: mtx_lock, mtx_unlock, sx_xlock, sx_xunlock
- **Event Handling**: eventhandler_register, eventhandler_deregister
- **Device Drivers**: make_dev_p, destroy_dev
- **And many more**: See `include/resolver.h` for the complete list

## Module Metadata

Every module automatically includes metadata that can be queried:

```cpp
extern const char __module_name[];     // Module name
extern const char __module_version[];  // Version string
extern const char __module_author[];   // Author name
extern const char __module_desc[];     // Description
```

These are automatically populated from your Makefile's MODULE_* variables.

## Supported Firmware Versions

The SDK currently supports automatic symbol resolution for:

- **9.00** (offsets-900.h)
- **12.02** (offsets-1202.h)
- **12.50** (offsets-1250.h)
- **13.00** (offsets-1300.h)

New firmware versions can be added by creating new offset header files in `include/offsets/` and updating the symbol resolver in `include/offsets/Offsets.h`.

## Build System Details

### Compiler Flags

The SDK uses specific flags optimized for PS4 kernel development:

- `-march=btver2`: Target PS4's Jaguar CPU architecture
- `-mcmodel=small`: Use small code model for kernel space
- `-fno-builtin`: Disable built-in function optimizations
- `-nostdlib`: Don't link against standard libraries
- `-D_KERNEL`: Enable kernel-mode definitions

### Linker Script

The custom linker script (`ld/module.ld`) ensures proper section layout for kernel modules, including:
- `.text` section for code
- `.data` and `.bss` for data
- `.modinfo` section for module metadata
- Proper alignment and permissions

## License

This is a work-in-progress SDK. Please ensure compliance with applicable laws and Sony's terms of service. This SDK is intended for educational and research purposes on authorized systems.

## Contributing

This project is under active development. Contributions are welcome, particularly:

- Additional firmware version symbol tables
- More sample modules demonstrating different features
- Documentation improvements
- Bug fixes and enhancements

## Troubleshooting

### Module fails to load

- Ensure you're on a supported firmware version
- Check that the module was built correctly (look for .elf file)
- Verify the system is jailbroken and can execute kernel payloads

### Build errors

- Verify GCC is installed and supports btver2 architecture
- Check that all SDK paths are correct in Makefile
- Ensure you're building from a Unix-like environment

### Symbol resolution fails

- Confirm your firmware version is supported
- Check kernel base address detection is working
- Review symbol offset tables for your specific firmware

## Examples

See the `samples/` directory for working examples:

- **hello_world**: Basic kernel module that prints information
- **userland_payload**: Demonstrates loading a kernel module from userland

## Additional Resources

For more information on PS4 kernel development:
- Kernel structure definitions: `include/types/`
- Symbol offset tables: `include/offsets/`
- Loader implementation: `loader/source/`

---

**Note**: This SDK is a work in progress. Features and APIs may change as development continues.
