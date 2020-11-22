CC=gcc
MPICC=mpicc
CILKCC=/usr/local/OpenCilk-9.0.1-Linux/bin/clang
CFLAGS=-O3

default: all

triangle: triangle.c
	$(CC) $(CFLAGS) -o triangle triangle.c

triangle_v2: triangle_v2.c
	$(CC) $(CFLAGS) -o triangle_v2 triangle_v2.c

triangle_v3: mmio.o coo2csc.o triangle_v3.o 
	$(CC) $(CFLAGS) -o triangle_v3 mmio.c coo2csc.c triangle_v3.c

triangle_v3_cilk: mmio.o coo2csc.o 
	$(CC) $(CFLAGS) -o triangle_v3_cilk mmio.c coo2csc.c triangle_v3_cilk.c -fcilkplus -fsanitize=cilk

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: triangle triangle_v2 triangle_v3 triangle_v3_cilk

.PHONY: clean

test:
	@printf "\n** Testing triangle\n\n"
	./triangle
	

clean:
	rm -f triangle triangle_v2 riangle_v3 triangle_v3_cilk triangle_v3.o example mmio.o coo2csc.o