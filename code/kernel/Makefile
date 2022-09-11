src_files = $(wildcard *.c)
obj_files = $(notdir $(patsubst %c, %o, $(src_files)))

all:
	$(CC) $(CFLAGS) -c $(src_files) -I $(INCLUDE)
	mv $(obj_files) -t $(OUT)