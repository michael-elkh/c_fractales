.PHONY: all clean

all : fractals.o matrix.o secured_alloc.o

fractals.o: fractals.c fractals.h
	$(CFLAGS) -c fractals.c -lpthread -lm

matrix.o: matrix.c matrix.h
	$(CFLAGS) -c matrix.c -lm -lpng

secured_alloc.o: secured_alloc.c secured_alloc.h
	$(CFLAGS) -c secured_alloc.c

clean:
	rm -f *.o