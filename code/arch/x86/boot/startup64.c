#include <mm/page_types.h>
#include <mm/layout.h>
#include <boot/multiboot2.h>
#include <sumios/types.h>
#include <asm/mm.h>

extern void boot_putchar(char ch);
extern void boot_printstr(char *str);
extern void boot_puts(char *str);
extern void boot_tty_init(void);
extern void boot_printnum(int64_t n);
extern void boot_printhex(uint64_t n);
extern void boot_mm_init(void);
extern void main();

void startup64(unsigned int magic, multiboot_uint8_t *mbi)
{
    /* this's just a temporary tty for booting stage */
    boot_tty_init();

    boot_puts("Welcome to Sumi OS v0.0.1\n");

    boot_puts("System setting up...\n");

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        boot_puts("[x] BAD BOOTLOADER MAGIC!");
        return ;
    }

    boot_printstr("\t[+] multiboot2 magic detected: 0x");
    boot_printhex(magic);

    /* kernel memory initialization */
    boot_mm_init();

    while (1) {
        
    }

    main();
}