#include <Types/All.h>
#include <offsets/offsets.h>

#include "elf_loader.h"

/* Validate ELF header */
int elf_validate(void* elf_data, size_t elf_size) 
{
    if (!elf_data || elf_size < sizeof(Elf64_Ehdr)) 
    {
        return ELF_ERR_INVALID;
    }

    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)elf_data;

    /* Check magic */
    if (ehdr->e_ident[0] != 0x7F ||
        ehdr->e_ident[1] != 'E' ||
        ehdr->e_ident[2] != 'L' ||
        ehdr->e_ident[3] != 'F') {
        return ELF_ERR_INVALID;
    }

    /* Check 64-bit */
    if (ehdr->e_ident[4] != 2) {  /* ELFCLASS64 */
        return ELF_ERR_INVALID;
    }

    /* Check x86-64 */
    if (ehdr->e_machine != 0x3E) {  /* EM_X86_64 */
        return ELF_ERR_INVALID;
    }

    return ELF_SUCCESS;
}

/* Calculate required mapped size */
int elf_get_mapped_size(void* elf_data, size_t* out_size) 
{
    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)elf_data;
    size_t max_addr = 0;

    if (ehdr->e_phoff && ehdr->e_phnum > 0) 
    {
        /* Use program headers */
        Elf64_Phdr* phdr = (Elf64_Phdr*)((uint8_t*)elf_data + ehdr->e_phoff);

        for (int i = 0; i < ehdr->e_phnum; i++) 
        {
            if (phdr[i].p_type == PT_LOAD) 
            {
                uint64_t end = phdr[i].p_vaddr + phdr[i].p_memsz;
                if (end > max_addr) {
                    max_addr = end;
                }
            }
        }
    }
    else if (ehdr->e_shoff && ehdr->e_shnum > 0) 
    {
        /* Fallback: use section headers */
        Elf64_Shdr* shdr = (Elf64_Shdr*)((uint8_t*)elf_data + ehdr->e_shoff);

        for (int i = 0; i < ehdr->e_shnum; i++) 
        {
            if (shdr[i].sh_flags & SHF_ALLOC) 
            {
                uint64_t end = shdr[i].sh_addr + shdr[i].sh_size;
                if (end > max_addr) 
                {
                    max_addr = end;
                }
            }
        }
    }
    else {
        return ELF_ERR_INVALID;
    }

    /* Align to page boundary */
    *out_size = (max_addr + 0x3FFF) & ~0x3FFF;
    return ELF_SUCCESS;
}

/* Map segments into memory */
int elf_map_segments(elf_ctx_t* ctx) 
{
    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)ctx->elf_data;
    elf_mem_ops_t* ops = ctx->ops;

    if (ehdr->e_phoff && ehdr->e_phnum > 0) 
    {
        /* Use program headers (preferred) */
        Elf64_Phdr* phdr = (Elf64_Phdr*)((uint8_t*)ctx->elf_data + ehdr->e_phoff);

        for (int i = 0; i < ehdr->e_phnum; i++) {
            if (phdr[i].p_type != PT_LOAD) {
                continue;
            }

            void* dest = (uint8_t*)ctx->base + phdr[i].p_vaddr;
            void* src = (uint8_t*)ctx->elf_data + phdr[i].p_offset;

            /* Copy file contents */
            if (phdr[i].p_filesz > 0) {
                ops->memcpy(dest, src, phdr[i].p_filesz);
            }

            /* Zero remaining (BSS) */
            if (phdr[i].p_memsz > phdr[i].p_filesz) 
            {
                ops->memset((uint8_t*)dest + phdr[i].p_filesz,
                    0,
                    phdr[i].p_memsz - phdr[i].p_filesz);
            }
        }
    }
    else if (ehdr->e_shoff && ehdr->e_shnum > 0) {
        /* Fallback: use sections */
        Elf64_Shdr* shdr = (Elf64_Shdr*)((uint8_t*)ctx->elf_data + ehdr->e_shoff);

        for (int i = 0; i < ehdr->e_shnum; i++) 
        {
            if (!(shdr[i].sh_flags & SHF_ALLOC)) 
            {
                continue;
            }

            void* dest = (uint8_t*)ctx->base + shdr[i].sh_addr;

            if (shdr[i].sh_type != SHT_NOBITS && shdr[i].sh_size > 0) 
            {
                void* src = (uint8_t*)ctx->elf_data + shdr[i].sh_offset;
                ops->memcpy(dest, src, shdr[i].sh_size);
            }
            else if (shdr[i].sh_type == SHT_NOBITS) 
            {
                ops->memset(dest, 0, shdr[i].sh_size);
            }
        }
    }
    else 
    {
        return ELF_ERR_MAP;
    }

    return ELF_SUCCESS;
}

/* Apply relocations */
int elf_apply_relocations(elf_ctx_t* ctx) 
{
    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)ctx->elf_data;

    if (!ehdr->e_shoff || !ehdr->e_shnum) 
    {
        /* No sections, no relocations needed */
        return ELF_SUCCESS;
    }

    Elf64_Shdr* shdr = (Elf64_Shdr*)((uint8_t*)ctx->elf_data + ehdr->e_shoff);

    for (int i = 0; i < ehdr->e_shnum; i++) 
    {
        if (shdr[i].sh_type != SHT_RELA) 
        {
            continue;
        }

        Elf64_Rela* rela = (Elf64_Rela*)((uint8_t*)ctx->elf_data + shdr[i].sh_offset);
        int num_relocations = shdr[i].sh_size / sizeof(Elf64_Rela);

        for (int j = 0; j < num_relocations; j++) 
        {
            uint64_t* target = (uint64_t*)((uint8_t*)ctx->base + rela[j].r_offset);
            uint32_t type = ELF64_R_TYPE(rela[j].r_info);

            switch (type) {
            case R_X86_64_RELATIVE:
                *target = (uint64_t)ctx->base + rela[j].r_addend;
                break;

            case R_X86_64_64:
            case R_X86_64_GLOB_DAT:
            case R_X86_64_JUMP_SLOT:
                /* TODO: Symbol resolution if needed */
                /* For self-contained modules, usually not needed */
                break;

            case R_X86_64_NONE:
                break;

            default:
                /* Unknown relocation type - might be okay */
                break;
            }
        }
    }

    return ELF_SUCCESS;
}

/* Main load function */
int elf_load(elf_ctx_t* ctx) 
{
    int ret;

    /* Validate */
    if ((ret = elf_validate(ctx->elf_data, ctx->elf_size)) != ELF_SUCCESS) 
    {
        return ret;
    }

    /* Map segments */
    if ((ret = elf_map_segments(ctx)) != ELF_SUCCESS) 
    {
        return ret;
    }

    /* Apply relocations */
    if ((ret = elf_apply_relocations(ctx)) != ELF_SUCCESS) 
    {
        return ret;
    }

    /* Set entry point */
    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)ctx->elf_data;
    ctx->entry = (void*)((uint8_t*)ctx->base + ehdr->e_entry);

    return ELF_SUCCESS;
}