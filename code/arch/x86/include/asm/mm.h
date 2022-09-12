#ifndef X86_ASM_MM_H
#define X86_ASM_MM_H

#include <sumios/types.h>

/* Address Range Descriptor Structure for int 0xe820*/
struct e820_descriptor {
    uint64_t addr;
    uint64_t size;
    uint32_t type;
} __attribute__((packed));

#endif