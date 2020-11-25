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

int compare( const void* a, const void* b)
{
     int int_a = * ( (int*) a );
     int int_b = * ( (int*) b );

     if ( int_a == int_b ) return 0;
     else if ( int_a < int_b ) return -1;
     else return 1;
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
    /* Requires every element in the diagonal to be zero */
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

    /* For the B matrix in CSC format */
    uint32_t* B_cscRow = (uint32_t *) malloc(0 * sizeof(uint32_t));
    uint32_t* B_cscColumn = (uint32_t *) malloc((N + 1) * sizeof(uint32_t));
    uint32_t* B_values = (uint32_t *) malloc(2 * nnz * sizeof(uint32_t));


    //B = A*A
    int values_counter = 0;
    B_cscColumn[0] = 0;
    for(int i = 0; i < N; i++) {
        int k_size = cscColumn[i+1] - cscColumn[i];
        int* k;
        k = (int *) malloc(k_size * sizeof(k));
        for(int w = 0; w < k_size; w++) {
            k[w] = cscRow[cscColumn[i] + w];
        }
        
        for(int j = 0; j < N; j++) {
            int flag = 0;
            int value = 0;
            int l_size = cscColumn[j+1] - cscColumn[j];
            int *l;
            l = malloc((k_size + l_size) * sizeof(int));
            /* Create the l vector with the appropriate values */
            for(int x = 0; x < k_size; x++) {
                l[x] = k[x];
            }
            for(int x = 0; x < l_size; x++) {
                l[k_size + x] = cscRow[cscColumn[j] + x];
            }

            // We have in our hands the array l with the columns of the i-th row and the rows of the j-th column
            // We have to sort it and for each duplicate element -> c[i,j]++
            qsort( l, (k_size + l_size), sizeof(int), compare );
            
            // printf("k size: %d\n", cscColumn[i+1] - cscColumn[i]);
            // printf("l size: %d\n", cscColumn[j+1] - cscColumn[j]);
            for(int index = 0; index < (k_size + l_size - 1); index++) {
                if(l[index] == l[index+1]) {
                    value++;
                    flag = 1;
                }
            }
            if(value) {
                printf("Element in row %d, col %d \n", i, j);
                values_counter++;
                B_cscRow = realloc(B_cscRow, values_counter * sizeof(int));
                B_cscRow[values_counter - 1] = j;
                B_values = realloc(B_values, values_counter * sizeof(int));
                B_values[values_counter - 1] = value;
            }
        }           
    B_cscColumn[i+1] = values_counter;
    }
    B_cscColumn[N+1] = values_counter;

    /* For the D_CSC */
    int d_nnz = 0;
    uint32_t* d_cscRow = (uint32_t *) malloc(d_nnz * sizeof(uint32_t));
    uint32_t* d_values = (uint32_t *) malloc(d_nnz * sizeof(uint32_t));
    uint32_t* d_cscColumn = (uint32_t *) malloc((N + 1) * sizeof(uint32_t));

    d_cscColumn[0] = 0;

    /* D = A (hadamard product) B; */
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < cscColumn[i+1] - cscColumn[i]; j++) {
            /* We loop through every element of the A matrix that has a value of 1 */
            int a_row = cscRow[cscColumn[i] + j];
            int a_col = i;

            /* Now we got to check whether, at this specific row and column, the B matrix has an element.
            If so we multiply these two values and store the result at the position i,j of the D matrix.
            We loop through the whole (same) B column and search for a b_row that matches our a_row.
            If such a row exists we search for its value (his value won't be zero).
            Then we store it in the D matrix and do d_nnz++.
            If such a value does not exist we store nothing. */

            for(int k = 0; k < B_cscColumn[a_col+1] - B_cscColumn[a_col]; k++) {
                int b_row = B_cscRow[B_cscColumn[a_col] + k];
                if(b_row == a_row) {
                    /* We found a non zero element at i,j.
                       It will be stored in the same column as A and B matrix.
                       Also we do d_nnz++ and we dynamically allocate more space to d_rows so as to store the row index of the new value
                       We also dynamically allocate more space to d_values so as to to store the value */
                       d_nnz++;
                       d_cscRow = realloc( d_cscRow, sizeof(int) * d_nnz);
                       d_values = realloc( d_values, sizeof(int) * d_nnz);
                       d_cscRow[d_nnz-1] = a_row;
                       d_values[d_nnz-1] = B_values[B_cscColumn[a_col] + k];
                }
            }
        }
        d_cscColumn[i+1] = d_nnz;
    }

    // printf("\n D Rows: ");
    // for(uint32_t i = 0; i < d_nnz; i++) {
    //     printf("%d ", d_cscRow[i]);
    // }

    // printf("\n D Columns: ");
    // for(uint32_t i = 0; i < N+1; i++) {
    //     printf("%d ", d_cscColumn[i]);
    // }

    // printf("\n D Values: ");
    // for(uint32_t i = 0; i < d_nnz; i++) {
    //     printf("%d ", d_values[i]);
    // }


    /* Initialize c3 with zeros*/
    int* c3;
    c3 = malloc(N * sizeof c3);    
    for(int i = 0; i < N; i++){
        c3[i] = 0;
    }

    /* Initialize v with ones*/
    int* v;
    v = malloc(N * sizeof v);    
    for(int i = 0; i < N; i++){
        v[i] = 1;
    }

    /* Initialize result with zeros*/
    int* result_vector;
    result_vector = malloc(N * sizeof result_vector);    
    for(int i = 0; i < N; i++){
        result_vector[i] = 0;
    }

    // Multiplication of a NxN matrix with a Nx1 vector
    // We search the whole column (aka row since it is symmetric)
    // Then every row that exists (aka column) has a value of 1
    // So we add up the multiplication of each row element with the value of the
    for(int i = 0; i < N; i++) {
        printf("i: %d \n", i);
        for(int j = 0; j < d_cscColumn[i+1] - d_cscColumn[i]; j++) {
            int row = d_cscRow[d_cscColumn[i] + j];
            int col = i;
            // we now have the element (row, col) | its value is 1
            // Because of its symmetry we also have the element (col, row)
            result_vector[row] += 1 * v[col]; /* res[row] = A[row, col] * v[col] */
            result_vector[col] += 1 * v[row]; /* res[col] = A[row, col] * v[row] */
        }
    }

    for(int i = 0; i < N; i++) {
        c3[i] = result_vector[i] / 2;
    }

    printf("\nResult vector \n");
    print1DMatrix(c3, N);

    /* Deallocate the arrays */
    free(I);
    free(J);
    free(c3);
    free(v);
    free(result_vector);

	return 0;
}

