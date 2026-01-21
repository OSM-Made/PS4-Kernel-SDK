#!/usr/bin/env python3
"""
PS4 Kernel Offsets Generator
Generates C header files from YAML offset definitions

Usage:
    python generate_offsets.py

This script reads all firmware-*.yaml files from the offsets/ directory
and generates corresponding C header files in include/offsets/:
    - Offsets.h: Main header with struct definition and utility functions
    - offsets-XXX.h: Individual firmware offset initialization functions

To add a new firmware version:
    1. Create a new YAML file: offsets/firmware-XXXX.yaml
    2. Define the firmware metadata (version, build_number, init_function)
    3. Add all offset definitions with their categories and types
    4. Run this script to regenerate the headers
    5. Update DetectFirmwareVersion build number mapping if needed

YAML Format:
    firmware:
      version: "X.XX"
      build_number: XXXXXX
      init_function: InitKernelXXXX

    offsets:
      - name: offset_name
        offset: 0xHEXVALUE
        type: void*|uint64_t
        category: Category Name

      # For arrays with relative offsets:
      - name: array_name
        type: uint64_t[N]
        category: Category Name
        base_offset: base_field_name
        relative_offsets: [0xOFF1, 0xOFF2, ...]
"""

import yaml
import os
from pathlib import Path
from typing import Dict, List, Any
from collections import OrderedDict


