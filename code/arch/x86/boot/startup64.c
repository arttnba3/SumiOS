#include <mm/page_types.h>
#include <boot/multiboot2.h>
#include <sumios/types.h>

extern void boot_putchar(char ch);
extern void boot_printstr(char *str);
extern void boot_puts(char *str);
extern void boot_set_char(uint8_t color, uint8_t val, uint8_t loc);
extern void boot_tty_init(void);
extern void main();
extern uint16_t *boot_text_mem;

void startup64(unsigned int magic, multiboot_uint8_t *mbi)
{
    /* this's just a temporary tty for booting stage */
    boot_tty_init();

    boot_puts("Sumi OS v0.0.1\n");

    boot_puts("[*] System setting up...");

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        boot_puts("BAD BOOTLOADER MAGIC!");
        return ;
    }

    for (int i = 0; i < 80000; i++) {
        boot_puts("Hello world!");
    }

    while (1) {
        
    }

    //main();
}