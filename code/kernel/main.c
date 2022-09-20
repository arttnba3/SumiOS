#include <asm/io.h>
#include <sumios/kernel.h>
#include <mm/init.h>
#include <boot/multiboot2.h>

int main(multiboot_uint8_t *mbi)
{
    /* init the kernel tty */
    tty_init();

    kputs("\n\t[+] Successfully to enter the highmem kernel!\n");

    /* init memory management */
    mm_init(mbi);
    kputs("\n\t[+] kernel memory initialization complete.");

    while (1) {
        asm volatile ("hlt");
    }

}