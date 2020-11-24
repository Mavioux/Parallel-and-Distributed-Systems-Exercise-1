#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mmio.h"
#include "coo2csc.h"

#define BILLION  1000000000L;

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

    /* Initialize the timespec values and the duration value for the calculation of the computation time */
    struct timespec start, stop;
    double duration;

    if (argc < 2)
	{
		fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
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
    /* Double the memory to store the full matrix */
    I = (uint32_t *) malloc(2 * nnz * sizeof(uint32_t));
    J = (uint32_t *) malloc(2 * nnz * sizeof(uint32_t));
    val = (double *) malloc(nnz * sizeof(double));

    /* For the CSC */
    uint32_t* cscRow = (uint32_t *) malloc(2 * nnz * sizeof(uint32_t));
    uint32_t* cscColumn = (uint32_t *) malloc((N + 1) * sizeof(uint32_t));


    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

    /* For graphs with values */
    // for (int i=0; i<nnz; i++)
    // {
    //     fscanf(f, "%d %d %lg\n", &I[i], &J[i], &val[i]);
    //     I[i]--;  /* adjust from 1-based to 0-based */
    //     J[i]--;
    // }

    for (uint32_t i=0; i<nnz; i++)
    {
        /* I is for the rows and J for the columns */
        fscanf(f, "%d %d \n", &I[i], &J[i]);
        I[i]--;  /* adjust from 1-based to 0-based */
        J[i]--;
    }

    if (f !=stdin) fclose(f);

    if(M != N) {
        printf("COO matrix' columns and rows are not the same");
    }

    /* Add each value once more so we get the full symmetric matrix */
    for(uint32_t i = 0; i < nnz; i++) {
        I[nnz + i] = J[i];
        J[nnz + i] = I[i];
    }

    /* Because the code works for an upper triangular matrix, we change the J, I according to the symmetric table that we have as input and the help of flag */
    /* flag 0 = upper triangular -> I,J | flag = 1 lower triangular -> J,I */
    int flag = 0;
    if(I[0] > J[0]) {
        flag = 1;
    }
    switch (flag)
    {
    case 0:
        coo2csc(cscRow, cscColumn, I, J, 2 * nnz, M, 0);
        break;
    
    case 1:
        coo2csc(cscRow, cscColumn, J, I, 2 * nnz, N, 0);
        break;

    default:
        break;
    }

    for(int i = 0; i < N; i++) {
        int k_size = cscColumn[i+1] - cscColumn[i];
        int* k;
        k = (int *) malloc(k_size * sizeof(k));
        for(int w = 0; w < k_size; w++) {
            k[w] = cscRow[cscColumn[i] + w];
        }
        for(int j = 0; j < N; j++) {
            int l_size = cscColumn[j+1] - cscColumn[j];
            int *l;
            l = realloc(k, (k_size + l_size) * sizeof(int));
            for(int x = 0; x < l_size; x++) {
                l[k_size + x] = cscRow[cscColumn[j] + x];
            }
            // We have in our hands the array l with the columns of the i-th row and the rows of the j-th column
            // We have to sort it and for each duplicate element -> c[i,j]++
            
        }
    }
    

    // printf("\n Rows: ");
    // for(uint32_t i = 0; i < 2* nnz; i++) {
    //     printf("%d ", cscRow[i]);
    // }

    // printf("\n Columns: ");
    // for(uint32_t i = 0; i < N+1; i++) {
    //     printf("%d ", cscColumn[i]);
    // }


    // /* Initialize c3 with zeros*/
    // int* c3;
    // c3 = malloc(N * sizeof c3);    
    // for(int i = 0; i < N; i++){
    //     c3[i] = 0;
    // }

    // /* Initialize v with ones*/
    // int* v;
    // v = malloc(N * sizeof v);    
    // for(int i = 0; i < N; i++){
    //     v[i] = 1;
    // }

    // /* Initialize result with zeros*/
    // int* result_vector;
    // result_vector = malloc(N * sizeof result_vector);    
    // for(int i = 0; i < N; i++){
    //     result_vector[i] = 0;
    // }

    // // /* We measure time from this point */
    // // if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
    // //   perror( "clock gettime" );
    // //   exit( EXIT_FAILURE );
    // // }

    // // Multiplication of a NxN matrix with a Nx1 vector
    // // We search the whole column (aka row since it is symmetric)
    // // Then every row that exists (aka column) has a value of 1
    // // So we add up the multiplication of each row element with the value of the
    // for(int i = 0; i < N; i++) {
    //     printf("i: %d \n", i);
    //     for(int j = 0; j < cscColumn[i+1] - cscColumn[i]; j++) {
    //         int row = cscRow[cscColumn[i] + j];
    //         int col = i;
    //         // we now have the element (row, col) | its value is 1
    //         // Because of its symmetry we also have the element (col, row)
    //         result_vector[row] += 1 * v[col]; /* res[row] = A[row, col] * v[col] */
    //         result_vector[col] += 1 * v[row]; /* res[col] = A[row, col] * v[row] */
    //     }
    // }

    // /* We stop measuring time at this point */
    // if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
    //   perror( "clock gettime" );
    //   exit( EXIT_FAILURE );
    // }

    // duration = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / BILLION;

    // printf("V matrix \n");
    // print1DMatrix(v, N);
    // printf("Result vector \n");
    // print1DMatrix(result_vector, N);
    // printf("Sum: %d \n", sum);
    // printf("Duration: %f \n", duration);

    /* Deallocate the arrays */
    free(I);
    free(J);
    // free(c3);
    // free(v);
    // free(result_vector);

	return 0;
}

