#include <sumios/types.h>
#include <mm/layout.h>
#include <mm/page_types.h>
#include <asm/mm.h>
#include <asm/io.h>

extern void boot_putchar(char ch);
extern void boot_printstr(char *str);
extern void boot_puts(char *str);
extern void boot_printnum(int64_t n);
extern void boot_printhex(uint64_t n);

extern uint64_t __boot_start, *__kernel_end;

uint64_t mem_total;

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
        mem_total = 16 * 1024 + mem_ext16;
    } else if (mem_ext) {
        mem_total = 1 * 1024 + mem_ext;
    } else {
        mem_total = mem_base;
    }

    boot_puts("\n\t[+] boot memory deteced done. ");
    boot_printstr("\t[*] total: 0x");
    boot_printhex(mem_total);
    boot_printstr("KB available, base = ");
    boot_printhex(mem_base);
    boot_printstr("KB, extend = 0x");
    boot_printhex(mem_total - mem_base);
    boot_puts("KB\n");
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
    

}