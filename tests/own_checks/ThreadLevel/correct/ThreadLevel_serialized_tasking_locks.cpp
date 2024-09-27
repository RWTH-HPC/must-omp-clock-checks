/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * A test for usage of threading level MPI_THREAD_SERIALIZED.
 *
 * Requests the threading level MPI_THREAD_SERIALIZED and correctly performs all MPI calls in
 * critical sections.
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
    int thread_support, size, rank;
    int N = 200;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    omp_lock_t mylock;
    omp_init_lock(&mylock);

#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int send_buffer = tid;
        int recv_buffer = 0;
        MPI_Request request;
        for (int i = 0; i < N; i++) {
#pragma omp task
            {
                int l_size = -1;
                omp_set_lock(&mylock);
                MPI_Comm_size(MPI_COMM_WORLD, &l_size);
                omp_unset_lock(&mylock);
            }
        }
    }
    omp_destroy_lock(&mylock);
    MPI_Finalize();
    return 0;
}
