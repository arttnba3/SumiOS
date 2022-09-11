#include <mm/page_types.h>
#include <cpu.h>

#define MULTIBOOT2_HEADER_MAGIC (0xe85250d6)
#define GRUB_MULTIBOOT_ARCHITECTURE_I386 (0)
#define HULTIBOOT2_HEADER_LENGTH (multiboot_header_end - multiboot_header)
#define SUMIOS_CHECKSUM \
    -(MULTIBOOT2_HEADER_MAGIC \
    + GRUB_MULTIBOOT_ARCHITECTURE_I386 \
    + HULTIBOOT2_HEADER_LENGTH)
#define MULTIBOOT2_HEADER_ALIGN 8

.extern long_mode_entry

.section .boot.header
    .align MULTIBOOT2_HEADER_ALIGN

    multiboot_header:
        .long   MULTIBOOT2_HEADER_MAGIC
        .long   GRUB_MULTIBOOT_ARCHITECTURE_I386
        .long   HULTIBOOT2_HEADER_LENGTH
        .long   SUMIOS_CHECKSUM
    tags_end:
        .short  0
        .short  0
        .long   8
    multiboot_header_end:

.section .boot.text:
    .code32
    .align PAGE_SIZE

    .globl start, _start

    start:
    _start:
        /* 
         * turn off the interrupt,
         * and we should turn it on after the IDT has been built.
         */
        cli

        /* init the stack */
        mov     $boot_stack_top, %esp

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

        /* establish upper page table */
        mov     $boot_page_table_pud, %eax
        or      $(PAGE_ATTR_P | PAGE_ATTR_RW), %eax
        mov     %eax, boot_page_table_pgd

        mov     $boot_page_table_pmd, %eax
        or      $(PAGE_ATTR_P | PAGE_ATTR_RW), %eax
        mov     %eax, boot_page_table_pud

        /**
         * establish each page table
         * edx for each table, ebx for each PTE, 
         * ecx for each table count, eax for PT
         */
        mov     $0, %ebx
        or      $(PAGE_ATTR_P | PAGE_ATTR_RW), %ebx
        mov     $boot_page_table_pt, %edx
        mov     $boot_page_table_pmd, %eax

        .pt_loop:
        mov     $0, %ecx
        pushl   %edx
        or      $(PAGE_ATTR_P | PAGE_ATTR_RW), %edx
        mov     %edx, (%eax)
        popl    %edx

        .pte_loop:
        mov     %ebx, (%edx)
        add     $PAGE_SIZE, %ebx
        add     $8, %edx
        inc     %ecx
        cmp     $512, %ecx
        jne     .pte_loop

        add     $8, %eax
        cmp     $boot_page_table_end, %edx
        jne     .pt_loop

        mov     $boot_page_table_pgd, %eax
        mov     %eax, %cr3

        /* recover multiboot info */
        popl    %edi
        popl    %esi

        /* enable PAE */
        mov     %cr4, %eax
        or      $CR4_PAE, %eax
        mov     %eax, %cr4

        /* enable long mode */
        mov     $0xc0000080, %ecx
        rdmsr
        or      $(1 << 8), %eax
        wrmsr

        /* enable paging */
        mov     %cr0, %eax
        or      $CR0_PG, %eax
        mov     %eax, %cr0

        /* set up GDT */
        mov     $gdt64_ptr, %eax
        lgdt    0(%eax)

        /* enter the 64-bit long mode world! */
    .code64
        mov     $long_mode_entry, %rax
        call    %rax
    
    .loop:
        /* we should not and never arrive there... */
        hlt
        jmp     .loop

.section .boot.data:
    .align PAGE_SIZE

    .globl boot_stack, boot_stack_top

    /**
     * When the system is just started, there's no a valid stack existed.
     * So we reserve a page there as a temporary stack for booting
     */
    boot_stack:
        .space PAGE_SIZE * 2
    boot_stack_top:

    /**
     * space reversed for temporary paging before entering long mode.
     * We'll rebuild a page table dynamically in long mode.
     */
    .align PAGE_SIZE
    .globl boot_page_table_pgd, boot_page_table_pud
    .globl boot_page_table_pmd, boot_page_table_pt
    boot_page_table_pgd:
        .space PAGE_SIZE
    boot_page_table_pud:
        .space PAGE_SIZE
    boot_page_table_pmd:
        .space PAGE_SIZE
    boot_page_table_pt:
        .space PAGE_SIZE * 10
    boot_page_table_end:

.section .boot.rodata
    .align PAGE_SIZE
    .globl gdt64, gdt64_code_segment, gdt64_ptr

    /* global segment descriptor table */
    gdt64:
        .quad 0
    gdt64_code_segment:
        //.quad (1 << 43) | (1 < 44) | (1 << 47) | (1 << 53)
        .quad 0x00209A0000000000
    gdt64_data_segment:
        //.quad (1 << 43) | (1 < 44) | (1 << 47) | (1 << 53)
        .quad 0x0000920000000000
    gdt64_ptr:
        .short gdt64_ptr - gdt64 - 1
        .long gdt64

