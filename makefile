#Debugger
CFLAGS := gcc -g -std=c11 -Wall -Wextra -fsanitize=address -fsanitize=undefined -fsanitize=leak
#Quick
#CFLAGS := gcc -std=c11 -Ofast

export CFLAGS

SUBDIRS = libs
.PHONY: clean $(SUBDIRS)

#Instructions
fractals: main.o libs
	$(CFLAGS) main.o libs/matrix.o libs/secured_alloc.o libs/fractals.o -lpthread -lpng -lm -o fractals
	
main.o: main.c
	$(CFLAGS) -c main.c -lm

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	@echo "---Cleaning---"
	rm -f *.o fractals

	@for subdir in $(SUBDIRS) ; do \
		$(MAKE) -C $$subdir clean ; \
	done
