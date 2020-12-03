#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "mmio.h"
#include "coo2csc.h"

#include <cilk/cilk.h>
#include <pthread.h>
#include <cilk/cilk_api.h>

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
    char* string_num_of_threads = argv[3];
    struct timeval start, end;

    if (argc < 2)
	{
		fprintf(stderr, "Usage: %s [martix-market-filename] [0 for binary or 1 for non binary] [num of threads]\n", argv[0]);
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


    /* Reseve memory for matrices */

    /* For the COO */
    /* Double the memory to store the full matrix */
    I = (uint32_t *) malloc(2 * nnz * sizeof(uint32_t));
    J = (uint32_t *) malloc(2 * nnz * sizeof(uint32_t));
    val = (double *) malloc(nnz * sizeof(double));

    /* For the CSC */
    uint32_t* cscRow = (uint32_t *) malloc(2 * nnz * sizeof(uint32_t));
    uint32_t* cscColumn = (uint32_t *) malloc((N + 1) * sizeof(uint32_t));

    /* For the C CSC */
    uint32_t* c_cscRow = (uint32_t *) malloc(0 * sizeof(uint32_t));
    uint32_t* c_values = (uint32_t *) malloc(0 * sizeof(uint32_t));
    uint32_t* c_cscColumn = (uint32_t *) malloc((N + 1) * sizeof(uint32_t));

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
    
    /* Stop reading coo file */
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

    printf("\nMatrix Loaded!\n");

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

    /* We measure time from this point */
    gettimeofday(&start,NULL);

    /* Initialization and memory allocation of C matrix */
    c_cscColumn[0] = 0;    
    c_cscRow = realloc(c_cscRow, 2 * nnz * sizeof(int));
    c_values = realloc(c_cscRow, 2 * nnz * sizeof(int)); 

    pthread_mutex_t mutex; //define the lock
    pthread_mutex_init(&mutex,NULL); //initialize the lock

    /* Cilk set number of workers */
    __cilkrts_set_param("nworkers",string_num_of_threads);
    int numWorkers = __cilkrts_get_nworkers();
    printf("There are %d workers.\n",numWorkers);

    // C = A.*(A*A)   
    cilk_for(int i = 0; i < N; i++) {
       cilk_for(int j = 0; j < cscColumn[i+1] - cscColumn[i]; j++) {
            int a_row = cscRow[cscColumn[i] + j];
            int a_col = i;

            // Element of (A*A)[i,j]
            int k_size = cscColumn[a_row+1] - cscColumn[a_row];  
            int l_size = cscColumn[a_col+1] - cscColumn[a_col];    
            int *l = malloc((l_size) * sizeof(int));
            int *k = malloc((k_size) * sizeof(int));
            /* Create the l and k vectors with the appropriate values */
            for(int x = 0; x < k_size; x++) {
                k[x] = cscRow[cscColumn[a_row] + x];
            }
            for(int x = 0; x < l_size; x++) {
                l[x] = cscRow[cscColumn[a_col] + x];
            }

            /* Compare k and l elements */
            int k_pointer = 0;
            int l_pointer = 0;
            int value = 0;
            while(k_pointer != k_size && l_pointer != l_size) {
                if(k[k_pointer] == l[l_pointer]) {
                    value++;
                    k_pointer++;
                    l_pointer++;
                }
                else if(k[k_pointer] > l[l_pointer]) {
                    l_pointer++;
                }
                else
                {
                    k_pointer++;
                }                
            }        
            if(value) {
                c_values[cscColumn[i] + j] = value;
            }
            free(l);
            free(k);
        }
    }
    /* Since no value can be zero other than the original ones, we can safely assume that row and column arrays of C matrix will have the identical elements with A matrix */
    c_cscRow = cscRow;
    c_cscColumn = cscColumn;

    /* Multiplication of a NxN matrix with a Nx1 vector
    We search the whole column (-> row since it is symmetric)
    Then every row that exists (-> column) has a specific value
    So we add up the multiplication of each row element with the value */
    /* Not worth parallelizing here */
    /* Because it requires mutex when writing the result_vector[row] */
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < c_cscColumn[i+1] - c_cscColumn[i]; j++) {
            int row = c_cscRow[c_cscColumn[i] + j];
            int col = i;
            int value = c_values[c_cscColumn[i] + j];
            /* we now have the element (row, col) | its value is value[] */
            /* Because of its symmetry we also have the element (col, row) */
            // pthread_mutex_lock(&mutex);
            result_vector[row] += value * v[col]; /* res[row] += A[row, col] * v[col] */
            // pthread_mutex_unlock(&mutex);
        }
    }
    int triangle_sum = 0;
    /* No real benefit parallelizing this either */
    for(int i = 0; i < N; i++) {
        c3[i] = result_vector[i] / 2;
        triangle_sum += c3[i];
    }

    triangle_sum = triangle_sum / 3;

    /* We stop measuring time at this point */
    gettimeofday(&end,NULL);
    double duration = (end.tv_sec+(double)end.tv_usec/1000000) - (start.tv_sec+(double)start.tv_usec/1000000);

    printf("\nTriangle Sum: %d",  triangle_sum);
    printf("\nDuration: %f\n",  duration);

    /* Deallocate the arrays */
    free(I);
    free(J);
    free(c_cscRow);
    free(c_cscColumn);
    free(c3);
    free(v);
    free(result_vector);

	return 0;
}

