#ifndef X86_CPU_H
#define X86_CPU_H

#include <sumios/types.h>

static inline size_t read_cr2(void)
{
    size_t cr2;

    asm volatile("mov %%cr2, %%rax;"
                 "mov %%rax, %0;"
                 : "=m"(cr2)
                 : );

    return cr2;
}

static inline size_t read_cr3(void)
{
    size_t cr3;

    asm volatile("mov %%cr3, %%rax;"
                 "mov %%rax, %0;"
                 : "=m"(cr3)
                 : );

    return cr3;
}

static inline void write_cr3(size_t cr3)
{
    asm volatile("mov %0, %%rax;"
                 "mov %%rax, %%cr3;"
                 :
                 : "r"(cr3));
}

#endif