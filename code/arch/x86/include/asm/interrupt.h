#ifndef X86_INTERRUPT_H
#define X86_INTERRUPT_H

struct idt_register {
    unsigned short size;
    unsigned long address;
} __attribute__((packed));

static inline void load_idt(struct idt_register *idt)
{
    asm volatile("lidtq %0" :: "m"(*idt));
}

extern struct idt_register idtr;

#endif