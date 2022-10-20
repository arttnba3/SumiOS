#ifndef X86_DESC_H
#define X86_DESC_H

#include <sumios/types.h>
#include <mm/layout.h>
#include <mm/page_types.h>

#define SEGMENT_DESCRIPTOR_GRANULARITY_4K (1 << 23)
#define SEGMENT_DESCRIPTOR_GRANULARITY_1B (0 << 23)
#define SEGMENT_DESCRIPTOR_OPERAND_SIZE_32BIT (1 << 22)
#define SEGMENT_DESCRIPTOR_OPERAND_SIZE_16BIT (0 << 22)
#define SEGMENT_DESCRIPTOR_LONGMODE (1 << 21)
#define SEGMENT_DESCRIPTOR_PRESENT (1 << 15)
#define SEGMENT_DESCRIPTOR_PRIVILEDGE_LEVEL0 (0b00 << 13)
#define SEGMENT_DESCRIPTOR_PRIVILEDGE_LEVEL1 (0b01 << 13)
#define SEGMENT_DESCRIPTOR_PRIVILEDGE_LEVEL2 (0b10 << 13)
#define SEGMENT_DESCRIPTOR_PRIVILEDGE_LEVEL3 (0b11 << 13)
#define SEGMENT_DESCRIPTOR_SYSTEM_SEG (1 << 12)
#define SEGMENT_DESCRIPTOR_NON_SYSTEM_SEG (0 << 12)
#define SEGMENT_DESCRIPTOR_TYPE_SYS_AVAILABLE_80286_TSS (0b0001 << 8)
#define SEGMENT_DESCRIPTOR_TYPE_SYS_LDT (0b0010 << 8)
#define SEGMENT_DESCRIPTOR_TYPE_SYS_BUSY_80286_TSS (0b0011 << 8)
#define SEGMENT_DESCRIPTOR_TYPE_SYS_AVAILABLE_80386_TSS (0b1001 << 8)
#define SEGMENT_DESCRIPTOR_TYPE_SYS_BUSY_80386_TSS (0b1011 << 8)
#define SEGMENT_DESCRIPTOR_TYPE_SYS_80386_TSS (0b1011 << 8)

/* 8-byte segment descriptor */
struct descriptor {
    /* low 32 bit */
    u16 limit0; /* bit 15 ~ 0 */
    u16 base0;  /* bit 31 ~ 16 */
    /* high 32 bit */
    u16 base1: 8, type: 4, s: 1, dpl: 2, p: 1;  /* bit 15 ~ 0 */
    u16 limit1: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8; /* bit 31 ~ 16 */
} __attribute__((packed));

#define GDT_ENTRIES     32
#define GDT_SIZE    (GDT_ENTRIES * sizeof(struct descriptor))

struct desc_selector {
    u16 index: 13, ti: 1, rpl: 2;
};

struct gdt_page {
    struct descriptor gdt[GDT_ENTRIES];
} __attribute__((aligned(PAGE_SIZE)));

struct gdt_register {
    unsigned short size;
    unsigned long address;
} __attribute__((packed));

/* referred to Linux */
#define DESCRIPTOR_INIT(base, limit, flags) \
    {                                       \
        .limit0 = (u16) (limit),            \
        .limit1 = ((limit) >> 16) & 0xF,    \
        .base0  = (u16) (base),             \
        .base1  = ((base) >> 16) & 0xFF,    \
        .base2  = ((base) >> 24) & 0xFF,    \
        .type   = (flags) & 0xF,            \
        .s      = ((flags) >> 4) & 1,       \
        .dpl    = ((flags) >> 5) & 3,       \
        .p      = ((flags) >> 7) & 1,       \
        .avl    = ((flags) >> 12) & 1,      \
        .l      = ((flags) >> 13) & 1,      \
        .d      = ((flags) >> 14) & 1,      \
        .g      = ((flags) >> 15) & 1,      \
    }

#define DESC_SELECTOR_INIT(index, ti, rpl)  \
    {                                       \
        .index  = index,                    \
        .ti     = ti,                       \
        .rpl    = rpl,                      \
    }

#define GDT_KERNEL_CS   1
#define GDT_KERNEL_DS   2
#define GDT_USER32_CS   3
#define GDT_USER_DS     4
#define GDT_USER_CS     5

static inline void load_gdt(struct gdt_register *gdtr)
{
    asm volatile("lgdtq %0" :: "m"(*gdtr));
}

extern const struct gdt_page gdt;
extern struct gdt_register gdtr;

#endif