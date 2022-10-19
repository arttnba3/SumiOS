#include <sumios/kernel.h>
#include <mm/types.h>
#include <mm/layout.h>
#include <asm/io.h>
#include <asm/desc.h>

static const struct gdt_page gdt = {
    .gdt = {
        [GDT_KERNEL_CS]     = DESCRIPTOR_INIT(0, 0xFFFFF, 0xA09B),
        [GDT_KERNEL_DS]     = DESCRIPTOR_INIT(0, 0xFFFFF, 0xC093),
        [GDT_USER32_CS]     = DESCRIPTOR_INIT(0, 0xFFFFF, 0xC0FB),
        [GDT_USER_DS]       = DESCRIPTOR_INIT(0, 0xFFFFF, 0xC0F3),
        [GDT_USER_CS]       = DESCRIPTOR_INIT(0, 0xFFFFF, 0xA0FB),
    },
};

static struct gdt_register gdtr;

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
    static const uint64_t interrupt_descriptor_table[] = {

    };
    phys_addr_t idt_addr = &interrupt_descriptor_table;

    asm volatile("lidtq %0" :: "m"(idt_addr));
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