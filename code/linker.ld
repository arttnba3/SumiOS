OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

SECTIONS
{
    /* boot loader will load the kernel there */
	KERM_VADDR = 0xffffffff80000000;
    . = 1M;
    .boot :
	{
		KEEP(*(.multiboot_header))
		*(.boot)
		*(.boot.*)
		. = ALIGN(4096);
	}

	. += KERM_VADDR;
	. = ALIGN(32768);

}