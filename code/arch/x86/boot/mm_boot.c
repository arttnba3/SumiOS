#include <sumios/types.h>
#include <sumios/kernel.h>
#include <mm/layout.h>
#include <mm/mm.h>
#include <asm/mm.h>
#include <asm/io.h>

extern void boot_putchar(char ch);
extern void boot_printstr(char *str);
extern void boot_puts(char *str);
extern void boot_printnum(int64_t n);
extern void boot_printhex(uint64_t n);

extern void *boot_memset(uint8_t *dst, uint8_t val, uint64_t sz);

extern uint64_t __boot_start, __kernel_end, __roseg_end, boot_page_table_pud;

uint64_t boot_mem_total;
phys_addr_t boot_mem_alloc_start;
phys_addr_t boot_kern_pgtable;

/**
 * Read from mc146818 CMOS
 * 
 * This function refers to xv6. 
 */
static int nvram_read(int r)
{
    uint8_t low, high;

    outb(0x070, r);
    low = inb(0x070 + 1);
    outb(0x070, r + 1);
    high = inb(0x070 + 1);

    return low | (high << 8);
}

/**
 * Because we hadn't set up an IDT before entering long mode,
 * so we cannot use the legacy 0xe820 int to get memory size.
 * 
 * I just ONLY expect to set up a page table for image in this stage,
 * so I choose to get basic memory info by CMOS.
 * 
 * This function refers to xv6.
 */
static void boot_mm_detect(void)
{
    size_t mem_base, mem_ext, mem_ext16;

    #define	MC_NVRAM_START	0xe
    #define NVRAM_BASELO (MC_NVRAM_START + 7)
    #define NVRAM_EXTLO	(MC_NVRAM_START + 9)
    #define NVRAM_EXT16LO	(MC_NVRAM_START + 38)

    mem_base = nvram_read(NVRAM_BASELO);
    mem_ext = nvram_read(NVRAM_EXTLO);
    mem_ext16 = nvram_read(NVRAM_EXT16LO) * 64;

    if (mem_ext16) {
        boot_mem_total = 16 * 1024 + mem_ext16;
    } else if (mem_ext) {
        boot_mem_total = 1 * 1024 + mem_ext;
    } else {
        boot_mem_total = mem_base;
    }

    boot_puts("\n\t[+] boot memory deteced done. ");
    boot_printstr("\t[*] total: 0x");
    boot_printhex(boot_mem_total);
    boot_printstr("KB available, base = 0x");
    boot_printhex(mem_base);
    boot_printstr("KB, extend = 0x");
    boot_printhex(boot_mem_total - mem_base);
    boot_puts("KB");

    /* we still need to count it by Bytes in later use */
    boot_mem_total *= 1024;
}

/**
 * @brief This's just a simple linear mm allocator for booting stage to build up
 * the page table for kernel image, and build up GDT,IDT, etc. So we don't
 * concern about free there.
 * 
 * @param sz the size to alloc. it'll be round up to page-aligned
 * @return void* :the allocated memory, NULL for failing
 */
void *boot_mm_alloc(uint64_t sz)
{
    void *chunk;

    if ((!sz)){
        boot_puts("[x] bad size!");
        return NULL;
    }

    sz = ALIGN(sz, PAGE_SIZE);
    if (((sz + boot_mem_alloc_start) >= boot_mem_total)) {
        boot_printstr("[x] sz round up to: ");
        boot_printhex(ALIGN(sz, PAGE_SIZE) + boot_mem_alloc_start);
        boot_puts(", that\'s too big!");
        return NULL;
    }

    chunk = (void*) boot_mem_alloc_start;
    boot_mem_alloc_start += sz;

    return chunk;
}

phys_addr_t boot_get_pgtable_map_pa(phys_addr_t pgtable, virt_addr_t va)
{
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    int pgd_i = PGD_ENTRY(va);
    int pud_i = PUD_ENTRY(va);
    int pmd_i = PMD_ENTRY(va);
    int pte_i = PTE_ENTRY(va);

    pgd = pgtable;
    if (!pgd[pgd_i]) {
        boot_puts("[x] pgd not found!");
        return -1;
    }
    pud = pgd[pgd_i];
    if (!pud[pud_i]) {
        boot_puts("[x] pud not found!");
        return -1;
    }
    pmd = pud[pud_i];
    if (!pmd[pmd_i]) {
        boot_puts("[x] pmd not found!");
        return -1;
    }
    pte = pmd[pmd_i];
    return pte[pte_i];
}

