// RUN: mpicc -g -fopenmp %s -o %s.exe 
// RUN: env OMP_NUM_THREADS=2 mustrun --must:mpimode MPMD --must:openmp --must:layout %root-dir/%omp-layout \
// RUN: %s.exe 2>&1 > %s.%must-version.log || true
// RUN: cat %s.%must-version.log | %filecheck  --implicit-check-not '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' --implicit-check-not 'BAD TERMINATION' %s


#include "../../nondeterminism.h"
#include <mpi.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * the user had to make shure only one collective initialization is active at a time per process and communicator
 * according to the standard (see p303), this can be done with a openmp critical
 */

int main(int argc, char *argv[]) {
  int myRank, size;
  int provided;
  const int requested = MPI_THREAD_MULTIPLE;

  MPI_Init_thread(&argc, &argv, requested, &provided);
  if (provided < requested) {
    // has_error_manifested(false);
    exit(EXIT_FAILURE);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const int other_rank = size - myRank - 1;

  if (myRank == 0) {
#pragma omp parallel
    {
#pragma omp task
      {
        int *buffer = malloc(BUFFER_LENGTH_BYTE);
#pragma omp critical
        MPI_Bcast(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD);
        free(buffer);
      }
#pragma omp task
      {
        int *buffer = malloc(BUFFER_LENGTH_BYTE);
#pragma omp critical
        MPI_Bcast(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD);
        free(buffer);
      }
    }  // end parallel

  }

  else {        // other MPI rank
    usleep(5);  // "force" other Process to call the barrier two times at the same time
    int *buffer = malloc(BUFFER_LENGTH_BYTE);
    MPI_Bcast(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD);
    // Added extra broadcasts since rank 0 posts 4 bcasts with 4 threads. 
    MPI_Bcast(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD);
    free(buffer);
  }

  MPI_Finalize();

  return 0;
}
