#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "mmio.h"

void print1DMatrix(int* matrix, int size){
    int i = 0;
    for(i = 0; i < size; i++){
        printf("%d: %d \n",i, matrix[i]);
    }
}

void readfile_stepone(
    FILE *f,
    uint32_t *m, 
    uint32_t *n, 
    uint32_t *nnz_pointer,
    int *i, 
    int *j,
    int binary    
    )
    {

    int ret_code;
    MM_typecode matcode;
    
    if (mm_read_banner(f, &matcode) != 0)
    {
        printf("Could not process Matrix Market banner.\n");
        exit(1);
    }


    /*  This is how one can screen matrix types if their application */
    /*  only supports a subset of the Matrix Market data types.      */

    if (mm_is_complex(matcode) && mm_is_matrix(matcode) && 
            mm_is_sparse(matcode) )
    {
        printf("Sorry, this application does not support ");
        printf("Market Market type: [%s]\n", mm_typecode_to_str(matcode));
        exit(1);
    }

    /* find out size of sparse matrix .... */

    if ((ret_code = mm_read_mtx_crd_size(f, m, n, nnz_pointer)) !=0)
        exit(1);
}

void readfile_steptwo(
    FILE *f,
    uint32_t *m, 
    uint32_t *n, 
    uint32_t *nnz_pointer,
    int *I, 
    int *J,
    double * val,
    int binary    
    )
    {

    uint32_t M = *m;
    uint32_t N = *n;
    uint32_t nnz = *nnz_pointer;
    
    /* Depending on the second argument of the main call our original matrix may be binary or non binary so we read the file accordingly */
    switch (binary) 
    {
    case 0:
        /* use this if the source file is not binary */
        for (uint32_t i=0; i<nnz; i++)
        {
            /* I is for the rows and J for the columns */
            fscanf(f, "%d %d %lg\n", &I[i], &J[i], &val[i]);
            I[i]--;  /* adjust from 1-based to 0-based */
            J[i]--;
        }
        break;

    case 1:
        /* use this if the source file is binary */
        for (uint32_t i=0; i<nnz; i++)
        {
            /* I is for the rows and J for the columns */
            fscanf(f, "%d %d \n", &I[i], &J[i]);
            I[i]--;  /* adjust from 1-based to 0-based */
            J[i]--;
        }
        break;
    
    default:
        printf("Not a valid second argument was passed\n");
        exit(1);
        break;
    }

    if (f !=stdin) fclose(f);

    if(M != N) {
        printf("COO matrix' columns and rows are not the same");
    }
}