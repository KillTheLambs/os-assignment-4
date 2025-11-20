#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int rank, nprocs;
    int size = (argc > 1) ? atoi(argv[1]) : 500;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    double *A = NULL, *B = malloc(size * size * sizeof(double));
    double *C = NULL;
    double *sub_A = malloc((size * size / nprocs) * sizeof(double));
    double *sub_C = malloc((size * size / nprocs) * sizeof(double));

    if (rank == 0) {
        A = malloc(size * size * sizeof(double));
        C = malloc(size * size * sizeof(double));
        // Init A and B here...
        for(int i=0; i<size*size; i++) { A[i]=1.0; B[i]=1.0; }
        printf("MPI Matrix Size: %dx%d on %d processes\n", size, size, nprocs);
    }

    // Broadcast B to all nodes (required for multiplication)
    MPI_Bcast(B, size * size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // Scatter rows of A
    MPI_Scatter(A, size * size / nprocs, MPI_DOUBLE, sub_A, size * size / nprocs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double start_time = MPI_Wtime();

    int rows_per_proc = size / nprocs;
    for (int i = 0; i < rows_per_proc; i++) {
        for (int j = 0; j < size; j++) {
            double sum = 0;
            for (int k = 0; k < size; k++) {
                sum += sub_A[i * size + k] * B[k * size + j];
            }
            sub_C[i * size + j] = sum;
        }
    }

    double end_time = MPI_Wtime();

    // Gather results back to C
    MPI_Gather(sub_C, size * size / nprocs, MPI_DOUBLE, C, size * size / nprocs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("MPI Time:        %f seconds\n", end_time - start_time);
        free(A); free(C);
    }
    
    free(B); free(sub_A); free(sub_C);
    MPI_Finalize();
    return 0;
}
