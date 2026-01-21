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
├── include/                  # Header files
│   ├── types/                # Kernel type definitions (Proc, VM, FS, etc.)
│   ├── offsets/              # Generated firmware-specific offset headers
│   ├── resolver.h            # Kernel symbol resolver
│   └── metadata.h            # Module metadata declarations
├── offsets/                  # YAML offset configuration files
├── tools/                    # Build and code generation tools
│   └── generate_offsets.py   # Generates C headers from YAML
├── lib/                      # CRT startup files
│   └── crt1.c                # Module initialization code
├── ld/                       # Linker scripts
│   └── module.ld             # Module linking layout
├── loader/                   # Kernel ELF loader library
│   ├── include/              # Loader headers
│   └── source/               # Loader implementation
├── samples/                  # Example modules
│   ├── hello_world/          # Basic kernel module example
│   └── userland_payload/     # Userland loader example
├── sdk.mk                    # SDK build configuration
└── Makefile                  # Main build system
```

## Requirements

### Build Tools

- **GCC/G++**: Compiler with support for btver2 architecture (PS4 CPU)
- **GNU Binutils**: objcopy, strip
- **GNU Make**: Build automation
- **Python 3**: For offset generation (requires PyYAML: `pip install pyyaml`)
- **Unix-like environment**: Linux, macOS, or WSL on Windows

### Target Platform

- PlayStation 4 console running firmware version 9.00, 12.02, 12.50, or 13.00
- Jailbroken system capable of executing custom payloads

## Building

### Generate Offset Headers

Before building, you need to generate the C header files from the YAML offset definitions:

```bash
# Generate all offset headers from YAML
make offsets
```

This reads the YAML files in `offsets/` and generates:
- `include/offsets/Offsets.h` - Main header with struct and utility functions
- `include/offsets/offsets-XXX.h` - Individual firmware initialization functions

**Note**: Offset headers are auto-generated and should not be edited directly. Modify the YAML files instead.

### Build All Components

Build the complete SDK including libraries, loader, and samples:

```bash
make all
```

This will:
1. Generate offset headers (if needed)
2. Compile CRT files (lib/)
3. Build loader libraries (loader/)
4. Build sample modules (samples/)

### Build Individual Components

```bash
# Generate offset headers only
make offsets

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

### Clean Build Artifacts

```bash
# Clean compiled objects and binaries
make clean

# Clean everything including generated headers
make distclean
```

## Supported Firmware Versions

The SDK currently supports automatic symbol resolution for:

- **9.00** (Build 178153) - `offsets/firmware-900.yaml`
- **12.02** (Build 222806) - `offsets/firmware-1202.yaml`
- **12.50** (Build 223219) - `offsets/firmware-1250.yaml`
- **13.00** (Build 225997) - `offsets/firmware-1300.yaml`

### Adding New Firmware Versions

To add support for a new firmware version:

1. **Create a new YAML file** in `offsets/` directory:
   ```bash
   cp offsets/firmware-1300.yaml offsets/firmware-XXXX.yaml
   ```

2. **Update firmware metadata**:
   ```yaml
   firmware:
     version: "XX.XX"
     build_number: XXXXXX
     init_function: InitKernelXXXX
   ```

3. **Update all offsets** with values for the new firmware version. You can find these by:
   - Analyzing the kernel binary with IDA Pro/Ghidra
   - Using kernel debugging tools
   - Referencing existing offset databases

4. **Regenerate headers**:
   ```bash
   make offsets
   ```

5. **Test** with a kernel module on the target firmware

The offset generation system automatically:
- Creates the C struct definition
- Generates initialization functions for each firmware
- Updates firmware detection logic
- Maintains proper type casting and array handling

## Examples

See the `samples/` directory for working examples:

- **hello_world**: Basic kernel module that prints information
- **userland_payload**: Demonstrates loading a kernel module from userland
