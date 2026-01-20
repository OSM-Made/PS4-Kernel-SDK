.intel_syntax noprefix

.section .text

.global _start
.global _fini
.global CurrentThread
.global Readmsr
.global cpu_enable_wp
.global cpu_disable_wp

.extern _resolver
.extern module_init

# Weak symbol - module_fini is optional
.weak module_fini

# Module entry point
_start:
    call _resolver
    call module_init
    ret

# Module cleanup (optional)
_fini:
    # Check if module_fini exists before calling
    lea rax, [rip + module_fini]
    test rax, rax
    jz .skip
    
    call module_fini

.skip:
    ret

CurrentThread:
    mov rax, gs:0
    ret

Readmsr:
    mov ecx, edi
    rdmsr
    shl rdx, 32
    or rax, rdx
    ret

cpu_enable_wp:
    mov rax, cr0
    or rax, 0x10000
    mov cr0, rax
    ret

cpu_disable_wp:
    mov rax, cr0
    and rax, ~0x10000
    mov cr0, rax
    ret

.section .note.GNU-stack,"",@progbits
