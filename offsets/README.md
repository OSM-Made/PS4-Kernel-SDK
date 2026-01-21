# PS4 Kernel Offsets

This directory contains YAML configuration files that define kernel symbol offsets for different PS4 firmware versions. These files are used to generate the C header files in `include/offsets/`.

## File Format

Each YAML file follows this structure:

```yaml
firmware:
  version: "X.XX"                # Human-readable version
  build_number: XXXXXX           # Kernel build number
  init_function: InitKernelXXXX  # Generated function name

offsets:
  - name: symbol_name
    offset: 0xHEXADECIMAL
    type: void* | uint64_t
    category: Category Name
```

## Offset Types

### Regular Offsets
Most offsets are simple memory addresses relative to kernel base:

```yaml
- name: printf
  offset: 0x000B7A30
  type: void*
  category: STD Lib
```

### Data Offsets
Some offsets point to kernel data structures (use `uint64_t` type):

```yaml
- name: QAFlags
  offset: 0x02216850
  type: uint64_t
  category: Util
```

### Array Offsets
Complex structures with relative offsets:

```yaml
- name: mdbgAssistMode
  type: uint64_t[22]
  category: Kernel Patches
  base_offset: patch_mdbg_basic
  relative_offsets: [0x54, 0x238, 0x1416, ...]
```

## Maintenance Guidelines

### Updating Offsets

1. Always update from a known-good kernel dump
2. Verify critical functions (printf, malloc, free) work correctly
3. Test on actual hardware before committing
4. Document any offset changes in commit messages

### Adding New Offsets

1. Determine the appropriate category
2. Use a descriptive name matching the kernel function
3. Choose correct type (`void*` for functions, `uint64_t` for data)
4. Add comments in the YAML if the offset is tricky to find

### Quality Checks

Before regenerating headers:
- Validate YAML syntax
- Check all hex values are properly formatted
- Ensure no duplicate offset names
- Verify firmware metadata is correct

## Regenerating Headers

After modifying any YAML file:

```bash
# Via Makefile (recommended)
make regenerate-offsets

# Or directly
python tools/generate_offsets.py
```

This will regenerate all C headers in `include/offsets/`.
