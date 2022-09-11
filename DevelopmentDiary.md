## Sep 8, 2022

Project Started.

Compared to those 32-bit toys(i.e. xv6 and [elephant os](https://github.com/arttnba3/OS_Kernel_Learning)), it seems more complex and difficult for 64-bit OS kernel development.

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

- Fix output problem.
- Complete serial operation for text-mode display.