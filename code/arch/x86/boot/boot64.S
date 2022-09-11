.extern startup64

.code64

.section .boot.text
    .globl long_mode_entry
        /* now we're under long mode! */
    long_mode_entry:
        mov $(2 << 3), %ax
        mov %ax, %ds
        mov %ax, %ss
        mov %ax, %es
        mov %ax, %fs
        mov %ax, %gs

        /* print something */
        movw     $0x0F61, 0xb8010
        movw     $0x0F72, 0xb8012
        movw     $0x0F74, 0xb8014
        movw     $0x0F74, 0xb8016
        movw     $0x0F6e, 0xb8018
        movw     $0x0F62, 0xb801a
        movw     $0x0F61, 0xb801c
        movw     $0x0F33, 0xb801e

        call    startup64
    
    .loop:
        hlt
        jmp .loop