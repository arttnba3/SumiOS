#include <asm/io.h>
#include <sumios/kernel.h>
#include <mm/init.h>
#include <boot/multiboot2.h>

extern void arch_init(void);
extern void task_init(void);

int main(multiboot_uint8_t *mbi)
{
    /* init the kernel tty */
    tty_init();

    kputs("\n\t[+] Successfully to enter the highmem kernel!\n");

    /* init memory management */
    kprintf("\t[*] mbi at %p\n", mbi);
    mm_init(mbi);
    kputs("\n\t[+] kernel memory initialization complete.");

    /* init for task schedule */
    task_init();
    kputs("\n\t[+] process schedule subsystem initialization complete.");

    /* arch related initialization */
    arch_init();
    kputs("\n\t[+] arch related initialization complete.");


    for (size_t i = 0; i < 1145141919; i++) {
        void *obj = kmalloc(0x10 << (i % 10));
        kprintf("[allocate time: %lu] get object at %p\n", i, obj);
    }

    while (1) {
        asm volatile ("hlt");
    }

}