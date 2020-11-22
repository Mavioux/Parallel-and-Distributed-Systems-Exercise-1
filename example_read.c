/* 
*   Matrix Market I/O example program
*
*   Read a real (non-complex) sparse matrix from a Matrix Market (v. 2.0) file.
*   and copies it to stdout.  This porgram does nothing useful, but
*   illustrates common usage of the Matrix Matrix I/O routines.
*   (See http://math.nist.gov/MatrixMarket for details.)
*
*   Usage:  a.out [filename] > output
*
*       
*   NOTES:
*
*   1) Matrix Market files are always 1-based, i.e. the index of the first
*      element of a matrix is (1,1), not (0,0) as in C.  ADJUST THESE
*      OFFSETS ACCORDINGLY offsets accordingly when reading and writing 
*      to files.
*
*   2) ANSI C requires one to use the "l" format modifier when reading
*      double precision floating point numbers in scanf() and
*      its variants.  For example, use "%lf", "%lg", or "%le"
*      when reading doubles, otherwise errors will occur.
*/

#include <stdio.h>
#include <stdlib.h>
#include "mmio.h"
#include "coo2csc.h"

int main(int argc, char *argv[])
{
    int ret_code;
    MM_typecode matcode;
    FILE *f;
    uint32_t M, N, nnz;   
    int *I, *J;
    double *val;

    if (argc < 2)
	{
		fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
		exit(1);
	}
    else    
    { 
        if ((f = fopen("test3.mtx", "r")) == NULL) 
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
    

    printf("\n Rows: ");
    for(uint32_t i = 0; i < nnz; i++) {
        printf("%d ", cscRow[i]);
    }

    printf("\n Columns: ");
    for(uint32_t i = 0; i < N+1; i++) {
        printf("%d ", cscColumn[i]);
    }

    printf("\n");
    

    int sum = 0;
    // for(uint32_t column = 0; column < N + 1 - 1; column++) {
    //     for(uint32_t j = 0; j < cscColumn[column+1] - cscColumn[column]; j++) {
    //         uint32_t row1 = cscRow[cscColumn[column] + j];
    //         // to prwto stoixeio poy elegxoume einai to row, column
    //         // sth synexeia kai me ayto to stoixeio, 8elw na vrw to column, row 
    //         for(uint32_t k = j + 1; k < cscColumn[column+1] - cscColumn[column]; k++) {
    //             // elegxoume twra sto idio column an yparxei allo stoixeio me row poy arkei na einai diaforetiko tou arxikou, kai ayto to epivevaiwnoyme me k = j + 1
    //             uint32_t row2 = cscRow[cscColumn[column] + k];
    //             for(uint32_t l = k + 1; l < cscColumn[column+1] - cscColumn[column]; l++) {
    //                 // elegxoume twra sto idio column an yparxei allo stoixeio me row poy arkei na einai diaforetiko tou row2 kai sygxronws idio me to row1, kai ayto to epivevaiwnoyme me k = j + 1 kai me elegxo
    //                 uint32_t row3 = cscRow[cscColumn[column] + l];
    //                 if(row1 == row3) {
    //                     sum++;
    //                 }
    //             }
    //         }
    //     }        
    // }

    //tsekarw ka8e column
    for(int i = 1; i < N; i++){
        printf("\ni: %d \n", i);
        // gia to antistoixo (i) column looparw to length twn rows pou yparxoyn kai kratw thn timh tou row
        // etsi vriskw to stoixeio row1 gia to opoio 8a 3ekinhsw thn anazhthsh
        for(int j = 0; j < cscColumn[i+1] - cscColumn[i]; j++) {
            printf("j: %d \n", j);
            int row1 = cscRow[cscColumn[i] + j];
            int col1 = i;
            printf("row1, col1: %d, %d \n", row1, col1);
            // se ayto to shmeio exoume parei pleon to prwto stoixeio kai 8eloume na vroume pi8anes deyteres synexeies pou 8a vriskontai sth seira row1
            // twra 8eloume na tsekaroume ana sthlh sthlh an yparxei stoixeio 1 kai na vroume kai se poia sthlh einai
            for(int tempColumn = i + 1; tempColumn < N; tempColumn++) {
                // printf("Checking the column tempColumn: %d and we are checking every element of it in order to find an element in rows vector that matches the value col1: %d\n", tempColumn, col1);
                for(int k = 0; k < cscColumn[tempColumn+1] - cscColumn[tempColumn]; k++) {
                    // printf("kth: %d element in column: %d \n", k, tempColumn);
                    int row2 = cscRow[cscColumn[tempColumn] + k];
                    // if nothing matches col1 then we move on to the next column
                    if (row2 == col1) {
                        // if something matches col1 then we save its column and we then check whether col2 > row1, if yes then we search in a row, if not in a column
                        int col2 = tempColumn;
                        printf("row2, col2: %d, %d \n", row2, col2);
                        // exoume pleon sta xeria mas kai to stoixeio col1, col2 h alliws row2, col2
                        // twra menei aplws na eleg3oume thn sthlh row1 kai na vroyme an sto eyros ths yparxei stoixeio col2
                        printf("col2, row1: %d %d  \n", col2, row1);
                        // if (col2 > row) epeidh exoume mono ton anw trigwniko pinaka 8a prepei na to koita3oume ana grammh kai na psa3oume gia to stoixeio row1, col2
                        // ara kanoume 3ana anazhthsh sthn grammh row1 kai psaxnoume na vroume an yparxei sthn grammh row1 stoixeio me timh col2
                        if(col2 > row1) {
                            // search every column after the column that we found the second element 
                            for(int l = 0; l < cscColumn[col2+1] - cscColumn[col2]; l++) {
                                // if we find a row3 that matches row 1 we have a triangle
                                int row3 = cscRow[cscColumn[col2] + l];
                                printf("row3: %d \n", row3);
                                if(row3 == row1 && row3 != row2) {
                                    printf("vrhka trigwno \n");
                                    printf("row1, col1: %d, %d \n", row1, col1);
                                    printf("row2, col2: %d, %d \n", row2, col2);
                                    printf("row3, col3: %d, %d \n---\n", col2, row1);
                                    sum++;
                                }
                            }                                
                            
                        }
                        // for(int l = 0; l < cscColumn[row1+2] - cscColumn[row1+1]; l++) {
                        //     printf("l: %d \n", l);
                        //     int row3 = cscRow[cscColumn[row1+2] + l];
                        //     printf("row3: %d \n", row3);
                        //     printf("col2: %d \n", col2);
                        //     if(row3 == col2) {
                        //         //vrhkame trigwno
                        //         printf("we found a triangle \n");
                        //         printf("%d, %d -> %d, %d -> %d, %d \n", row1, col1, col1, col2, row3, row1);
                        //         sum++;
                        //     }
                        // }
                    }
                }      
            }
        }
    }

    printf("Sum: %d \n", sum);

	return 0;
}

