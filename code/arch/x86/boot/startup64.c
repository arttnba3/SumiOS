#include <mm/page_types.h>
#include <boot/multiboot2.h>
#include <sumios/types.h>

extern void boot_putchar(char ch);
extern void boot_printstr(char *str);
extern void boot_puts(char *str);
extern void boot_tty_init(void);
extern void main();

void startup64(unsigned int magic, multiboot_uint8_t *mbi)
{
    /* this's just a temporary tty for booting stage */
    boot_tty_init();

    boot_puts("Welcome to Sumi OS v0.0.1\n");

    boot_puts("[*] System setting up...");

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        boot_puts("BAD BOOTLOADER MAGIC!");
        return ;
    }

    while (1) {
        boot_puts("Hello world!");
        boot_puts("This is Sumi OS v0.01!");
    }

    //main();
}