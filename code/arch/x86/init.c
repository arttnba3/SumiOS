#include <sumios/kernel.h>
#include <mm/types.h>
#include <mm/layout.h>
#include <asm/io.h>
#include <asm/desc.h>
#include <asm/interrupt.h>

/**
 * We just used a temporary gdt at the booting stage, which only contains
 * a kernel code descriptor and kernel data descriptor. 
 * Now we're going to use a more complete one.
 */
static void gdt_init(void)
{
    gdtr.address = &gdt;
    gdtr.size = GDT_SIZE;
    load_gdt(&gdtr);
}

static void idt_init(void)
{
    idtr.address = &idt;
    idtr.size = sizeof(idt);
    load_idt(&idtr);
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

    int n = *((int*)(0xdeadbeef));

    /* set the clock */
    clock_init();

    /* now we have done everything! turn on the irq for current core now */
    asm volatile (" cli; ");

    /* init for other cores */
    cpu_init();
}