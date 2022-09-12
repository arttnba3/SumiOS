#include <asm/io.h>

char tmp[100] = { 0 };

int main(void)
{
    inb(0x15);
    while (1) {
        ;
    }
    
}