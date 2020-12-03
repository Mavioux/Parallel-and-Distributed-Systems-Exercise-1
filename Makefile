CC=gcc
MPICC=mpicc
CILKCC=/usr/local/OpenCilk-9.0.1-Linux/bin/clang
CFLAGS=-O3
PTHREADSFLAGS = -O2 -pthread

default: all

triangle: triangle.c
	$(CC) $(CFLAGS) -o triangle triangle.c

triangle_v2: triangle_v2.c
	$(CC) $(CFLAGS) -o triangle_v2 triangle_v2.c

triangle_v3: mmio.o coo2csc.o triangle_v3.o 
	$(CC) $(CFLAGS) -o triangle_v3 mmio.c coo2csc.c triangle_v3.c

triangle_v3_cilk: mmio.o coo2csc.o triangle_v3_cilk.c
	$(CILKCC) $(CFLAGS) -o triangle_v3_cilk mmio.c coo2csc.c triangle_v3_cilk.c -fcilkplus -lm

triangle_v3_openmp: mmio.o coo2csc.o triangle_v3_openmp.c
	$(CC) $(CFLAGS) -o triangle_v3_openmp mmio.c coo2csc.c triangle_v3_openmp.c -fopenmp

triangle_v4: mmio.o coo2csc.o triangle_v4.o 
	$(CC) $(CFLAGS) -o triangle_v4 mmio.c coo2csc.c triangle_v4.c

triangle_v4_1: mmio.o coo2csc.o triangle_v4_1.o 
	$(CC) $(CFLAGS) -o triangle_v4_1 mmio.c coo2csc.c triangle_v4_1.c

triangle_v4_cilk: mmio.o coo2csc.o triangle_v4_cilk.c
	$(CILKCC) $(CFLAGS) -o triangle_v4_cilk mmio.c coo2csc.c triangle_v4_cilk.c -fcilkplus

triangle_v4_openmp: mmio.o coo2csc.o triangle_v4_openmp.c
	$(CC) $(CFLAGS) -o triangle_v4_openmp mmio.c coo2csc.c triangle_v4_openmp.c -fopenmp

# triangle_v4_pthreads: mmio.o coo2csc.o triangle_v4_pthreads.c
# 	$(CC) $(CFLAGS) -Wall -g -pthread -o triangle_v4_pthreads mmio.c coo2csc.c triangle_v4_pthreads.c -lpthread -lm

triangle_v4_pthreads: mmio.o coo2csc.o triangle_v4_pthreads.c
	$(CC) $(PTHREADSFLAGS) -o triangle_v4_pthreads mmio.c coo2csc.c triangle_v4_pthreads.c

test : test.c
	$(CC) $(PTHREADSFLAGS) -o test test.c -lpthread

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: triangle triangle_v2 triangle_v3 triangle_v3_cilk triangle_v3_openmp triangle_v4 triangle_v4_1 triangle_v4_cilk triangle_v4_openmp triangle_v4_pthreads

.PHONY: clean
	

clean:
	rm -f triangle triangle_v2 triangle_v3_cilk triangle_v3_openmp triangle_v3.o example mmio.o coo2csc.o 