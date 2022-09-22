# SumiOS v0.0.1
# Author: arttnba3
# 2022.09

boot_dir = ./boot
X86_INCLUDE = $(shell pwd)/include/

export X86_INCLUDE

src_asm = $(wildcard *.S)
src_asm += $(wildcard $(boot_dir)/*.S)

src_c = $(wildcard *.c)
src_c += $(wildcard $(boot_dir)/*.c)

src_files = $(src_asm)
src_files += $(src_c)

obj_files = $(notdir $(patsubst %S, %o, $(src_asm)))
obj_files += $(notdir $(patsubst %c, %o, $(src_c)))

all: $(src_files) mm_files
	$(CC) $(CFLAGS) -c $(src_files) -I $(INCLUDE) -I $(X86_INCLUDE)
	mv $(obj_files) -t $(ARCH_OUT)

mm_files: mm
	make -C mm/
