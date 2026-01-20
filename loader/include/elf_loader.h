#pragma once

#include <Types/All.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Error Codes */
#define ELF_SUCCESS        0
#define ELF_ERR_INVALID   -1
#define ELF_ERR_NOMEM     -2
#define ELF_ERR_SIZE      -3
#define ELF_ERR_MAP       -4
#define ELF_ERR_RELOC     -5

/* Memory operations abstraction */
typedef struct 
{
    void* (*memcpy)(void* dst, const void* src, size_t len);
    void* (*memset)(void* ptr, int value, size_t len);
} elf_mem_ops_t;

/* ELF Loader Context */
typedef struct 
{
    void* elf_data;          /* Source ELF data */
    size_t elf_size;         /* Source ELF size */
    void* base;              /* Mapped base address */
    size_t mapped_size;      /* Total mapped size */
    void* entry;             /* Entry point address */
    elf_mem_ops_t* ops;      /* Memory operations */
} elf_ctx_t;

/* Core ELF Functions (platform-agnostic) */
int elf_validate(void* elf_data, size_t elf_size);
int elf_get_mapped_size(void* elf_data, size_t* out_size);
int elf_map_segments(elf_ctx_t* ctx);
int elf_apply_relocations(elf_ctx_t* ctx);
int elf_load(elf_ctx_t* ctx);

#ifdef __cplusplus
}
#endif