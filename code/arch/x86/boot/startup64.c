#include <mm/page_types.h>
#include <boot/multiboot2.h>
#include <sumios/types.h>

uint16_t *tm = (uint16_t*) 0xB8000;

extern void putchar(char ch);
extern void printstr(char *str);
extern void puts(char *str);
extern void set_char(uint8_t color, uint8_t val, uint8_t loc);

void startup64(unsigned int magic, multiboot_uint8_t *mbi)
{
    //puts("Welcome to Sumi OS!");



    while (1) {
            tm[100] = 0x2F61;
    }
}