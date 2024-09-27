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
// RUN: env OMP_NUM_THREADS=4 mustrun --must:instant-logging warning --must:openmp --must:layout %root-dir/%omp-layout \
// RUN: %s.exe 2>&1 > %s.%must-version.log || true
// RUN: cat %s.%must-version.log | %filecheck --check-prefix=CHECK-%must-version --implicit-check-not 'BAD TERMINATION' %s

// CHECK-clock-DAG: [MUST-REPORT] Warning: from: call MPI_Recv@{{.*}}: Found concurrent MPI receives
// CHECK-counter-DAG: [MUST-REPORT] {{[Warning|Error]}}:

#include <mpi.h>
#include <omp.h>
#include <vector>
#include <iostream>

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
        if (tid % 2 == 0) {
            buf = tid;
            MPI_Send(&buf, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);
        } else {
            MPI_Recv(
                &buf,
                1,
                MPI_INT,
                (rank - 1 + size) % size,
                0,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
#pragma omp critical
            {
                std::cout << "[" << tid << "] received from " << buf << std::endl;
            }
        }
    }

    MPI_Finalize();
    return 0;
}
