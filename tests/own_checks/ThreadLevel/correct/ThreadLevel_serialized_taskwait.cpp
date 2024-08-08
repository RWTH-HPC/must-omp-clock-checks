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
// RUN: env OMP_NUM_THREADS=4 mustrun --must:mpimode MPMD --must:openmp --must:layout %root-dir/%omp-layout \
// RUN: %s.exe 2>&1 > %s.log || true
// RUN: cat %s.log | %filecheck --implicit-check-not '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' --implicit-check-not 'BAD TERMINATION' %s

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
#pragma omp single
        {
#pragma omp task
            {
                MPI_Barrier(MPI_COMM_WORLD);
            }
#pragma omp taskwait
#pragma omp task
            {
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
    }
    MPI_Finalize();
    return 0;
}
