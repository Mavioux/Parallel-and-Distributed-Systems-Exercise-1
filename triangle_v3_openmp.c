#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mmio.h"
#include "coo2csc.h"

#include <omp.h>

#define CHUNKSIZE   100

void print1DMatrix(int* matrix, int size){
    int i = 0;
    for(i = 0; i < size; i++){
        printf("%d: %d \n",i, matrix[i]);
    }
}

int main(int argc, char *argv[])
{
    int ret_code;
    MM_typecode matcode;
    FILE *f;
    uint32_t M, N, nnz;   
    int *I, *J;
    double *val;
    int binary = atoi(argv[2]);
    int num_of_threads = atoi(argv[3]);
    struct timeval start, end;

    if (argc < 2)
	{
		fprintf(stderr, "Usage: %s [martix-market-filename] [0 for non binary 1 for binary matrix]\n", argv[0]);
		exit(1);
	}
    else    
    { 
        if ((f = fopen(argv[1], "r")) == NULL) 
            exit(1);
    }

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

    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nnz)) !=0)
        exit(1);


    /* reseve memory for matrices */
    /* For the COO */
    I = (uint32_t *) malloc(nnz * sizeof(uint32_t));
    J = (uint32_t *) malloc(nnz * sizeof(uint32_t));
    val = (double *) malloc(nnz * sizeof(double));

    /* For the CSC */
    uint32_t* cscRow = (uint32_t *) malloc(nnz * sizeof(uint32_t));
    uint32_t* cscColumn = (uint32_t *) malloc((N + 1) * sizeof(uint32_t));

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

    /*
        Code that converts any symmetric matrix in upper triangular
    */
    /* Because the code works for an upper triangular matrix, we change the J, I according to the symmetric table that we have as input and the help of flag */
    /* flag 0 = upper triangular -> I,J | flag = 1 lower triangular -> J,I */
    int flag = 0;
    if(I[0] > J[0]) {
        flag = 1;
    }
    switch (flag)
    {
    case 0:
        printf("case 0 \n");
        coo2csc(cscRow, cscColumn, I, J, nnz, M, 0);
        break;
    
    case 1:
        printf("case 1 \n");
        coo2csc(cscRow, cscColumn, J, I, nnz, N, 0);
        break;

    default:
        break;
    }

    /* Initialize c3 with zeros*/
    int* c3;
    c3 = malloc(N * sizeof c3);    
    for(int i = 0; i < N; i++){
        c3[i] = 0;
    }

    printf("Matrix Loaded, now Searching!\n");

    /* We measure time from this point */
    gettimeofday(&start,NULL);

    int sum = 0;
    omp_set_dynamic(0);     // Explicitly disable dynamic teams
    omp_set_num_threads(num_of_threads); // Use num_of_threads threads for all consecutive parallel regions
    #pragma omp parallel for shared(sum, c3)
    for(int i = 1; i < N; i++) {
        for(int j = 0; j < cscColumn[i+1] - cscColumn[i]; j++) {
            int row1 = cscRow[cscColumn[i] + j];
            int col1 = i;
            for(int k = 0; k < cscColumn[row1+1] - cscColumn[row1]; k++) {
                int row3 = cscRow[cscColumn[row1] + k];
                int col3 = row1;
                // now i am searching for the x,y element 
                // x = col1
                // y = row3
                
                if(row3>col1) {
                    // loop the whole row3 column
                    for (int l = 0; l < cscColumn[row3+1] -cscColumn[row3]; l++) {
                        int row2 = cscRow[cscColumn[row3] + l];
                        if(row2 == col1) {
                            #pragma omp critical
                            sum++;
                            c3[col1]++;
                            c3[row3]++;
                            c3[col3]++;
                        }
                    }
                }
                else {
                    // loop the whole col1 column
                    for (int l = 0; l < cscColumn[col1+1] - cscColumn[col1]; l++) {
                        int row2 = cscRow[cscColumn[col1] + l];
                        if(row2 == row3) {
                            #pragma omp critical
                            sum++;
                            c3[col1]++;
                            c3[row3]++;
                            c3[col3]++;
                        }
                    }
                }
            }
        }
    }

    /* We stop measuring time at this point */
    gettimeofday(&end,NULL);
    double duration = (end.tv_sec+(double)end.tv_usec/1000000) - (start.tv_sec+(double)start.tv_usec/1000000);

    printf("Sum: %d \n", sum);
    printf("Duration: %f \n", duration);

    /* Deallocate the arrays */
    free(I);
    free(J);
    free(c3);

	return 0;
}

