#include <asm/io.h>
#include <sumios/kernel.h>

int main(void)
{
    /* init the kernel tty */
    tty_init();

    kputs("\n\t[+] Successfully to enter the highmem kernel!");

    /* init memory management */
        

    while (1) {
        asm volatile ("hlt");
    }

}