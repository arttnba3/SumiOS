#include <sumios/kernel.h>
#include <mm/mm.h>
#include <mm/layout.h>
#include <boot/multiboot2.h>

extern void *boot_mm_alloc(uint64_t sz);

extern phys_addr_t boot_kern_pgtable;
extern phys_addr_t boot_mem_alloc_start;

phys_addr_t kern_pgtable;
uint64_t total_mem = 0;
struct page **pages;
uint64_t pages_num;

/* initialize the memory management subsystem */
void mm_init(multiboot_uint8_t *mbi)
{
    struct multiboot_tag *tags;
    struct multiboot_tag_basic_meminfo *bmem_info;
    struct multiboot_tag_mmap *mmap_info = NULL;
    struct multiboot_mmap_entry *mmap_entry;
    uint64_t pg_idx = 0;

    /* We can just reuse the old page table for kernel */
    kern_pgtable = PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(boot_kern_pgtable);

    /**
     * Because we cannot use e820 and some other BIOS interrupts
     * under long mode, so we have to use the multiboot2 info provided by GRUB
     * to get the whole map of memory.
     */
    kputs("\t[*] Analyzing multiboot2 tags...");
    tags = (struct multiboot_tag *) (((uint64_t) mbi) + 8);
    while (tags->type != MULTIBOOT_TAG_TYPE_END) {
        switch (tags->type) {
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                /* It's a useless method now, we won't use it... */
                /*
                bmem_info = tags;
                kprintf("\t\t[*] mem_lower=0x%x, mem_upper=0x%x\n", 
                        bmem_info->mem_lower, bmem_info->mem_upper);*/
                break;
            case MULTIBOOT_TAG_TYPE_MMAP:
                mmap_info = tags;
                break;
            default:
                /* we don't need other tags there, just ignore it */
                break;
        }

        if (mmap_info) {
            break;
        }

        tags = (struct multiboot_tag *) \
                    ((multiboot_uint8_t *) tags + ((tags->size + 7) & ~7));
    }

    /* something went wrong! just hang on... */
    if (!mmap_info) {
        kprintf("[x] FAILED to get memory info from multiboot tags!");
        asm volatile (" hlt; ");
    }

    /* alloc for each page struct */
    for (int i = sizeof(struct multiboot_tag_mmap); 
        i < tags->size; 
        i += sizeof(struct multiboot_mmap_entry)) {
        mmap_entry = ((uint64_t)mmap_info) + i;

        /* TODO: analyze each area and alloc page struct */

        kprintf("\t\t[*] mmap type: 0x%x, base=0x%lx, len=0x%lx\n", 
                mmap_entry->type, mmap_entry->addr, mmap_entry->len);
    }

    /* unmap the old phys part */
    ((pgd_t*) kern_pgtable)[0] = NULL;
}