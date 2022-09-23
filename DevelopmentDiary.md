## Sep 8, 2022

Project Started.

Compared to those 32-bit toys(e.g. xv6 and [elephant os](https://github.com/arttnba3/OS_Kernel_Learning)), it seems more complex and difficult for 64-bit OS kernel development.

Fortunately I found [OSDev wiki](https://wiki.osdev.org/Main_Page) and [this video](https://www.youtube.com/watch?v=FkrpUaGThTQ) besides [Multiboot2 Spec](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html), they helped me a lot at the beginning.

- Completed basic structure that satisfied [Multiboot2 Spec](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html).

## Sep 11, 2022

Many unexpected troubles appeared because of my carelessness when I tried to enter the long mode. Luckily all of them seem to be solved by my hard work and I successfully made my little `SumiOS` enter the long mode.

It seems that both `lretq` and `jmp` can set the cs to a correct val, but when I used `lretq` to make a return to enter 64-bit long mode, I cannot write the VGA text memory correctly to make some characters appear on the screen in `boot64.S`, what's more is that **it seems that it's the only difference and both of them can work correctly in the later C code.** I don't know the reason so I gave up and chose the `jmp`.

My sleep time had been crashed by this _useless project_ , but **I FEEL HAPPY ABOUT WRITING SUMIOS**.

- Add basic structure for outputing to screen under text mode.
- Enter 64-bit long mode under x86 Assembly.
- Enter 64-bit C world from x86 Assembly.

## Sep 12, 2022

An interger overflow was found in `boot_set_cursor_loc` after hours. The stupid problem almost made me lose the faith in computer science when I found the code **acted differently** between `fuction call` and `direct expansion of function code`. Fortunately the problem was solved again and my sleep got sucked too.

CGA display needs some special steps to operate the serial and some other shits to work correctly. I solved this problem referred to what xv6 did, because I don't want to waste too much time on it.

More shits occurred in front of me because of my carelessness and my lacks of basic CS knowledge. Hope that tomorrow will be well...

- Fix output problem.
- Complete serial operation for text-mode display.
- Complete memory detection by CMOS.

## Sep 13, 2022

I tried to add a basic and temporary memory allocator for booting stage to establish the real kernel page table, and after some silly mistakes it seems that my baby liner-memory allocator works well. But when I tried to load the new page table into cr3, it didn't work at all on the high mem.

- Add a basic memory allocator for booting stage.
- Add basic structure for highmem area(not work correctly yet).

## Sep 15, 2022

In fact I'm a little busy recently so I only solved a bug today. 

Do you still remenber that in last diary I found the page table didn't work on the high mem? At first I thought that I gave a wrong offset on some level or I made a misunderstanding about the 4-level page table's structure. But everything seems fucking ok, so I thought that the reason may be some long-mode features that I didn't know and I still found nothing about it after hours.

The point of solving this problem came at the time I tried to remap the start 2M like what I did in `boot.S` and it crashed as what used to appear on the high mem. It told me that the problem should be in `boot_get_pgtable_map_pa()` and I finally found out that I forgot to set the attributes for upper-level entries. So I quickly fixed it and successfully come to the high men area.

Next come to the most important part: `memory management`. I may take a long time to complete...

- Fix the bug in memory mapping in booting stage.

## Sep 22, 2022

It seems that I may have made some mistakes while initializing the kernel memory. So I worked hard today to solve those fucking bugs and continue to complete the memory management system.

The first mistake I made is that I tried to make a fully mapping of the whole physical memory on a specific virtual address space called `direct mapping area` (just like what Linux does), so I simply add this in `boot_mm_pgtable_init()` and forgot that it's not depending on the real memory information provided by the multiboot2 tags but the simple CMOS, which is not accurate. So I decided to just map a little part of memory in the `boot_mm_pgtable_init()` and make the full mapping later in `mm_init()` by the information from multiboot2 tags.

The second bug happened in `boot_pgtable_map()`, which works correctly for mapping the kernel images but works like a shit while mapping for direct mapping area, and the problem appeared unexpectedly because everything seemed okay when I finished the mapping of direct mapping area at home, and the shit hit me when I pull my code to my working laptop at my workspace. I didn't know why it happened because it works well while mapping for the kernel image, right? So I made a breakpoint and found out that it referred an invalid address while reading the page table. But the exact address it tried to read is a strange val, how and why? After I checked the code again and made sure that there's no OOB read or write, the reason come out: the page table hadn't been initialized properly because I wrote the `memset()` as `memset(uint8_t *dst, uint64_t sz, uint8_t val)` and used it as `memset(uint8_t *dst, uint8_t val, uint64_t sz)`. Fortunately the problem finally got found and fixed.

The third problem is that on my own computer there're several multiboot2 tags indicating variable memory areas, but on my working laptop it got no tags available. But I don't know why because I use the QEMU to run the kernel with the same arguments. So I check again and find out that the multiboot2 tags had been cleared by the fixed memset() because my `boot_mm_alloc()` allocates from the end of kernel image, and the GRUB put the multiboot2 tags on it, too. So I reserve a page for it and the multiboot2 tags appeared correctly again.

- fix bugs in memory initialization.
- fix bugs in memset().

## Sep 23, 2022

I'm always thinking about how to representing the whole memory in kernel space, and finally decided to create a `page` structure for each page frame, just like what Linux and XV6 does. And use a continuous array in virtual address space to store these `page` structures.

For storing these unexpected-count `page` structures, I choose to put then on a specific area starts at `0xffff800000000000` and allocate the memory dynamically by checking whether there's a mapping for the `page[i]` in page table. And I use a simple linear memory allocator `mm_phys_alloc_linear()` to allocate the page-aligned temporarily. It judges the free memory according to the multiboot2 tags.

There's no doubt that my code consists of shit, so an unexpected disaster appeared while initializing the pages struct. Fortunately I spent hours and finally locate it in the macro `PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR()`.

Now the basic part of memory management has finished. For next it'll be the allocator: `buddy` and `slub`, the best algorithm I have seen.

- fix bugs of macro `PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR()`
- add basic linear physical memory allocator
- add basic function of memory mapping