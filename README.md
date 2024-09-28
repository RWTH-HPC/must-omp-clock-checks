# Towards Correctness Checking of MPI Partitioned Communication in MUST - Artifact

This repository contains the artifact with the test cases for the paper *Correctness Checking of MPI+OpenMP Applications Using Vector Clocks in MUST*. The test cases used are in the [tests/](tests/) folder and are grouped by MPI-CorrBench (COBE) and our own test suite, as well as correct and erroneous test cases.

## Flaky Test Cases
Some test cases are not uniquely classified. For the counter-based MPI-Serialized analysis, the error has to manifest to be detected, meaning any of the erroneous thread-level test cases could be classified as TP or FN. This includes the four test cases in our own test suite:
 - `ThreadLevel_serialized_error.cpp`
 - `ThreadLevel_serialized_few_tasks_error.cpp`
 - `ThreadLevel_serialized_many_tasks_error.cpp`
 - `ThreadLevel_serialized_sections_error.cpp`

And the single test case from MPI-CorrBench:
 - `wrong_threading_level_3.c`

The `ThreadLevel_serialized_many_tasks_error.cpp` test case performs many concurrent MPI calls and is more consistently classified as TP. The other four test cases are not consistently classified as TP or FN.

## False Positive Test Case'
The test case `own_checks/ConComm/correct/concurrent_communicator_transitive.cpp` is a correct test for the concurrent collectives that use the same communicator error class, that uses MPI synchronization to synchronize the threads of the first process transitively. Since the clock-based approach currently does not track the MPI synchronization it also does not recognize this transitive synchronization and reports this test case as erroneous even though the execution of the collectives that use the same communicator are not concurrent, and hence clock-based approach classifies this test case as (FP).
MUST does not have a counter-based approach for this error class and hence classifies this test case as TN.

## Classification Results

 - TP: True Positive (correct-alert)
 - FP: False Positive (false-alert)
 - TN: True Negative (error-free)
 - FN: False Negative (missing-alert)
 - A: Accuracy

<table border="1" class="dataframe">
  <thead>
    <tr>
      <th>Analyses</th>
      <th>Test Suite</th>
      <th>Total</th>
      <th>TP</th>
      <th>FP</th>
      <th>TN</th>
      <th>FN</th>
      <th>A</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>Counter-based</th>
      <th>Own</th>
      <td>30</td>
      <td>1</td>
      <td>0</td>
      <td>17</td>
      <td>12</td>
      <td>0.60</td>
    </tr>
    <tr>
      <th>Clock-based</th>
      <th>Own</th>
      <td>30</td>
      <td>13</td>
      <td>1</td>
      <td>16</td>
      <td>0</td>
      <td>0.97</td>
    </tr>
    <tr>
      <th>Counter-based</th>
      <th>COBE</th>
      <td>20</td>
      <td>0</td>
      <td>0</td>
      <td>10</td>
      <td>10</td>
      <td>0.50</td>
    </tr>
    <tr>
      <th>Clock-based</th>
      <th>COBE</th>
      <td>20</td>
      <td>10</td>
      <td>0</td>
      <td>10</td>
      <td>0</td>
      <td>1.00</td>
    </tr>
  </tbody>
</table>

## Detailed Results


