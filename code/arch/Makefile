ARCH_DIR := ''

# FIXME: fix judgement of architecture
# I can't get the arch by $(ARCH), but I don't know why...
ifeq ($(ARCH),x86)
	ifeq ($(shell uname -m),x86_64)
		ARCH_DIR += x86
	else
		# do nothing
	endif
else
	# do nothing
endif

all:
ifeq ($(ARCH_DIR), '')
	make -C x86/
else
	@echo "unsupported arch"
	@false
endif