class OffsetsGenerator:
    def __init__(self, offsets_dir: str, include_dir: str):
        self.offsets_dir = Path(offsets_dir)
        self.include_dir = Path(include_dir)
        self.firmwares = []

    def load_firmware_configs(self):
        """Load all firmware YAML configuration files"""
        yaml_files = sorted(self.offsets_dir.glob("firmware-*.yaml"))

        for yaml_file in yaml_files:
            with open(yaml_file, 'r') as f:
                config = yaml.safe_load(f)
                self.firmwares.append(config)

        print(f"Loaded {len(self.firmwares)} firmware configurations")

    def generate_warning_header(self) -> str:
        """Generate warning header for auto-generated files"""
        lines = []
        lines.append("/*")
        lines.append(" * AUTO-GENERATED FILE - DO NOT EDIT")
        lines.append(" *")
        lines.append(" * This file was automatically generated from YAML offset definitions.")
        lines.append(" * To modify offsets, edit the source YAML files in offsets/ directory")
        lines.append(" * and regenerate with: make regenerate-offsets")
        lines.append(" *")
        lines.append(" * Source: offsets/firmware-*.yaml")
        lines.append(" * Generator: tools/generate_offsets.py")
        lines.append(" */")
        return "\n".join(lines)

    def generate_struct_definition(self) -> str:
        """Generate the KernelAddrs structure definition"""
        if not self.firmwares:
            raise ValueError("No firmware configurations loaded")

        # Use the first firmware as the reference for structure
        reference = self.firmwares[0]

        lines = []
        lines.append("/* Resolved addresses structure */")
        lines.append("typedef struct _KernelAddrs")
        lines.append("{")

        # Group offsets by category
        categories = OrderedDict()
        for offset in reference['offsets']:
            category = offset.get('category', 'Misc')
            if category not in categories:
                categories[category] = []
            categories[category].append(offset)

        # Generate struct members
        for category, offsets in categories.items():
            lines.append(f"    /* {category} */")
            for offset in offsets:
                name = offset['name']
                offset_type = offset['type']

                # Handle array types (e.g., "uint64_t[22]" -> "uint64_t name[22]")
                if '[' in offset_type and ']' in offset_type:
                    base_type = offset_type.split('[')[0]
                    array_size = '[' + offset_type.split('[')[1]
                    lines.append(f"    {base_type} {name}{array_size};")
                else:
                    lines.append(f"    {offset_type} {name};")
            lines.append("")

        lines.append("} KernelAddrs;")

        return "\n".join(lines)

    def generate_firmware_header(self, firmware: Dict[str, Any]) -> str:
        """Generate individual firmware offset header file"""
        version = firmware['firmware']['version']
        build_number = firmware['firmware']['build_number']
        init_function = firmware['firmware']['init_function']

        lines = []
        lines.append(self.generate_warning_header())
        lines.append("")
        lines.append("#pragma once")
        lines.append("")
        lines.append("#include <stdint.h>")
        lines.append("")
        lines.append(f"static inline void {init_function}(uint64_t kernelBase, KernelAddrs* addrs)")
        lines.append("{")

        # Group by category for organized output
        categories = OrderedDict()
        for offset in firmware['offsets']:
            category = offset.get('category', 'Misc')
            if category not in categories:
                categories[category] = []
            categories[category].append(offset)

        # Generate offset assignments
        for category, offsets in categories.items():
            lines.append(f"    /* {category} */")
            for offset in offsets:
                name = offset['name']
                offset_type = offset['type']

                # Handle array types specially
                if '[' in offset_type and ']' in offset_type:
                    # This is an array with relative offsets
                    base = offset.get('base_offset', '')
                    rel_offsets = offset.get('relative_offsets', [])

                    if base and rel_offsets:
                        # Extract base type
                        base_type = offset_type.split('[')[0]

                        # Generate the relative offsets array
                        # Handle both int and hex string formats
                        formatted_offsets = []
                        for o in rel_offsets:
                            if isinstance(o, int):
                                formatted_offsets.append(f"0x{o:X}")
                            elif isinstance(o, str):
                                # Already a hex string, use as-is
                                formatted_offsets.append(o)
                            else:
                                formatted_offsets.append(str(o))

                        offsets_str = ", ".join(formatted_offsets)
                        lines.append(f"    {base_type} {name}_offsets[] = {{ {offsets_str} }};")
                        lines.append(f"    for (int i = 0; i < {len(rel_offsets)}; i++) {{")
                        lines.append(f"        addrs->{name}[i] = addrs->{base} + {name}_offsets[i];")
                        lines.append(f"    }}")
                else:
                    # Regular offset
                    offset_value = offset.get('offset', 0)

                    if offset_type == 'uint64_t':
                        lines.append(f"    addrs->{name} = kernelBase + 0x{offset_value:08X};")
                    else:
                        lines.append(f"    addrs->{name} = (void*)(kernelBase + 0x{offset_value:08X});")

            lines.append("")

        lines.append("}")
        lines.append("")

        return "\n".join(lines)

    def generate_detect_firmware_version(self) -> str:
        """Generate DetectFirmwareVersion function"""
        lines = []
        lines.append("static int DetectFirmwareVersion(uint64_t kernelbase)")
        lines.append("{")
        lines.append("    const char* buildString = (char*)(*(uint64_t*)(kernelbase + 0x130) + 0x80);")
        lines.append("    if (buildString == 0 || buildString[0] != 'r')")
        lines.append("    {")
        lines.append("        return -1;")
        lines.append("    }")
        lines.append("")
        lines.append("    int buildNum = 0;")
        lines.append("    for (int i = 1; buildString[i] >= '0' && buildString[i] <= '9'; i++)")
        lines.append("    {")
        lines.append("        buildNum = buildNum * 10 + (buildString[i] - '0');")
        lines.append("    }")
        lines.append("")
        lines.append("    switch (buildNum)")
        lines.append("    {")

        # Generate switch cases for each firmware
        for firmware in self.firmwares:
            build_number = firmware['firmware']['build_number']
            version_code = firmware['firmware']['version'].replace('.', '')
            lines.append(f"    case {build_number}:")
            lines.append(f"        return {version_code};")

        lines.append("    default:")
        lines.append("        return -1;")
        lines.append("    }")
        lines.append("}")

        return "\n".join(lines)

    def generate_init_firmware_with_version(self) -> str:
        """Generate InitializeFirmwareOffsetsWithVersion function"""
        lines = []
        lines.append("static inline const char* InitializeFirmwareOffsetsWithVersion(KernelAddrs* offsets, uint64_t kernelBase, int firmware)")
        lines.append("{")
        lines.append("    if (!offsets) return 0;")
        lines.append("")
        lines.append("    switch (firmware)")
        lines.append("    {")

        # Generate switch cases for each firmware
        for firmware in self.firmwares:
            version = firmware['firmware']['version']
            version_code = version.replace('.', '')
            init_function = firmware['firmware']['init_function']

            lines.append(f"    case {version_code}:")
            lines.append(f"        {init_function}(kernelBase, offsets);")
            lines.append(f"        return \"{version}\";")

        lines.append("    default:")
        lines.append("        return 0;")
        lines.append("    }")
        lines.append("}")

        return "\n".join(lines)

    def generate_init_firmware(self) -> str:
        """Generate InitializeFirmwareOffsets function"""
        lines = []
        lines.append("static inline const char* InitializeFirmwareOffsets(KernelAddrs* offsets, uint64_t kernelBase)")
        lines.append("{")
        lines.append("    int firmware = DetectFirmwareVersion(kernelBase);")
        lines.append("")
        lines.append("    return InitializeFirmwareOffsetsWithVersion(offsets, kernelBase, firmware);")
        lines.append("}")

        return "\n".join(lines)

    def generate_main_header(self) -> str:
        """Generate the main Offsets.h header file"""
        lines = []
        lines.append(self.generate_warning_header())
        lines.append("")
        lines.append("#pragma once")
        lines.append("")
        lines.append("#include <stdint.h>")
        lines.append("")
        lines.append("#ifdef __cplusplus")
        lines.append("extern \"C\" {")
        lines.append("#endif")
        lines.append("")

        # Add struct definition
        lines.append(self.generate_struct_definition())
        lines.append("")

        # Add include statements for firmware headers
        lines.append("/* Include implementation files */")
        for firmware in self.firmwares:
            version_code = firmware['firmware']['version'].replace('.', '')
            lines.append(f"#include \"offsets-{version_code}.h\"")
        lines.append("")

        # Add utility functions
        lines.append(self.generate_detect_firmware_version())
        lines.append("")
        lines.append(self.generate_init_firmware_with_version())
        lines.append("")
        lines.append(self.generate_init_firmware())
        lines.append("")

        lines.append("#ifdef __cplusplus")
        lines.append("}")
        lines.append("#endif")

        return "\n".join(lines)

    def generate(self):
        """Generate all header files"""
        # Load firmware configurations
        self.load_firmware_configs()

        if not self.firmwares:
            print("ERROR: No firmware configurations found!")
            return

        # Create output directory if it doesn't exist
        output_dir = self.include_dir / "offsets"
        output_dir.mkdir(parents=True, exist_ok=True)

        # Generate individual firmware headers
        for firmware in self.firmwares:
            version_code = firmware['firmware']['version'].replace('.', '')
            header_path = output_dir / f"offsets-{version_code}.h"

            print(f"Generating {header_path}...")
            content = self.generate_firmware_header(firmware)

            with open(header_path, 'w') as f:
                f.write(content)

        # Generate main header
        main_header_path = output_dir / "Offsets.h"
        print(f"Generating {main_header_path}...")
        content = self.generate_main_header()

        with open(main_header_path, 'w') as f:
            f.write(content)

        print("Generation complete!")


def main():
    # Determine paths relative to script location
    script_dir = Path(__file__).parent
    project_root = script_dir.parent

    offsets_dir = project_root / "offsets"
    include_dir = project_root / "include"

    # Create generator and run
    generator = OffsetsGenerator(str(offsets_dir), str(include_dir))
    generator.generate()


if __name__ == "__main__":
    main()
