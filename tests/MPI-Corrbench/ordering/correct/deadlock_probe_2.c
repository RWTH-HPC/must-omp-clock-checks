// RUN: mpicc -g -fopenmp %s -o %s.exe 
// RUN: env OMP_NUM_THREADS=2 mustrun --must:instant-logging warning --must:openmp --must:layout %root-dir/%omp-layout \
// RUN: %s.exe 2>&1 > %s.%must-version.log || true
// RUN: cat %s.%must-version.log | %filecheck  --implicit-check-not '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' --implicit-check-not 'BAD TERMINATION' %s

#include "../../nondeterminism.h"

#include <mpi.h>
#include <stdlib.h>

// Deadlock occurs, if the probe (marker "A") matches to the same message on both threads meaning that one thread will
// wait indefinitely for the message to arrive (at marker "B")

// Fix: use mprobe

int main(int argc, char *argv[]) {
  int provided;
  const int requested = MPI_THREAD_MULTIPLE;

  MPI_Init_thread(&argc, &argv, requested, &provided);
  if (provided < requested) {
    // has_error_manifested(false);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  int size;
  int rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

#pragma omp parallel
  {
    if (rank == 0) {
      DISTURB_THREAD_ORDER
#pragma omp sections
      {
#pragma omp section
        {
          int value[2] = {-1, -1};
          MPI_Send(&value[0], 2, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
#pragma omp section
        {
          int value = -2;
          MPI_Send(&value, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
        }
      }

    } else if (rank == 1) {
      MPI_Status status;
      MPI_Message message;
      MPI_Mprobe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &message, &status); /* A */

      int count;
      MPI_Get_count(&status, MPI_INT, &count);

      int *value = (int *)malloc(sizeof(int) * count);
      MPI_Mrecv(value, count, MPI_INT, &message, MPI_STATUS_IGNORE); /* B */

      free(value);
    }
  }

// #ifdef USE_DISTURBED_THREAD_ORDER
//   has_error_manifested(true);  // true is default, so if it deadlocks, it is ok
// #else
//   has_error_manifested(false);
// #endif

  MPI_Finalize();
  return 0;
}
