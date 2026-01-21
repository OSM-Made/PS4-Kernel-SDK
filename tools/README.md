# PS4 Kernel SDK - Build Tools

This directory contains Python scripts for code generation and SDK maintenance.

## Scripts

### generate_offsets.py

The main offset header generator that converts YAML offset definitions into C header files.

**Usage:**
```bash
python tools/generate_offsets.py
```

Or via Makefile:
```bash
make offsets              # Generate if missing or out of date
make regenerate-offsets   # Force regeneration
```

**What it does:**
- Reads all `offsets/firmware-*.yaml` files
- Generates `include/offsets/Offsets.h` with the KernelAddrs structure
- Generates `include/offsets/offsets-XXX.h` for each firmware version
- Creates firmware detection and initialization functions

**Requirements:**
- Python 3.x
- PyYAML library (`pip install pyyaml`)

**Output files:**
- `include/offsets/Offsets.h` - Main header
- `include/offsets/offsets-900.h` - 9.00 offsets
- `include/offsets/offsets-1202.h` - 12.02 offsets
- `include/offsets/offsets-1250.h` - 12.50 offsets
- `include/offsets/offsets-1300.h` - 13.00 offsets

### convert_offsets_to_yaml.py

Utility script to convert existing C offset headers to YAML format.

**Usage:**
```bash
python tools/convert_offsets_to_yaml.py
```

**What it does:**
- Parses existing `include/offsets/offsets-*.h` files
- Extracts offset definitions, types, and categories
- Generates YAML files in `offsets/` directory

**Note:** This script was used for the initial migration from C headers to YAML. You typically don't need to run it unless you're adding offsets from an external C header file.

## YAML Format

The YAML offset files follow this structure:

```yaml
firmware:
  version: "X.XX"           # Human-readable version (e.g., "9.00")
  build_number: XXXXXX      # Build number from kernel string
  init_function: InitKernelXXXX  # C function name

offsets:
  # Regular offsets
  - name: function_name
    offset: 0xHEXVALUE
    type: void*             # or uint64_t
    category: Category Name

  # Arrays with relative offsets
  - name: array_name
    type: uint64_t[N]
    category: Category Name
    base_offset: base_field_name
    relative_offsets: [0xOFF1, 0xOFF2, ...]
```

### Categories

Offsets are organized into logical categories:
- **Util** - Core kernel utilities and syscalls
- **STD Lib** - Standard library functions
- **Event Handling** - Event handler management
- **Proc** - Process management
- **Virtual Memory** - VM subsystem
- **Mutex Locks** - Synchronization primitives
- **Driver** - Device driver functions
- **Flash & NVS** - Flash and NVS storage
- **Sysctl** - Sysctl interface
- **FSelfs** - SELF file authentication
- **Fake Pkgs** - Package spoofing/patching
- **Library Replacement** - Dynamic library loading
- **TTY Redirector** - Console redirection
- **Kernel Patches** - Memory patches and hooks

## Adding New Firmware Versions

1. **Create YAML file:**
   ```bash
   cp offsets/firmware-1300.yaml offsets/firmware-XXXX.yaml
   ```

2. **Update metadata:**
   ```yaml
   firmware:
     version: "XX.XX"
     build_number: XXXXXX  # From kernel build string
     init_function: InitKernelXXXX
   ```

3. **Update offsets:**
   - Use IDA Pro/Ghidra to find kernel symbols
   - Update each offset hex value
   - Maintain the same structure

4. **Regenerate headers:**
   ```bash
   make regenerate-offsets
   ```

5. **Verify:**
   - Check that `include/offsets/Offsets.h` includes new firmware
   - Verify `DetectFirmwareVersion()` has the new build number
   - Test with a kernel module

## Troubleshooting

**Error: "No module named 'yaml'"**
```bash
pip install pyyaml
```

**Error: "No firmware configurations found"**
- Ensure YAML files exist in `offsets/` directory
- Check that files match pattern `firmware-*.yaml`

**Generated headers look wrong**
- Validate YAML syntax with a YAML linter
- Check that offset values are hex strings (e.g., `0x12345678`)
- Ensure array types use proper syntax: `uint64_t[N]`

**Offsets not matching on console**
- Verify firmware version and build number
- Check that offsets are from the correct kernel version
- Some symbols may be inlined or optimized out in certain builds

## Development

To modify the generator:

1. Edit `tools/generate_offsets.py`
2. Test changes: `python tools/generate_offsets.py`
3. Verify generated headers compile correctly
4. Check that generated code matches expected output

The generator uses:
- `yaml.safe_load()` for parsing YAML
- OrderedDict to maintain category order
- Template-based code generation
