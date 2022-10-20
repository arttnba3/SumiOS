#ifndef X86_INTERRUPT_H
#define X86_INTERRUPT_H

#include <asm/gate.h>
#include <mm/page_types.h>

#define IDT_ENTRIES     32

struct interrupt_descriptor_table {
    struct gate_desc idt[IDT_ENTRIES];
} __attribute__((aligned(PAGE_SIZE)));

struct idt_register {
    unsigned short size;
    unsigned long address;
} __attribute__((packed));

static inline void load_idt(struct idt_register *idt)
{
    asm volatile("lidtq %0" :: "m"(*idt));
}

extern struct interrupt_descriptor_table idt;
extern struct idt_register idtr;

extern void page_fault_handler(void);

#endif