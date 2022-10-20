#ifndef X86_GATE_H
#define X86_GATE_H

#include <sumios/types.h>
#include <asm/desc.h>

/**
 * @brief 64-bit gate descriptor
 * TODO: add 32-bit support in the future
 */
struct gate_desc {
    /* low 32 bit */
    u16 offset0;
    u16 selector;
    /* mid 32 bit */
    u16 ist: 3, zero: 5, type: 4, zero2: 1, dpl: 2, p: 1;
    u16 offset1;
    /* high 64 bit */
    u32 offset2;
    u32 reserved;
} __attribute__((packed));

#define KERNEL_INTR_GATE_INIT(handler, selector)        \
    {                                                   \
        .offset0    = (u16) (handler),                  \
        .offset1    = (u16) ((handler) >> 16),          \
        .offset2    = ((u64)(handler)) >> 32,           \
        .selector   = (u16) (selector),                 \
        .zero       = 0,                                \
        .zero2      = 0,                                \
        .type       = SEG_TYPE_SYS_80386_INTR_GATE,     \
        .dpl        = 0,                                \
        .p          = 1,                                \
    }

#define KERNEL_TRAP_GATE_INIT(handler, selector)        \
    {                                                   \
        .offset0    = (u16) (handler),                  \
        .offset1    = (u16) ((handler) >> 16),          \
        .offset2    = (handler) >> 32,                  \
        .selector   = selector,                         \
        .zero       = 0,                                \
        .zero2      = 0,                                \
        .type       = SEG_TYPE_SYS_80386_TRAP_GATE,     \
        .dpl        = 0,                                \
        .p          = 1,                                \
    }

#endif