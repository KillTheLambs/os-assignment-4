# Compiler definitions
CC = gcc
MPICC = mpicc

# Flags: -O2 for optimization, -Wall for warnings
# -fopenmp is included for OpenMP support
CFLAGS = -O2 -Wall -fopenmp
LIBS = -lpthread

# 'all' target builds all matrix multiplication programs
all: seq pthread mpi

# 1. Sequential (and OpenMP if included in seq.c)
seq: seq.c
	$(CC) $(CFLAGS) -o seq seq.c $(LIBS)

# 2. Pthreads
pthread: pthread.c
	$(CC) $(CFLAGS) -o pthread pthread.c $(LIBS)

# 3. MPI - Uses the mpicc wrapper
mpi: mpi.c
	$(MPICC) $(CFLAGS) -o mpi mpi.c

# Clean up compiled binaries
clean:
	rm -f seq pthread mpi
