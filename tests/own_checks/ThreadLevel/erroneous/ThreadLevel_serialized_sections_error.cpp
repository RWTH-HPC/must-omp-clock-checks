/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * A test for usage of threading level MPI_THREAD_SERIALIZED.
 *
 * Requests the threading level MPI_THREAD_SERIALIZED and tries to perform MPI calls from multiple
 * threads that run concurrently.
 *
 * This is a test for the analysis group ThreadSanity.
 */

// RUN: mpicxx -g -fopenmp %s -o %s.exe 
// RUN: env OMP_NUM_THREADS=2 mustrun --must:mpimode MPMD --must:openmp --must:layout %root-dir/%omp-layout \
// RUN: %s.exe 2>&1 > %s.%must-version.log || true
// RUN: cat %s.%must-version.log | %filecheck --check-prefix=CHECK-%must-version --implicit-check-not 'BAD TERMINATION' %s

// CHECK-clock_based-DAG: [MUST-REPORT] Error: from: call {{[MPI_Isend@|MPI_Recv@|MPI_Wait@].*}}: Found concurrent mpi calls while MPI_THREAD_SERIALIZED was requested.
// CHECK-counter_based-DAG: [MUST-REPORT] Error: from: call {{[MPI_Isend@|MPI_Recv@|MPI_Wait@].*}}: Multiple threads call MPI functions simultaneously while you required MPI_THREAD_SERIALIZED.


#include <mpi.h>
#include <omp.h>

int main(int argc, char** argv)
{
    int provided = -1;
    int size, rank;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

#pragma omp parallel
    {
        int l_size=0;
#pragma omp sections
        {
        #pragma omp section
            {
                // CHECK-new-DAG: Error: from: call MPI_Comm_size@{{.*}}: Found concurrent mpi calls while MPI_THREAD_SERIALIZED was requested.
                
                // CHECK-old-DAG: Error: from: call MPI_Comm_size@{{.*}}: Multiple threads call MPI functions simultaneously while you required MPI_THREAD_SERIALIZED.
        
                MPI_Comm_size(MPI_COMM_WORLD, &l_size);
            }

        #pragma omp section
            {
                MPI_Comm_size(MPI_COMM_WORLD, &l_size);
            }
        }
    }
    MPI_Finalize();
    return 0;
}
