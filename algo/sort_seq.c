#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Merges two subarrays of arr[].
void merge(int *arr, int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    // Create temp arrays
    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));

    // Copy data to temp arrays
    for (i = 0; i < n1; i++) L[i] = arr[l + i];
    for (j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    // Merge the temp arrays back into arr[l..r]
    i = 0; j = 0; k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L); free(R);
}

void mergeSort(int *arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

int main(int argc, char *argv[]) {
    int n = (argc > 1) ? atoi(argv[1]) : 1000000; // Default 1 Million
    printf("Sequential Sort | Array Size: %d\n", n);

    int *arr = malloc(n * sizeof(int));
    srand(42); // Seed for reproducibility
    for(int i = 0; i < n; i++) arr[i] = rand();

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    mergeSort(arr, 0, n - 1);

    clock_gettime(CLOCK_MONOTONIC, &end);
    double time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time Taken: %f seconds\n", time);

    free(arr);
    return 0;
}
