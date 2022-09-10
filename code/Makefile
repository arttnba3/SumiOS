# SumiOS v0.0.1
# Author: arttnba3
# 2022.09

CC	:= $(GCCPREFIX)gcc -pipe
AS	:= $(GCCPREFIX)as
AR	:= $(GCCPREFIX)ar
LD	:= $(GCCPREFIX)ld
CFLAGS := -ffreestanding -nostdlib -fno-pie -fno-stack-protector
LDFLAGS := -nostdlib -z max-page-size=0x1000

export CC
export AS
export AR
export LD
export CFLAGS

INCLUDE := $(shell pwd)/include/
OUT := $(shell pwd)/out/

export INCLUDE
export OUT

all: prepare kernel.bin

prepare:
	mkdir -p $(OUT)

kernel.bin: arch_file
	$(LD) -T linker.ld -o $(OUT)/$@ $(shell find $(OUT) -name *.o)

arch_file:
	make -C arch/

qemu: kernel

clean:
	rm -rf ./out

.PHONY: all