/* map for single page */
void boot_pgtable_map(phys_addr_t pgtable, virt_addr_t va, phys_addr_t pa, 
                      page_attr_t attr)
{
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    int pgd_i = PGD_ENTRY(va);
    int pud_i = PUD_ENTRY(va);
    int pmd_i = PMD_ENTRY(va);
    int pte_i = PTE_ENTRY(va);

    /**
     * don't FORGET to set the attr for each level's entry!
     */
    pgd = pgtable;
    if (!pgd[pgd_i]) {
        pgd[pgd_i] = boot_mm_alloc(PAGE_SIZE);
        boot_memset(pgd[pgd_i], 0, PAGE_SIZE);
        pgd[pgd_i] |= attr;
    }
    pud = pgd[pgd_i] & PAGE_MASK;
    if (!pud[pud_i]) {
        pud[pud_i] = boot_mm_alloc(PAGE_SIZE);
        boot_memset(pud[pud_i], 0, PAGE_SIZE);
        pud[pud_i] |= attr;
    }
    pmd = pud[pud_i] & PAGE_MASK;
    if (!pmd[pmd_i]) {
        pmd[pmd_i] = boot_mm_alloc(PAGE_SIZE);
        boot_memset(pmd[pmd_i], 0, PAGE_SIZE);
        pmd[pmd_i]|= attr;
    }
    pte = pmd[pmd_i] & PAGE_MASK;
    pte[pte_i] = pa | attr;
}

void boot_mm_pgtable_init(void)
{
    pgd_t *pgd;
    phys_addr_t kern_phys = 0;
    virt_addr_t kern_virt = KERNEL_BASE_ADDR;

    boot_kern_pgtable = boot_mm_alloc(PAGE_SIZE);
    boot_memset(boot_kern_pgtable, 0, PAGE_SIZE);

    /* map for read-only region */
    while (kern_virt < (uint64_t) (&__roseg_end)) {
        //boot_printstr("map kern virt 0x");boot_printhex(kern_virt);
        //boot_printstr(" to phys 0x");boot_printhex(kern_phys);boot_puts("");
        boot_pgtable_map(boot_kern_pgtable, kern_virt, kern_phys, PAGE_ATTR_P);
        kern_virt += PAGE_SIZE;
        kern_phys += PAGE_SIZE;
    }

    /* map for read-write region */
    while (kern_virt < (uint64_t) (&__kernel_end)) {
        boot_pgtable_map(boot_kern_pgtable, kern_virt, kern_phys, 
                        PAGE_ATTR_P | PAGE_ATTR_RW);
        kern_virt += PAGE_SIZE;
        kern_phys += PAGE_SIZE;
    }

    /* map for the 10MB direct mapping memory temporarily */
    for (kern_virt = KERNEL_DIRECT_MAPPING_AREA, kern_phys = 0;
        kern_phys < 0xa00000;
        kern_virt += PAGE_SIZE, kern_phys += PAGE_SIZE) {
        boot_pgtable_map(boot_kern_pgtable, kern_virt, kern_phys, 
                         PAGE_ATTR_P | PAGE_ATTR_RW);
    }

    /* reget the boot page table in it, we'll clear it in virt kernel stage */
    pgd = boot_kern_pgtable;
    pgd[0] = (phys_addr_t) (&boot_page_table_pud);
    pgd[0] |= PAGE_ATTR_P | PAGE_ATTR_RW;

    /* load the new page table now! */
    asm volatile(
        "mov    %0, %%rax;"
        "mov    %%rax, %%cr3;"
        :
        : "a" (boot_kern_pgtable)
    );

    boot_puts("\t[+] booting-kernel page table initialization done!");
}

void boot_mm_init(void)
{
    uint64_t kernel_size;

    kernel_size = ((uint64_t) &__kernel_end) - ((uint64_t) &__boot_start);
    kernel_size -= KERNEL_BASE_ADDR;

    boot_printstr("\n\t[*] kernel image phys mm start: 0x");
    boot_printhex(&__boot_start);
    boot_printstr(",  kernel image size: 0x");
    boot_printhex(kernel_size);
    boot_puts("");

    /* detect basic usable memory */
    boot_mm_detect();
    
    /* we want a PAGE-aligned allocation start there */
    boot_mem_alloc_start = \
        ALIGN(((uint64_t) &__boot_start) + kernel_size + PAGE_SIZE, PAGE_SIZE);
    boot_printstr("\t[*] booting memory allocate from 0x");
    boot_printhex(boot_mem_alloc_start);
    boot_puts("");

    /* now we start to allocate a page table for kernel space from highmem */
    boot_mm_pgtable_init();
}