src_files = $(wildcard *.c)
obj_files = $(notdir $(patsubst %c, %o, $(src_files)))

all: process_files
	$(CC) $(CFLAGS) -c $(src_files) -I $(INCLUDE) -I $(ARCH_INCLUDE)
	mv $(obj_files) -t $(OUT)

process_files: process
	make -C process/