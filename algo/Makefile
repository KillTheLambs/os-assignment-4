CC = gcc
CFLAGS = -O2 -Wall -fopenmp
LIBS = -lpthread

# Build all three sorting programs
all: sort_seq sort_pthread sort_omp

# 1. Sequential Sort
sort_seq: sort_seq.c
	$(CC) $(CFLAGS) -o sort_seq sort_seq.c

# 2. Pthreads Sort
sort_pthread: sort_pthread.c
	$(CC) $(CFLAGS) -o sort_pthread sort_pthread.c $(LIBS)

# 3. OpenMP Sort
sort_omp: sort_omp.c
	$(CC) $(CFLAGS) -o sort_omp sort_omp.c

clean:
	rm -f sort_seq sort_pthread sort_omp
