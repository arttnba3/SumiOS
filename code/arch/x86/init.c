#include <sumios/kernel.h>
#include <mm/types.h>
#include <asm/io.h>

/**
 * We just used a temporary gdt at the booting stage, which only contains
 * a kernel code descriptor and kernel data descriptor. 
 * Now we're going to use a more complete one.
 */
static void gdt_init(void)
{
    static const uint64_t global_descriptor_table[] = {

    };
    phys_addr_t gdt_addr;

    asm volatile("lgdtq %0" :: "m" (global_descriptor_table));
}

static void idt_init(void)
{
    
}

static void clock_init(void)
{

}

static void cpu_init(void)
{

}

void arch_init(void)
{
    /* reload a new gdt */
    gdt_init();

    /* load the idt */
    idt_init();

    /* set the clock */
    clock_init();

    /* now we have done everything! turn on the irq now */
    asm volatile (" cli; ");

    /* init for numa cpus */
    cpu_init();
}