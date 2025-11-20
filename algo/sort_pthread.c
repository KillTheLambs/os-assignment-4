#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Fixed number of threads for simplicity (matches the 4 cores/threads logic)
#define NUM_THREADS 4

// Struct to pass specific data ranges to each thread
struct ThreadArgs {
    int *arr;
    int left;
    int right;
};

// --- HELPER FUNCTIONS (Standard Merge Logic) ---

// Merges two subarrays: arr[l..m] and arr[m+1..r]
void merge(int *arr, int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));

    for (i = 0; i < n1; i++) L[i] = arr[l + i];
    for (j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    i = 0; j = 0; k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L); free(R);
}

// Standard Sequential Merge Sort (used locally by each thread)
void mergeSortSerial(int *arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSortSerial(arr, l, m);
        mergeSortSerial(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

// --- PTHREAD FUNCTIONS ---

// The function entry point for each thread
void* thread_runner(void* arg) {
    struct ThreadArgs* data = (struct ThreadArgs*)arg;
    // Sort the specific chunk assigned to this thread
    mergeSortSerial(data->arr, data->left, data->right);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    // Default to 1 million elements if no argument provided
    int n = (argc > 1) ? atoi(argv[1]) : 1000000;
    printf("Pthreads Sort | Array Size: %d | Threads: %d\n", n, NUM_THREADS);

    int *arr = malloc(n * sizeof(int));
    
    // Initialize array with random numbers
    srand(42); 
    for(int i = 0; i < n; i++) arr[i] = rand();

    pthread_t threads[NUM_THREADS];
    struct ThreadArgs args[NUM_THREADS];

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // 1. Divide array into 4 chunks and launch threads
    int chunk_size = n / NUM_THREADS;
    
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].arr = arr;
        args[i].left = i * chunk_size;
        
        // Calculate the right boundary
        if (i == NUM_THREADS - 1) {
            // The last thread takes whatever is left
            args[i].right = n - 1;
        } else {
            args[i].right = (i + 1) * chunk_size - 1;
        }
        
        // Create the thread
        pthread_create(&threads[i], NULL, thread_runner, &args[i]);
    }

    // 2. Wait for all 4 threads to finish sorting their individual chunks
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // 3. Merge the 4 sorted chunks together
    // Since we have 4 fixed chunks: [0], [1], [2], [3]
    
    // Calculate split points based on the chunk size used earlier
    int p1 = chunk_size - 1;          // End of T0
    int p2 = 2 * chunk_size - 1;      // End of T1
    int p3 = 3 * chunk_size - 1;      // End of T2

    // Merge T0 [0..p1] and T1 [p1+1..p2]
    merge(arr, 0, p1, p2);
    
    // Merge T2 [p2+1..p3] and T3 [p3+1..end]
    merge(arr, p2 + 1, p3, n - 1);
    
    // Final Merge: [Sorted Half 1] and [Sorted Half 2]
    merge(arr, 0, p2, n - 1);

    clock_gettime(CLOCK_MONOTONIC, &end);
    
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time Taken:   %f seconds\n", time_taken);

    free(arr);
    return 0;
}
