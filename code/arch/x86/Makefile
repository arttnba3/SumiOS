# SumiOS v0.0.1
# Author: arttnba3
# 2022.09

target_files := $(shell find . -name *.S)
#target_files += $(shell find . -name *.c)

all:
	$(CC) $(CFLAGS) -o boot.o -c $(target_files) -I $(INCLUDE)
	mv boot.o $(OUT)