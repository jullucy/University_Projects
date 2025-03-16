CC=gcc
CFLAGS=-Wall -Wextra -Wno-implicit-fallthrough -std=gnu17 -fPIC -O2
LDFLAGS=-shared -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=reallocarray -Wl,--wrap=free -Wl,--wrap=strdup -Wl,--wrap=strndup

.PHONY: all clean

nand.o: nand.c nand.h
	$(CC) $(CFLAGS) -c nand.c -o nand.o

memory_tests.o: memory_tests.c memory_tests.h
	$(CC) $(CFLAGS) -c memory_tests.c -o memory_tests.o

libnand.so: nand.o memory_tests.o
	$(CC) $(LDFLAGS) nand.o memory_tests.o -o libnand.so

nand_example.o: nand_example.c
	$(CC) $(CFLAGS) -c nand_example.c -o nand_example.o

nand_example: nand_example.o libnand.so
	$(CC) -o nand_example nand_example.o -L. -lnand -Wl,-rpath,.

clean:
	rm -f *.o *.so nand_example

