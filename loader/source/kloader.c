#include <Types/All.h>
#include <offsets/offsets.h>

#include "elf_loader.h"

extern unsigned long syscall(unsigned long n, ...);
extern uint64_t Readmsr(int Reg);
extern void cpu_enable_wp();
extern void cpu_disable_wp();

KernelAddrs g_KernelAddrs;

struct install_args 
{
    uint64_t sycall;
    void* payload;
    size_t payload_size;
};

static void* (*kernel_memcpy)(void* dst, const void* src, size_t len) = NULL;
static void* (*kernel_memset)(void* ptr, int value, size_t num) = NULL;
static void (*kernel_printf)(const char* fmt, ...) = NULL;
static uint64_t(*kmem_alloc)(void* map, size_t size) = NULL;
static void (*kmem_free)(void* map, void* addr, size_t size) = NULL;
static void* kernel_map = NULL;

static elf_mem_ops_t kern_mem_ops;

void* kloader_alloc_exec(size_t size) 
{
    size_t aligned = (size + 0x3FFF) & ~0x3FFF;
    return (void*)kmem_alloc(kernel_map, aligned);
}

void kloader_free_exec(void* ptr, size_t size) 
{
    size_t aligned = (size + 0x3FFF) & ~0x3FFF;
    kmem_free(kernel_map, ptr, aligned);
}

int kloader_load_kern(void* elf_data, size_t elf_size, void** entry_out) 
{
    int ret;
    size_t mapped_size = 0;

    /* Get required size */
    if ((ret = elf_get_mapped_size(elf_data, &mapped_size)) != ELF_SUCCESS) 
    {
        kernel_printf("[kloader] Failed to get mapped size: %d\n", ret);
        return ret;
    }

    /* Allocate executable memory */
    void* base = kloader_alloc_exec(mapped_size);
    if (!base) {
        kernel_printf("[kloader] Failed to allocate memory\n");
        return ELF_ERR_NOMEM;
    }

    kernel_printf("[kloader] Allocated at %p\n", base);

    /* Setup context */
    elf_ctx_t ctx = {
        .elf_data = elf_data,
        .elf_size = elf_size,
        .base = base,
        .mapped_size = mapped_size,
        .entry = NULL,
        .ops = &kern_mem_ops
    };

    /* Load ELF */
    if ((ret = elf_load(&ctx)) != ELF_SUCCESS) 
    {
        kernel_printf("[kloader] Failed to load ELF: %d\n", ret);
        kloader_free_exec(base, mapped_size);
        return ret;
    }

    kernel_printf("[kloader] ELF loaded, entry: %p\n", ctx.entry);

    if (entry_out) 
    {
        *entry_out = ctx.entry;
    }

    return ELF_SUCCESS;
}

int kloader_install_kern(void* payload, size_t payload_size, uint64_t kern_base, KernelAddrs* addrs) 
{
    int ret;
    void* entry = NULL;

    if ((ret = kloader_load_kern(payload, payload_size, &entry)) != ELF_SUCCESS) 
    {
        return ret;
    }

    kernel_printf("[kloader] Calling entry point...\n");
    int (*module_entry)() = entry;
    ret = module_entry();

    if (ret != 0) 
    {
        kernel_printf("[kloader] Module returned error: %d\n", ret);
        return ret;
    }

    kernel_printf("[kloader] Module loaded successfully\n");
    return 0;
}

static void jailbreak_proc(struct thread* td) 
{
    struct ucred* cred = td->td_proc->p_ucred;
    struct filedesc* fd = td->td_proc->p_fd;

    cred->cr_prison = *(struct prison**)(g_KernelAddrs.prison0);
    fd->fd_rdir = *(struct vnode**)(g_KernelAddrs.rootvnode);
    fd->fd_jdir = *(struct vnode**)(g_KernelAddrs.rootvnode);

    cred->cr_uid = 0;
    cred->cr_ruid = 0;
    cred->cr_rgid = 0;
    cred->cr_groups[0] = 0;

    cred->cr_sceAuthID = 0x3801000000000013;
    cred->cr_sceCaps[0] = 0xFFFFFFFFFFFFFFFF;
    cred->cr_sceCaps[1] = 0xFFFFFFFFFFFFFFFF;
}

static void apply_patches() 
{
    cpu_disable_wp();

    *(uint8_t*)(g_KernelAddrs.patch_memcpy) = 0xEB;
    *(uint8_t*)(g_KernelAddrs.patch_kmem_alloc1) = VM_PROT_ALL;
    *(uint8_t*)(g_KernelAddrs.patch_kmem_alloc2) = VM_PROT_ALL;

    cpu_enable_wp();
}

static int install_kernel_syscall(struct thread* td, struct install_args* args) 
{
    uint64_t kernelBase = (uint64_t)(Readmsr(0xC0000082) - 0x1C0);

    const char* detectedFirmware = InitializeFirmwareOffsets(&g_KernelAddrs, kernelBase);
    if (detectedFirmware == 0)
    {
        /* Failed to detect firmware */
        return 1;
    }
    kernel_memcpy = (void*)(g_KernelAddrs.memcpy);
    kernel_memset = (void*)(g_KernelAddrs.memset);
    kernel_printf = (void*)(g_KernelAddrs.printf);
    kmem_alloc = (void*)(g_KernelAddrs.kmem_alloc);
    kmem_free = (void*)(g_KernelAddrs.kmem_free);
    kernel_map = *(void**)(g_KernelAddrs.kernel_map);

    kern_mem_ops.memcpy = kernel_memcpy,
    kern_mem_ops.memset = kernel_memset,
    
    jailbreak_proc(td);
    apply_patches();

    return kloader_install_kern(args->payload, args->payload_size, kernelBase, &g_KernelAddrs);
}

int kloader_load(void* payload, size_t payload_size) 
{
    return syscall(11, install_kernel_syscall, payload, payload_size);
}