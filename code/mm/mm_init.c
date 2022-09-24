#include <sumios/kernel.h>
#include <mm/mm.h>
#include <mm/layout.h>
#include <boot/multiboot2.h>

extern phys_addr_t boot_kern_pgtable;
extern phys_addr_t boot_mem_alloc_start, boot_mem_total;

phys_addr_t kern_pgtable;
uint64_t total_mem = 0;
struct page *pages = KERNEL_PAGE_ARRAY;
uint64_t pages_num;

static phys_addr_t mm_alloc_start = -1;
static phys_addr_t mm_alloc_end;
#define PHYS_MM_SEGS_NUM_MAX 0x20
static struct phys_mm_area {
    phys_addr_t start;
    phys_addr_t end;
} phys_mm_area_arr[PHYS_MM_SEGS_NUM_MAX] = {0};
static int phys_mm_area_num = 0;
static int phys_mm_area_idx = 0;

/**
 * @brief This's just a simple linear mm allocator for booting stage to
 * build up the page table for kernel image, and build up GDT,IDT, etc. 
 * So we don't concern about free there.
 * 
 * @param sz the size to alloc. it'll be round up to page-aligned
 * @return void* :the allocated memory, NULL for failing
 */
void *mm_phys_alloc_linear(uint64_t sz)
{
    void *chunk;

    if ((!sz)){
        kputs("[x] bad size!");
        return NULL;
    }

    sz = ALIGN(sz, PAGE_SIZE);

    if (((sz + mm_alloc_start) > mm_alloc_end)) {
        kprintf("[x] sz round up to: 0x%lx, that\'s too big!\n", 
                ALIGN(sz, PAGE_SIZE));
        return NULL;
    }

    chunk = (void*) mm_alloc_start;
    mm_alloc_start += sz;

    /* we have run out current memory segment, slide to next one */
    if (mm_alloc_start >= mm_alloc_end) {
        kputs("\t[*] allocator moves to next area!");
        while (phys_mm_area_idx < phys_mm_area_num) {
            if (phys_mm_area_arr[phys_mm_area_idx].start >= mm_alloc_end) {
                mm_alloc_start = phys_mm_area_arr[phys_mm_area_idx].start;
                mm_alloc_end = phys_mm_area_arr[phys_mm_area_idx].end;
                break;
            }

            phys_mm_area_idx++;
        }

        kprintf("\t[+] new mm_alloc_start: %p\n", mm_alloc_start);

        if (phys_mm_area_idx == phys_mm_area_num) {
            kputs("[x] We have run out all of the memory!");
        }
    }

    return chunk;
}

/* initialize the memory management subsystem */
void mm_init(multiboot_uint8_t *mbi)
{
    struct multiboot_tag *tags;
    struct multiboot_tag_basic_meminfo *bmem_info;
    struct multiboot_tag_mmap *mmap_info = NULL;
    struct multiboot_mmap_entry *mmap_entry;
    uint64_t pg_idx = 0;

    /* We can just reuse the old page table for kernel */
    kern_pgtable = boot_kern_pgtable;

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
                /* It's an out-of-date method now, we won't use it... */
                /*bmem_info = tags;
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
        kprintf("\t[x] FAILED to get memory info from multiboot tags!");
        asm volatile (" hlt; ");
    }

    /* analyze for valid memory areas */
    for (int i = sizeof(struct multiboot_tag_mmap); 
        i < tags->size; 
        i += sizeof(struct multiboot_mmap_entry)) {
        mmap_entry = ((uint64_t)mmap_info) + i;

        if (mmap_entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
            phys_mm_area_arr[phys_mm_area_num].start = mmap_entry->addr;
            phys_mm_area_arr[phys_mm_area_num].end = \
                                            mmap_entry->addr + mmap_entry->len;
            
            if (boot_mem_alloc_start >= phys_mm_area_arr[phys_mm_area_num].start
            && boot_mem_alloc_start < phys_mm_area_arr[phys_mm_area_num].end) {
                mm_alloc_start = boot_mem_alloc_start;
                mm_alloc_end = phys_mm_area_arr[phys_mm_area_num].end;
            }

            phys_mm_area_num++;
        }
    }

    /* map for direct mapping area*/
    for (phys_addr_t i = sizeof(struct multiboot_tag_mmap); 
        i < tags->size; 
        i += sizeof(struct multiboot_mmap_entry)) {
        mmap_entry = ((uint64_t)mmap_info) + i;
        for (size_t pa = mmap_entry->addr;
            pa < (mmap_entry->addr + mmap_entry->len);
            pa += PAGE_SIZE) {
            mm_pgtable_map(kern_pgtable, PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(pa),
                           pa, PAGE_ATTR_RW | PAGE_ATTR_P);
        }
    }

    /* alloc for each page struct */
    for (phys_addr_t i = sizeof(struct multiboot_tag_mmap); 
        i < tags->size; 
        i += sizeof(struct multiboot_mmap_entry)) {
        mmap_entry = ((uint64_t)mmap_info) + i;

        kprintf("\t\t[*] mmap type: 0x%x, base=0x%lx, len=0x%lx\n", 
                mmap_entry->type, mmap_entry->addr, mmap_entry->len);

        for (size_t start = mmap_entry->addr;
            start < (mmap_entry->addr + mmap_entry->len);
            start += PAGE_SIZE) {
            size_t idx = start / PAGE_SIZE;
            struct page *p = &pages[idx];

            /* alloc space for pages struct */
            if (mm_pgtable_get_va_pte(kern_pgtable, p) == -1) {
                phys_addr_t s = mm_phys_alloc_linear(PAGE_SIZE);
                mm_pgtable_map(kern_pgtable, p, s, PAGE_ATTR_P | PAGE_ATTR_RW);
            }

            p->ref_count = -1;
            p->flags = mmap_entry->type;
        }
    }

    /**
     * We have used some pages while initialize pages' array, so we donot count
     * that during initialization but after it.
     */
    /* TODO: initialize the buddy system */

    /* unmap the old phys part */
    ((pgd_t*) kern_pgtable)[0] = NULL;
}
