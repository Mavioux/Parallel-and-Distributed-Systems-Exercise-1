#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BILLION  1000000000L;

void print2DMatrix(int** matrix, int size){
    int i = 0;
    for(i = 0; i < size; i++){
        int j = i;
        for(j = 0; j < size; j++){
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void print1DMatrix(int* matrix, int size){
    int i = 0;
    for(i = 0; i < size; i++){
        printf("%d: %d \n",i, matrix[i]);
    }
}

int main(int argc, char* argv[]) {

    /* Get the number of Node from the command line arguments */
    int nodes = atoi(argv[1]);

    /*
       Graph adjacency matrix creation
    */

    /* Initialize the adjacency matrix dynamically */
    int** adjacencyMatrix;
    adjacencyMatrix = malloc(nodes * sizeof *adjacencyMatrix);
    int i = 0;
    for (i=0; i < nodes; i++) {
        adjacencyMatrix[i] = malloc(nodes * sizeof *adjacencyMatrix[i]);
    }

    srand(time(NULL));

    /* Initialize the upper half of the adjacency matrix */
    for(i = 0; i < nodes; i++){
        int j = i;
        for(j = i; j < nodes; j++){
            if(i == j) {
                /* The diagonal line should be zero */
               adjacencyMatrix[i][j] = 0; 
            }
            else{
                /* Inialize the upper half of the adjacency matrix */
                adjacencyMatrix[i][j] = random() % 2;     /* random int between 0 and 1 */
                adjacencyMatrix[j][i] = adjacencyMatrix[i][j];      /* Inialize the lower half of the adjacency matrix with the symmetrical value*/
            }
            
        }
    }

    /* Initialize c3 with zeros*/
    int* c3;
    c3 = malloc(nodes * sizeof c3);    
    for(i = 0; i < nodes; i++){
        c3[i] = 0;
    }

    // print2DMatrix(adjacencyMatrix, nodes);

    /* We measure time from this point */
    clock_t begin = clock();

    /* Search for triangles */
    for(int i = 0; i < nodes; i++){
        int j = 0;
        for(int j = 0; j < nodes; j++){
            int k = 0;
            for(int k = 0; k < nodes; k++){
                if(adjacencyMatrix[i][j] == 1 && adjacencyMatrix[j][k] == 1 && adjacencyMatrix[k][i] == 1) {
                    c3[i]++;
                    c3[j]++;
                    c3[k]++;
                }
            }
        }
    }

    /* We stop measuring time at this point */
    clock_t end = clock();
    double duration = (double)(end - begin) / CLOCKS_PER_SEC;

    print1DMatrix(c3, nodes);

    printf("Duration: %f \n", duration);
    

    /* Deallocate the arrays */
    for (i=0; i < nodes; i++) {
        free(adjacencyMatrix[i]);
    }
    free(adjacencyMatrix);
    free(c3);

}

