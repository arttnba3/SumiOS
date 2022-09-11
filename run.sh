#!/bin/sh
qemu-system-x86_64 \
    -cpu kvm64 \
    -smp cores=2,threads=2 \
    -cdrom kernel.iso \
    -m 1024M \
    -s

