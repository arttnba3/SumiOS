#include <asm/cpu.h>

.extern startup64

.code64

.section .boot.text
    .globl long_mode_entry
        /* now we're under long mode! */
    long_mode_entry:
        /* all set to the data segment descriptor */
        mov $(2 << SELECTOR_INDEX), %ax
        mov %ax, %ds
        mov %ax, %ss
        mov %ax, %es
        mov %ax, %fs
        mov %ax, %gs

        call    startup64
    
    .loop:
        hlt
        jmp .loop