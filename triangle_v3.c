#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "mmio.h"
#include "coo2csc.h"
#include "readfile.h"


int main(int argc, char *argv[])
{
    // int ret_code;
    // MM_typecode matcode;
    FILE *f;
    uint32_t M, N, nnz;   
    int *I, *J;
    double *val;
    int binary = atoi(argv[2]);
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

    readfile_stepone(f, &M, &N, &nnz, I, J, binary);

    /* For the COO */
    I = (uint32_t *) malloc(nnz * sizeof(uint32_t));
    J = (uint32_t *) malloc(nnz * sizeof(uint32_t));
    val = (double *) malloc(nnz * sizeof(double));

    readfile_steptwo(f, &M, &N, &nnz, I, J, val, binary);

    /* For the CSC */
    uint32_t* cscRow = (uint32_t *) malloc(nnz * sizeof(uint32_t));
    uint32_t* cscColumn = (uint32_t *) malloc((N + 1) * sizeof(uint32_t));

    /* Initialize c3 with zeros*/
    int* c3;
    c3 = malloc(N * sizeof c3);    
    for(int i = 0; i < N; i++){
        c3[i] = 0;
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

    printf("Matrix Loaded, now Searching!\n");

    /* We measure time from this point */
    gettimeofday(&start,NULL);

    /* ALGORITHM STARTS HERE */

    int sum = 0;
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

    /* ALGORITHM ENDS HERE */

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

