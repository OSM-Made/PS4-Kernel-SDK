#include <Types/All.h>
#include <kloader.h>

/* If you embedded a kernel module in resources/ */
extern unsigned char _binary_resources_hello_world_elf_start[];
extern unsigned char _binary_resources_hello_world_elf_end[];

int _main(void) 
{
    /* Load the embedded kernel module */
    /* This will:
     *   1. Detect firmware version
     *   2. Jailbreak the process
     *   3. Apply kernel patches
     *   4. Load hello_world.elf into kernel
     *   5. Execute its module_init()
     */
    kloader_load(_binary_resources_hello_world_elf_start, (uint64_t)&_binary_resources_hello_world_elf_end - (uint64_t)&_binary_resources_hello_world_elf_start);
    
    return 0;
}