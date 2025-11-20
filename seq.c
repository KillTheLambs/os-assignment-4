#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define N 500 // Change this or pass as argument for 5x4, 2000x2000 etc

// Timer helper
double get_time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

void init_matrix(double *mat, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++) mat[i] = rand() % 10;
}

int main(int argc, char *argv[]) {
    int size = (argc > 1) ? atoi(argv[1]) : N;
    printf("Matrix Size: %dx%d\n", size, size);

    double *A = malloc(size * size * sizeof(double));
    double *B = malloc(size * size * sizeof(double));
    double *C = malloc(size * size * sizeof(double));
    
    init_matrix(A, size, size);
    init_matrix(B, size, size);

    struct timespec start, end;

    // --- SEQUENTIAL ---
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            double sum = 0;
            for (int k = 0; k < size; k++) sum += A[i * size + k] * B[k * size + j];
            C[i * size + j] = sum;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("Sequential Time: %f seconds\n", get_time_diff(start, end));

    // --- OPENMP ---
    // Reset C
    for(int i=0; i<size*size; i++) C[i] = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            double sum = 0;
            for (int k = 0; k < size; k++) sum += A[i * size + k] * B[k * size + j];
            C[i * size + j] = sum;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("OpenMP Time:     %f seconds\n", get_time_diff(start, end));

    free(A); free(B); free(C);
    return 0;
}
