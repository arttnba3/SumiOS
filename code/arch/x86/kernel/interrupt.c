#include <asm/interrupt.h>
#include <asm/gate.h>
#include <asm/trap.h>
#include <asm/desc.h>
#include <asm/cpu.h>
#include <sumios/kernel.h>

struct interrupt_descriptor_table idt;
struct idt_register idtr;

void set_intr_handler(int nr, void (*handler)(void))
{
    int selector = DEFAULT_KERNEL_CS_SELECTOR;
    struct gate_desc desc = KERNEL_INTR_GATE_INIT((size_t) handler, selector);
    
    memcpy(&idt.idt[nr], &desc, sizeof(struct gate_desc));
}

void set_trap_handler(int nr, void (*handler)(void))
{
    int selector = DEFAULT_KERNEL_CS_SELECTOR;
    struct gate_desc desc = KERNEL_TRAP_GATE_INIT((size_t) handler, selector);
    
    memcpy(&idt.idt[nr], &desc, sizeof(struct gate_desc));
}

void page_fault_handler(void)
{
    kprintf("[!] kernel page fault detected at addr: %p\n", read_cr2());
    kputs("[x] NO HANDLER NOW! hlting...");
    asm volatile(" hlt ");
}

void null_intr_handler(void)
{
    kputs("[x] NO handler set for CURRENT interrupt! hlting...");
    asm volatile(" hlt ");
}
