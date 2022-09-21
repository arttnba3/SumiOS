# SumiOS v0.0.1
# Author: arttnba3
# 2022.09

CC	:= $(GCCPREFIX)gcc -pipe
AS	:= $(GCCPREFIX)as
AR	:= $(GCCPREFIX)ar
LD	:= $(GCCPREFIX)ld
CFLAGS := -ffreestanding -nostdlib -fno-pie -fno-stack-protector -mcmodel=large
LDFLAGS := -nostdlib -z max-page-size=0x1000

export CC
export AS
export AR
export LD
export CFLAGS

INCLUDE := $(shell pwd)/include/
ARCH_INCLUDE := $(shell pwd)/arch/x86/include/
OUT := $(shell pwd)/out/
ARCH_OUT := $(shell pwd)/out/arch/

export INCLUDE
export ARCH_INCLUDE
export OUT
export ARCH_OUT

all: prepare kernel.bin

prepare:
	mkdir -p $(OUT)
	mkdir -p $(ARCH_OUT)

kernel.bin: arch_file kernel_file mm_file lib_file
	$(LD) -T linker.ld -o $(OUT)/$@ $(shell find $(OUT) -name *.o)

arch_file:
	make -C arch/

kernel_file:
	make -C kernel/

mm_file:
	make -C mm/

lib_file:
	make -C lib/

qemu: kernel.bin

clean:
	rm -rf ./out

.PHONY: all