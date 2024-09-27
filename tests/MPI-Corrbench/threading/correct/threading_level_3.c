
// RUN: mpicc -g -fopenmp %s -o %s.exe 
// RUN: env OMP_NUM_THREADS=4 mustrun --must:instant-logging warning --must:openmp --must:layout %root-dir/%omp-layout \
// RUN: %s.exe 2>&1 > %s.log || true
// RUN: cat %s.log | %filecheck  --implicit-check-not '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' --implicit-check-not 'BAD TERMINATION' %s

#include <mpi.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * This program needs at least MPI_THREAD_SERIALIZED
 */
int main(int argc, char *argv[]) {
  int myRank;
  int buffer_out[10], buffer_in[10];
  int provided;
  const int requested = MPI_THREAD_MULTIPLE;

  MPI_Init_thread(&argc, &argv, requested, &provided);
  if (provided < requested) {
    exit(EXIT_FAILURE);  // no MPI error if program exit here
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

#pragma omp parallel
  {
#pragma omp for
    for (int i = 0; i < 10; i++) {
      buffer_out[i] = i * 10;
    }
// implicit OpenMP barrier
#pragma omp single
    {
      if (myRank == 0) {
        MPI_Send(buffer_out, 10, MPI_INT, 1, 123, MPI_COMM_WORLD);
      } else if (myRank == 1) {
        MPI_Recv(buffer_in, 10, MPI_INT, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }
  }

  if (myRank == 0) {
    MPI_Send(buffer_out, 10, MPI_INT, 1, 123, MPI_COMM_WORLD);
  } else if (myRank == 1) {
    MPI_Recv(buffer_in, 10, MPI_INT, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();

  return 0;
}
