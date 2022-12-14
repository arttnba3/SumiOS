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

Now the basic part of memory management has finished. For next it'll be the allocator: `buddy` and `slub`, the most beautiful algorithm I have ever seen.

- fix bugs of macro `PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR()`
- add basic linear physical memory allocator
- add basic function of memory mapping

## Sep 26, 2022

The memory management has taken more time than I'd ever thought.

## Sep 27, 2022

The memory management subsystem is much more complex than I have ecer thought. I have just done a basic model now and it doesn't work correctly so far, I need to fix it later.

And I tried to complete something related to specific ISA today, and it took me lots of time. So I decided to complete it step by step.

- add basic structure for ISA related initialization.
- add basic structure for buddy system.

## Sep 28, 2022

It's been busy again recently. Fortunately I have completed the basic buddy system and fixed existed bugs today and it seems that everything works well. There's no doubt that some bugs appeared but finally they all got fixed.

And I add the basic implementation of spin lock, with the basic implementation of atomic operations. I used to think of implementing it with atomic assembly code like `lock;`, but I found that the gcc provided useful inner functions like `__sync_bool_compare_and_swap`. So I just use them to implement the atomic operations.

One more thing is that I forgot to migrate the `boot_stack` to high mem, but it seems that nothing happen even if I have unmapped the start entries of kernel page table. I don't know why it didn't crash while accessing on an invalid stack, but it's better to fix it before it really effects.

- fix bugs in buddy system
- add basic atomic operations
- migrate `boot_stack` to highmem

## Oct 20, 2022

I've almost forgot there's an uncompleted kernel here because of continuously coming shits to deal with. So it's been a long while until I made a new commit for it.

I'm still struglling against the part that related to the architecture, only x86-64 is in a half-completed stage. I wish to add supports for more ISA in the future.

The part of handling interrupt cost me plenty of time. The system developed by Intel is much more burdensome than I've ever thought. So it seems that the interrupt handler needs more time to be completed.

- add gdt initialization
- add basic structure of gates

## Oct 21, 2022

The IDT and gates seem to work correctly, so the next thing for me to do is to complete handlers for traps and exceptions.

One more thing is that as everything seems to be working well, I need to add more subsystems to complete the kernel. There's only one buddy system in memory management now, so my next step is to build a slub allocator.

An unconscious bug in the `mm_phys_alloc()` was found when I re-check the code in `page_alloc.c`. Luckily the function hadn't been used yet so the bug didn't cause severe disasters.

- fix bug in `mm_phys_alloc()`

## Nov 28, 2022

It's been a long while later and I've achieved lying down on the bed for a month :)

In fact I'm not so busy but just lazy recently, so I decided to relax for several days and it gradually become weeks. But when I tried to restart the project weeks ago, my disk(PM981A, Samsung SSD) broke down unexpectedly and all my system data lost. Luckily all my data is stored in another disk physically including my Ubuntu VM for development.

Today I'd like to complete part of slub allocator because dynamic memory allocation is the most basic part of the kernel.

- add `kmem_cache` array and basic part of `kmalloc()`

## Nov 29, 2022

The slub allocator seen to be more easier to complete than I've expected though I've almost forgot what the members mean in `struct page` and some other structures. But when I thought that I had made the allocating part perfect, it threw a `TRAP_GP` (General Protection) to my face and I don't know why.

But where there's a road, there's a way. After uncountable times of reparing, I finally found that the best solution for me is to rewrite the core function and hope it can work well once again. And I'll do it SOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOON...

- start rewriting `slub_alloc()`

## Nov 30

The memory allocation part is almost down and it seems that there's no bugs for that after my hard coding for the whole day(in fact some bugs appeared and stuck me for a long while but finally got solved).

So it comes to the part of `kfree()`, which is more complex than the `kmalloc()`.

- complete `kmalloc()`

