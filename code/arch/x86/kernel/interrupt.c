#include <asm/interrupt.h>
#include <asm/gate.h>
#include <asm/trap.h>
#include <asm/desc.h>
#include <sumios/kernel.h>

struct interrupt_descriptor_table idt;
struct idt_register idtr;

void set_intr_handler(int nr, void (*handler)(void))
{
    int selector = DEFAULT_KERNEL_CS_SELECTOR;
    struct gate_desc desc = KERNEL_INTR_GATE_INIT((size_t) handler, selector);
    
    memcpy(&idt.idt[nr], &desc, sizeof(struct gate_desc));
}

void page_fault_handler(void)
{
    kputs("kernel page fault detected!");
    asm volatile(" hlt ");
}
