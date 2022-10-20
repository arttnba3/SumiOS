#include <asm/desc.h>

const struct gdt_page gdt = {
    .gdt = {
        [GDT_KERNEL_CS]     = DESCRIPTOR_INIT(0, 0xFFFFF, 0xA09B),
        [GDT_KERNEL_DS]     = DESCRIPTOR_INIT(0, 0xFFFFF, 0xC093),
        [GDT_USER32_CS]     = DESCRIPTOR_INIT(0, 0xFFFFF, 0xC0FB),
        [GDT_USER_DS]       = DESCRIPTOR_INIT(0, 0xFFFFF, 0xC0F3),
        [GDT_USER_CS]       = DESCRIPTOR_INIT(0, 0xFFFFF, 0xA0FB),
    },
};

struct gdt_register gdtr;
