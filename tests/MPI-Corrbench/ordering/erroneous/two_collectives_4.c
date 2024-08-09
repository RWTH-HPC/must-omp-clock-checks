
// RUN: mpicc -g -fopenmp %s -o %s.exe 
// RUN: env OMP_NUM_THREADS=4 mustrun --must:mpimode MPMD --must:openmp --must:layout %root-dir/%omp-layout \
// RUN: %s.exe 2>&1 > %s.%must-version.log || true
// RUN: cat %s.%must-version.log | %filecheck --check-prefix=CHECK-%must-version --implicit-check-not 'BAD TERMINATION' %s

// CHECK-clock_based-DAG: [MUST-REPORT] Error: from: call {{.*}}: Found concurrent MPI collectives that use the same communicator.
// CHECK-counter_based-DAG: [MUST-REPORT] Error:

#include "../../nondeterminism.h"
#include <mpi.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * the user had to make shure only one collective initialization is active at a time per process and communicator
 * according to the standard (see p303) If one reads the standard strictly, this is also forbidden with mixed
 * collectives
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

  // DEF_ORDER_CAPTURING_VARIABLES

  const int other_rank = size - myRank - 1;

#pragma omp parallel
  {
#pragma omp task
    {
      // CHECK_OVERLAP_BEGIN
      MPI_Barrier(MPI_COMM_WORLD);
      // CHECK_OVERLAP_END
    }
#pragma omp task
    {
      int *buffer = malloc(BUFFER_LENGTH_BYTE);
      // CHECK_OVERLAP_BEGIN
      MPI_Bcast(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD);
      // CHECK_OVERLAP_END
      free(buffer);
    }
  }  // end parallel

  // has_error_manifested(overlap_count != 0);
  MPI_Finalize();

  return 0;
}
