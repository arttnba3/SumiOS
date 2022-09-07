#include <mm/page_types.h>

#define MULTIBOOT2_HEADER_MAGIC (0xe85250d6)
#define GRUB_MULTIBOOT_ARCHITECTURE_I386 (0)
#define HULTIBOOT2_HEADER_LENGTH (multiboot_header_end - multiboot_header)
#define SUMIOS_CHECKSUM \
    -(MULTIBOOT2_HEADER_MAGIC \
    + GRUB_MULTIBOOT_ARCHITECTURE_I386 \
    + HULTIBOOT2_HEADER_LENGTH)
#define MULTIBOOT2_HEADER_ALIGN 8

.extern startup_64

.code32

.section .boot.text:

    .globl start, _start
    .align MULTIBOOT2_HEADER_ALIGN

    multiboot_header:
        .long   MULTIBOOT2_HEADER_MAGIC
        .long   GRUB_MULTIBOOT_ARCHITECTURE_I386
        .long   HULTIBOOT2_HEADER_LENGTH
        .long   SUMIOS_CHECKSUM
    
    tags_end:
        .short   0
        .short   0
        .long   8
    multiboot_header_end:

    start:
    _start:
        /* turn off the interrupt */
        cli

        /* init the stack */
        mov     $(boot_stack_top), %esp

        /* pointer to the MULTIBOOT2 information structure */
        pushl   %ebx
        /* magic number */
        pushl   %eax

        /* print something */
        movw     $0x2F61, 0xb8000
        movw     $0x2F72, 0xb8002
        movw     $0x2F74, 0xb8004
        movw     $0x2F74, 0xb8006
        movw     $0x2F6e, 0xb8008
        movw     $0x2F62, 0xb800a
        movw     $0x2F61, 0xb800c
        movw     $0x2F33, 0xb800e

        //call    startup32
    
    .loop:
        hlt
        jmp     .loop

.section .boot.data:

    .globl boot_stack, boot_stack_top
    /**
     * When the system is just started, there's no a valid stack existed.
     * So we reserve a page there as a temporary stack for booting
     */
    boot_stack:
        .space PAGE_SIZE
    boot_stack_top: