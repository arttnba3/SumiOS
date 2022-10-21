#include <sumios/kernel.h>
#include <mm/types.h>
#include <mm/layout.h>
#include <asm/io.h>
#include <asm/desc.h>
#include <asm/interrupt.h>
#include <asm/trap.h>

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
    /* set handlers for each interrupts */
    set_intr_handler(TRAP_DE, null_intr_handler);
    set_intr_handler(TRAP_DB, null_intr_handler);
    set_intr_handler(TRAP_NMI, null_intr_handler);
    set_intr_handler(TRAP_BP, null_intr_handler);
    set_intr_handler(TRAP_OF, null_intr_handler);
    set_intr_handler(TRAP_BR, null_intr_handler);
    set_intr_handler(TRAP_UD, null_intr_handler);
    set_intr_handler(TRAP_NM, null_intr_handler);
    set_intr_handler(TRAP_DF, null_intr_handler);
    set_intr_handler(TRAP_OLD_MF, null_intr_handler);
    set_intr_handler(TRAP_TS, null_intr_handler);
    set_intr_handler(TRAP_NP, null_intr_handler);
    set_intr_handler(TRAP_SS, null_intr_handler);
    set_intr_handler(TRAP_GP, null_intr_handler);
    set_intr_handler(TRAP_PF, page_fault_handler);
    set_intr_handler(TRAP_NON_1, null_intr_handler);
    set_intr_handler(TRAP_MF, null_intr_handler);
    set_intr_handler(TRAP_AC, null_intr_handler);
    set_intr_handler(TRAP_MC, null_intr_handler);
    set_intr_handler(TRAP_XM, null_intr_handler);
    set_intr_handler(TRAP_VE, null_intr_handler);
    set_intr_handler(TRAP_CP, null_intr_handler);

    /* load the IDTR */
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

    /* set the clock */
    clock_init();

    /* now we have done everything! turn on the irq for current core now */
    asm volatile (" cli; ");

    /* init for other cores */
    cpu_init();
}