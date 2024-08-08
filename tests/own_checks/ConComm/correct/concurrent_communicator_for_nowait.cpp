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
// RUN: %s.exe 2>&1 > %s.log || true
// RUN: cat %s.log | %filecheck  --implicit-check-not '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' --implicit-check-not 'BAD TERMINATION' %s


#include <mpi.h>
#include <omp.h>
#include <vector>

int main(int argc, char** argv)
{
    int provided = -1;
    const int N = 10;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    std::vector<MPI_Comm> comms;
#pragma omp parallel
    {
        int tid = omp_get_thread_num();
#pragma omp single
        {
            for (int i = 0; i < omp_get_num_threads(); i++) {
                comms.emplace_back();
                MPI_Comm_dup(MPI_COMM_WORLD, &comms[i]);
            }
        }

        for (int j = 0; j < N; j++) {
#pragma omp for nowait schedule(static)
            for (int i = 0; i < omp_get_num_threads(); i++) {
                MPI_Barrier(comms[i]);
            }
        }
    }

    for (auto& comm : comms) {
        MPI_Comm_free(&comm);
    }
    MPI_Finalize();
    return 0;
}
