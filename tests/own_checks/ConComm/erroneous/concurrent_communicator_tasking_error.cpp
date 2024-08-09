/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 *
 * Requests the threading level MPI_THREAD_MULTIPLE and tries to perform MPI barrier calls  within omp tasks that DO NOT use the same communicator from multiple
 * threads that run concurrently.
 *
 * This is a test for the analysis group ThreadSanity.
 */


// RUN: mpicxx -g -fopenmp %s -o %s.exe 
// RUN: env OMP_NUM_THREADS=4 mustrun --must:mpimode MPMD --must:openmp --must:layout %root-dir/%omp-layout \
// RUN: %s.exe 2>&1 > %s.%must-version.log || true
// RUN: cat %s.%must-version.log | %filecheck --check-prefix=CHECK-%must-version --implicit-check-not 'BAD TERMINATION' %s

// CHECK-clock_based-DAG: [MUST-REPORT] Error: from: call MPI_Barrier@{{.*}}: Found concurrent MPI collectives that use the same communicator.
// CHECK-counter_based-DAG: [MUST-REPORT] Error:

#include <mpi.h>
#include <omp.h>

int main(int argc, char** argv)
{
    int provided = -1;
    const int N = 200;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

#pragma omp parallel
    {
        for (int i = 0; i < N; i++)
#pragma omp task
        {
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
