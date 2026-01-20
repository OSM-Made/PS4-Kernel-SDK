OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

PHDRS
{
  text PT_LOAD FLAGS(5);  /* R+X */
  data PT_LOAD FLAGS(6);  /* R+W */
}

SECTIONS
{
  . = 0x0;
  
  .text : ALIGN(0x1000) { 
    *(.text .text.*)
  } :text
  
  .rodata : ALIGN(0x1000) {
    *(.rodata .rodata.*)
  } :text
  
  /* Module metadata section */
  .modinfo : ALIGN(0x10) {
    PROVIDE(__modinfo_start = .);
    *(.modinfo)
    PROVIDE(__modinfo_end = .);
  } :text
  
  .data : ALIGN(0x1000) { 
    *(.data .data.*)
  } :data
  
  .bss : ALIGN(0x1000) { 
    *(.bss .bss.*)
    *(COMMON)
  } :data
  
  /DISCARD/ : { 
    *(.eh_frame)
    *(.comment)
    *(.note.GNU-stack)
  }
}