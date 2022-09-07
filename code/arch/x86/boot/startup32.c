#include <mm/page_types.h>
#include <boot/multiboot.h>

void startup32(unsigned int magic, multiboot_info_t *mbi)
{
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        return ;
    }
}