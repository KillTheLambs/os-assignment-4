#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Helper to initialize matrices with reproducible random values
void init_matrix(double *mat, int n, int fill_value) {
    if (fill_value == 1) {
        // Use a fixed value for validation purposes (A=B=1)
        for (int i = 0; i < n * n; i++) mat[i] = 1.0;
    } else {
        // Use random values for general benchmarking (like the Pthreads file)
        srand(42); 
        for (int i = 0; i < n * n; i++) mat[i] = (rand() % 100) / 10.0;
    }
}

int main(int argc, char** argv) {
    int rank, nprocs;
    int size = (argc > 1) ? atoi(argv[1]) : 500;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    // --- 1. Memory Allocation and Distribution Setup ---
    
    // Arrays for distribution setup (only needed on root)
    int *sendcounts = NULL;
    int *displs = NULL;

    // A and C only need full allocation on the root (Rank 0)
    double *A = NULL;
    double *C = NULL;

    // B must be stored fully on all nodes (broadcast)
    double *B = malloc(size * size * sizeof(double));

    if (rank == 0) {
        printf("MPI Matrix Size: %dx%d on %d processes\n", size, size, nprocs);
        
        // Allocate full A and C on root
        A = malloc(size * size * sizeof(double));
        C = malloc(size * size * sizeof(double));

        // Initialize A and B (using fixed values for easy validation)
        // If you prefer reproducible random values, change the '1' to '0'
        init_matrix(A, size, 1); 
        init_matrix(B, size, 1);

        // Calculate sendcounts and displacements for ScatterV/GatherV
        sendcounts = malloc(nprocs * sizeof(int));
        displs = malloc(nprocs * sizeof(int));
        
        int offset = 0;
        for (int i = 0; i < nprocs; i++) {
            // Calculate the number of rows for this process.
            // Distribute rows as evenly as possible. The remainder goes to the first ranks.
            int rows = size / nprocs + (i < (size % nprocs) ? 1 : 0);
            
            // Sendcount is rows * columns (size)
            sendcounts[i] = rows * size;
            displs[i] = offset;
            offset += sendcounts[i];
        }
    }
    
    // Calculate size of the local chunk (same logic as above, but run on every rank)
    int rows_per_proc = size / nprocs + (rank < (size % nprocs) ? 1 : 0);
    int local_data_size = rows_per_proc * size;

    double *sub_A = malloc(local_data_size * sizeof(double));
    double *sub_C = malloc(local_data_size * sizeof(double));

    // --- 2. Communication and Timing ---
    
    // Broadcast B to all nodes (required for multiplication)
    MPI_Bcast(B, size * size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Synchronize clocks across all processes before starting the timed section
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // Scatter rows of A (using ScatterV to handle uneven distribution)
    MPI_Scatterv(A, sendcounts, displs, MPI_DOUBLE, sub_A, local_data_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // --- 3. Local Computation (Multiplication) ---
    for (int i = 0; i < rows_per_proc; i++) {
        for (int j = 0; j < size; j++) {
            double sum = 0;
            for (int k = 0; k < size; k++) {
                // sub_A is local row i, col k. B is global row k, col j.
                sum += sub_A[i * size + k] * B[k * size + j];
            }
            sub_C[i * size + j] = sum;
        }
    }

    // --- 4. Gather Results and Final Time ---
    
    // Synchronize again before stopping the clock
    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    // Gather sub_C results back to C on the root (using GatherV)
    MPI_Gatherv(sub_C, local_data_size, MPI_DOUBLE, C, sendcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // --- 5. Report and Cleanup ---
    if (rank == 0) {
        printf("MPI Time (incl. comms): %f seconds\n", end_time - start_time);

        // Validation Check (based on A=B=1)
        double expected_result = (double)size; // Result should be SUM(1*1) = size
        if (C[0] == expected_result) {
            printf("Validation: C[0][0] = %.2f (PASS)\n", C[0]);
        } else {
            printf("Validation: C[0][0] = %.2f (FAIL - Expected %.2f)\n", C[0], expected_result);
        }
        
        free(A); free(C);
        free(sendcounts); free(displs);
    }
    
    free(B); free(sub_A); free(sub_C);
    MPI_Finalize();
    return 0;
}
