CC=gcc
MPICC=mpicc
CILKCC=/usr/local/OpenCilk-9.0.1-Linux/bin/clang
CFLAGS=-O3

default: all

triangle: 
	$(CC) $(CFLAGS) -o triangle triangle.c

triangle_v2: 
	$(CC) $(CFLAGS) -o triangle_v2 triangle_v2.c

example_read:
	$(CC) $(CFLAGS) -c mmio.c
	$(CC) $(CFLAGS) -c coo2csc.c
	$(CC) $(CFLAGS) -c example_read.c
	$(CC) $(CFLAGS) -o example mmio.c coo2csc.c example_read.c

all: clean triangle triangle_v2 example_read

.PHONY: clean

test:
	@printf "\n** Testing triangle\n\n"
	./triangle
	

clean:
	rm -f triangle triangle_v2 example_read.o example mmio.o coo2csc.o