# Compiler definitions
CC = gcc
MPICC = mpicc

# Flags: -O2 for optimization, -Wall for warnings
# -fopenmp is included for OpenMP support in seq.c or mergeSort.c
CFLAGS = -O2 -Wall -fopenmp
LIBS = -lpthread

# 'all' target builds everything
all: seq pthread mpi mergeSort

# 1. Sequential (and likely OpenMP if included in same file)
seq: seq.c
	$(CC) $(CFLAGS) -o seq seq.c $(LIBS)

# 2. Pthreads
pthread: pthread.c
	$(CC) $(CFLAGS) -o pthread pthread.c $(LIBS)

# 3. MPI - Uses the mpicc wrapper
mpi: mpi.c
	$(MPICC) $(CFLAGS) -o mpi mpi.c

# 4. Custom Algorithm (Merge Sort)
mergeSort: mergeSort.c
	$(CC) $(CFLAGS) -o mergeSort mergeSort.c $(LIBS)

# Clean up compiled binaries
clean:
	rm -f seq pthread mpi mergeSort
