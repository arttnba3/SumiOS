#!/bin/sh
cd code/
make || exit
cd ..
cp code/out/kernel.bin targets/x86_64/iso/boot/kernel.bin
grub-mkrescue /usr/lib/grub/i386-pc -o kernel.iso targets/x86_64/iso
