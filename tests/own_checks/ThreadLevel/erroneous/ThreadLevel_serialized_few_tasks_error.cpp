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
// RUN: env OMP_NUM_THREADS=8 mustrun --must:mpimode MPMD --must:openmp --must:layout %root-dir/%omp-layout \
// RUN: %s.exe 2>&1 > %s.log || true
// RUN: cat %s.log | %filecheck --check-prefix=CHECK-%must-version --implicit-check-not 'BAD TERMINATION' %s

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
    #pragma omp task
        {
            int l_size = 0;
            // CHECK-new-DAG: Error: from: call MPI_Comm_size@{{.*}}: Found concurrent mpi calls while MPI_THREAD_SERIALIZED was requested.    
            // CHECK-old-DAG: Error: from: call MPI_Comm_size@{{.*}}: Multiple threads call MPI functions simultaneously while you required MPI_THREAD_SERIALIZED.
        
            MPI_Comm_size(MPI_COMM_WORLD, &l_size);
        }

    #pragma omp task
        {
            int l_size = 0;
            MPI_Comm_size(MPI_COMM_WORLD, &l_size);
        }
    }
    MPI_Finalize();
    return 0;
}
