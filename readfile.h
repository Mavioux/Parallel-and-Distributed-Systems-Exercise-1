#ifndef READFILE_H
#define READFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void readfile_stepone(
    FILE *f,
    uint32_t *m, 
    uint32_t *n, 
    uint32_t *nnz_pointer,
    int *i, 
    int *j,
    int binary    
    );

void readfile_steptwo(
    FILE *f,
    uint32_t *m, 
    uint32_t *n, 
    uint32_t *nnz_pointer,
    int *I, 
    int *J,
    double * val,
    int binary    
    );

void print1DMatrix(int* matrix, int size);

#endif