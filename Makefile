CC=gcc
MPICC=mpicc
CILKCC=/usr/local/OpenCilk-9.0.1-Linux/bin/clang
CFLAGS=-O3

default: all

triangle: 
	$(CC) $(CFLAGS) -o triangle triangle.c

triangle_v2: 
	$(CC) $(CFLAGS) -o triangle_v2 triangle_v2.c


all: triangle triangle_v2

.PHONY: clean

test:
	@printf "\n** Testing triangle\n\n"
	./triangle
	

clean:
	rm -f triangle triangle_v2