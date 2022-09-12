OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

SECTIONS
{
    /* boot loader will load the kernel there */
	KERM_VADDR = 0xffffffff81000000;
    . = 1M;
	__boot_start = .;

    .boot :
	{
		KEEP(*(.multiboot_header))
		*(.boot.header)
		*(.boot.*)
		. = ALIGN(4096);

		/* C part boot kernel */
		*startup64.o
		*_boot.o
	}

	__boot_end = .;

	/* now it comes to the real kernel */
	. += KERM_VADDR;
	. = ALIGN(32768);

	/* we use AT() there to make it loaded on phys correctly */
	.text ALIGN(4096) : AT (ADDR (.text) - KERM_VADDR)
	{
		*(.text)
	}

	.rodata ALIGN(4096) : AT (ADDR (.rodata) - KERM_VADDR)
	{
		*(.rodata)
	}

	.data ALIGN(4096) : AT (ADDR (.data) - KERM_VADDR)
	{
		*(.data)
	}

	.bss ALIGN(4096) : AT (ADDR (.bss) - KERM_VADDR)
	{
		*(COMMON)
		*(.bss)
	}

	__kernel_end = .;
}