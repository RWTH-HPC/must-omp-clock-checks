/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 *
 * Requests the threading level MPI_THREAD_MULTIPLE and tries to perform MPI barrier calls that DO NOT use the same communicator from multiple
 * threads that run concurrently.
 *
 * This is a test for the analysis group ThreadSanity.
 */


// RUN: mpicxx -g -fopenmp %s -o %s.exe 
// RUN: env OMP_NUM_THREADS=4 mustrun --must:mpimode MPMD --must:openmp --must:layout %root-dir/%omp-layout \
// RUN: %s.exe 2>&1 > %s.log || true
// RUN: cat %s.log | %filecheck --implicit-check-not 'BAD TERMINATION' %s

#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char** argv)
{
    int provided = -1;
    int size, rank;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#pragma omp parallel
    {
        int buf = 0;
        int tid = omp_get_thread_num();
        if (tid == 0) {
            buf = tid;
            MPI_Send(&buf, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);
        } else if (tid == 1) {
            usleep(1000000);  
            // CHECK-DAG: Error
            MPI_Recv(
                &buf,
                1,
                MPI_INT,
                (rank - 1 + size) % size,
                MPI_ANY_TAG,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
        } else if (tid == 2) {
           MPI_Probe((rank + 1) % size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    MPI_Finalize();
    return 0;
}
