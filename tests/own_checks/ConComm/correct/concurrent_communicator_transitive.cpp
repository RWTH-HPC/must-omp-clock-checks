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
// RUN: env OMP_NUM_THREADS=4 mustrun --must:instant-logging warning --must:openmp --must:layout %root-dir/%omp-layout \
// RUN: %s.exe 2>&1 > %s.%must-version.log || true
// RUN: cat %s.%must-version.log | %filecheck --implicit-check-not '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' --implicit-check-not 'BAD TERMINATION' %s

#include <mpi.h>
#include <omp.h>

int main(int argc, char** argv)
{
    int provided = -1;
    int size, rank;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm comm1, comm2;
    MPI_Comm_dup(MPI_COMM_WORLD, &comm1);
    MPI_Comm_dup(MPI_COMM_WORLD, &comm2);

    if (rank == 0) {
#pragma omp parallel
        {
            int tid = omp_get_thread_num();
            if (tid == 0) {
                MPI_Barrier(comm1);
            } else if (tid == 1) {
                MPI_Barrier(comm2);
                MPI_Barrier(comm1);
            }
        }
    } else if (rank == 1) {
        MPI_Barrier(comm1);
        MPI_Barrier(comm2);
        MPI_Barrier(comm1);
    }

    MPI_Finalize();
    return 0;
}
