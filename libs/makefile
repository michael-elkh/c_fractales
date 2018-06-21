.PHONY: all clean

all : fractals.o matrix.o secured_alloc.o libpng.o

fractals.o: fractals.c fractals.h
	$(CFLAGS) -c fractals.c -lpthread -lm

libpng.o: libpng.c libpng.h
	$(CFLAGS) -c libpng.c -lm -lpng

matrix.o: matrix.c matrix.h
	$(CFLAGS) -c matrix.c

secured_alloc.o: secured_alloc.c secured_alloc.h
	$(CFLAGS) -c secured_alloc.c

clean:
	rm -f *.o