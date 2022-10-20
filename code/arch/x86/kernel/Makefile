src_files = $(wildcard *.c)
obj_files = $(notdir $(patsubst %c, %o, $(src_files)))

all:
	$(CC) $(CFLAGS) -c $(src_files) -I $(INCLUDE) -I $(X86_INCLUDE)
	mv $(obj_files) -t $(ARCH_OUT)