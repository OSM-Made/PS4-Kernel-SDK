#include <resolver.h>
#include <metadata.h>

extern "C"
{
    int module_init(void)
    {
        printf("===========================================\n");
        printf("         Hello World Kernel Module         \n");
        printf("===========================================\n");
        printf("Kernel Base: %p\n", (void*)g_KernelBase);
        printf("Firmware: %s\n", g_DetectedFirmware);
        printf("Module loaded successfully!\n");
        printf("===========================================\n");
        printf("              Module Metadata              \n");
        printf("===========================================\n");
        printf("  Module:  %s\n", __module_name);
        printf("  Version: %s\n", __module_version);
        printf("  Author:  %s\n", __module_author);
        printf("  Desc:    %s\n", __module_desc);
        printf("===========================================\n");

        return 0;
    }

    void module_fini(void)
    {
        printf("Goodbye from hello_world module!\n");
    }
}