<table border="1" class="dataframe">
  <thead>
    <tr>
      <th>Test Suite</th>
      <th>Test</th>
      <th>Error class</th>
      <th>Type</th>
      <th>Counter-based</th>
      <th>Clock-based</th>
    </tr>
  </thead>
  <tbody>    
    <tr>
      <th>COBE</th>
      <th>deadlock_probe_2</th>
      <th>Probe</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>probe</th>
      <th>Probe</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>two_collectives</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>two_collectives_2</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>two_collectives_3</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>two_collectives_4</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>two_collectives_5</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>two_collectives_6</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>two_collectives_7</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>two_collectives</th>
      <th>Thread-serialized</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>deadlock_probe_2</th>
      <th>Probe</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>probe</th>
      <th>Probe</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>two_collectives</th>
      <th>Collectives</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>two_collectives_2</th>
      <th>Collectives</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>two_collectives_3</th>
      <th>Collectives</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>two_collectives_4</th>
      <th>Collectives</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>two_collectives_5</th>
      <th>Collectives</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>two_collectives_6</th>
      <th>Collectives</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>two_collectives_7</th>
      <th>Collectives</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    <tr>
      <th>COBE</th>
      <th>wrong_threading_level_3</th>
      <th>Thread-serialized</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    <tr>
      <th>Own</th>
      <th>concurrent_recv</th>
      <th>Receive</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>Own</th>
      <th>concurrent_communicator</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>Own</th>
      <th>concurrent_communicator_for_dynamic</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>Own</th>
      <th>concurrent_communicator_for_nowait</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>Own</th>
      <th>concurrent_communicator_sections</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>Own</th>
      <th>concurrent_communicator_single</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>Own</th>
      <th>concurrent_communicator_task_nesting</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>Own</th>
      <th>concurrent_communicator_tasking_depend</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>Own</th>
      <th>concurrent_communicator_tasking_locks</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    <tr>
      <th>Own</th>
      <th>concurrent_communicator_transitive</th>
      <th>Collective</th>
      <th>Correct</th>
      <th>TN</th>
      <th>FP</th>
    </tr>
    <tr>
      <th>Own</th>
      <th>concurrent_recv_probe</th>
      <th>Probe</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
      <th>Own</th>
      <th>concurrent_recv_probe_any_tag</th>
      <th>Probe</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>ThreadLevel_serialized</th>
      <th>Thread-serialized</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>ThreadLevel_serialized_single</th>
      <th>Thread-serialized</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>ThreadLevel_serialized_tasking_depend</th>
      <th>Thread-serialized</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>ThreadLevel_serialized_tasking_locks</th>
      <th>Thread-serialized</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>ThreadLevel_serialized_taskwait</th>
      <th>Thread-serialized</th>
      <th>Correct</th>
      <th>TN</th>
      <th>TN</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>concurrent_recv_any_tag_error</th>
      <th>Receive</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>concurrent_recv_error</th>
      <th>Receive</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
      <th>Own</th>
      <th>concurrent_communicator_error</th>
      <th>Collective</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>concurrent_communicator_for_dynamic_nowait_error</th>
      <th>Collective</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>concurrent_communicator_sections_error</th>
      <th>Collective</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>concurrent_communicator_task_nesting_error</th>
      <th>Collective</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>concurrent_communicator_tasking_error</th>
      <th>Collective</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>concurrent_recv_probe_any_tag_error</th>
      <th>Probe</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>concurrent_recv_probe_error</th>
      <th>Probe</th>
      <th>Erroneous</th>
      <th>FN</th>
      <th>TP</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>ThreadLevel_serialized_error</th>
      <th>Thread-serialized</th>
      <th>Erroneous</th>
      <th>Flaky (mostly FN)</th>
      <th>TP</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>ThreadLevel_serialized_few_tasks_error</th>
      <th>Thread-serialized</th>
      <th>Erroneous</th>
      <th>Flaky (mostly FN)</th>
      <th>TP</th>
    </tr>
      <th>Own</th>
      <th>ThreadLevel_serialized_many_tasks_error</th>
      <th>Thread-serialized</th>
      <th>Erroneous</th>
      <th>TP (inherently flaky)</th>
      <th>TP</th>
    </tr>
    </tr>
      <th>Own</th>
      <th>ThreadLevel_serialized_sections_error</th>
      <th>Thread-serialized</th>
      <th>Erroneous</th>
      <th>Flaky (mostly FN)</th>
      <th>TP</th>
    </tr>
  </tbody>
</table>




## Building Docker Container
The docker container installs MPICH v4.0.2, Clang v15.0.6, MUST, and LLVM lit.
To build the container, run in the root directory:

```
docker build -t must-omp-clock .
```

## Running Tests
To run all test cases for the counter-based and clock-based analyses, run:

```
docker run -it must-omp-clock ./tests/run_tests.sh
```
(Make sure that `run_tests.sh` uses LF line endings and not CRLF, otherwise Linux will provide the error message "No such file or directory")

To start a specific test case, run:
```
docker run -e LIT_MUST_VERSION=<analysis-type> -it must-omp-clock lit -j1 -a <path-to-test>
```