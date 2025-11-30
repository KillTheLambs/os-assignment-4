#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 4

int size;
double *A, *B, *C;

// Helper to initialize matrices with random values
void init_matrix(double *mat, int n) {
    for (int i = 0; i < n * n; i++) {
        mat[i] = (rand() % 100) / 10.0; // Random float between 0.0 and 10.0
    }
}

void* multiply(void* arg) {
    int thread_id = *(int*)arg;
    
    // Calculate the slice of rows this thread is responsible for
    int start = (thread_id * size) / MAX_THREADS;
    int end = ((thread_id + 1) * size) / MAX_THREADS;

    for (int i = start; i < end; i++) {
        for (int j = 0; j < size; j++) {
            double sum = 0;
            for (int k = 0; k < size; k++) {
                sum += A[i * size + k] * B[k * size + j];
            }
            C[i * size + j] = sum;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    size = (argc > 1) ? atoi(argv[1]) : 500;
    printf("Pthread Matrix Size: %dx%d with %d threads\n", size, size, MAX_THREADS);

    A = malloc(size * size * sizeof(double));
    B = malloc(size * size * sizeof(double));
    C = malloc(size * size * sizeof(double));

    // --- INITIALIZATION (Added this part) ---
    srand(42); // Fixed seed for reproducibility
    init_matrix(A, size);
    init_matrix(B, size);
    
    // We don't need to init C because we overwrite it in the loop
    
    pthread_t threads[MAX_THREADS];
    int thread_ids[MAX_THREADS];
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < MAX_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, multiply, &thread_ids[i]);
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Pthreads Time:   %f seconds\n", time_taken);

    free(A); free(B); free(C);
    return 0;
}
