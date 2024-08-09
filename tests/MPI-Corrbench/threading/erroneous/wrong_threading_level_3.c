
// RUN: mpicc -g -fopenmp %s -o %s.exe 
// RUN: env OMP_NUM_THREADS=2 mustrun --must:mpimode MPMD --must:openmp --must:layout %root-dir/%omp-layout \
// RUN: %s.exe 2>&1 > %s.%must-version.log || true
// RUN: cat %s.%must-version.log | %filecheck --check-prefix=CHECK-%must-version --implicit-check-not 'BAD TERMINATION' %s

// CHECK-clock_based-DAG: [MUST-REPORT] Error: from: call {{[MPI_Isend@|MPI_Recv@].*}}: Found concurrent mpi calls while MPI_THREAD_SERIALIZED was requested.
// CHECK-counter_based-DAG: [MUST-REPORT] Error: from: call {{[MPI_Isend@|MPI_Recv@].*}}: Multiple threads call MPI functions simultaneously while you required MPI_THREAD_SERIALIZED.

#include <mpi.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * A Threaded Program need to use  MPI_THREAD_MULTIPLE
 */
int main(int argc, char *argv[]) {
  int myRank;
  int buffer_out[10], buffer_in[10];
  int provided;
  const int requested = MPI_THREAD_SERIALIZED;

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
#pragma omp sections
    {
#pragma omp section
      {
        if (myRank == 0) {
          MPI_Send(buffer_out, 10, MPI_INT, 1, 123, MPI_COMM_WORLD);
        } else if (myRank == 1) {
          MPI_Recv(buffer_in, 10, MPI_INT, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
      }
#pragma omp section
      if (myRank == 1) {
        MPI_Send(buffer_out, 10, MPI_INT, 0, 123, MPI_COMM_WORLD);
      } else if (myRank == 0) {
        MPI_Recv(buffer_in, 10, MPI_INT, 1, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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
