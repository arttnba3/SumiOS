#include <mm/page_types.h>
#include <boot/multiboot2.h>
#include <sumios/types.h>

extern void putchar(char ch);
extern void printstr(char *str);
extern void puts(char *str);
extern void clear_screen(void);
extern void main();

void startup64(unsigned int magic, multiboot_uint8_t *mbi)
{
    clear_screen();
    puts("Sumi OS v0.0.1\n");

    puts("[*] System setting up...");

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        puts("BAD BOOTLOADER MAGIC!");
        return ;
    }

    for (int i = 0; i < 15; i++) {
        puts("Hello world!");
    }

    for (int i = 0; i < 10; i++) {
        puts("Test for roll screen!");
    }

    while (1) {
        
    }
    //main();
}