# Towards Correctness Checking of MPI Partitioned Communication in MUST - Artifact

This repository contains the artifact with the test cases for the paper *Correctness Checking of MPI+OpenMP Applications Using Vector Clocks in MUST*. The test cases used are in the [tests/](tests/) folder and are grouped by MPI-CorrBench and our own test suite, as well as correct and erroneous test cases.

## Flaky Test Cases
Some test cases are not uniquely classified. For the counter-based MPI-Serialized analysis, the error has to manifest to be detected, meaning any of the erroneous thread-level test cases could be classified as TP or FN. This includes the four test cases in our own test suite:
 - `ThreadLevel_serialized_error.cpp`
 - `ThreadLevel_serialized_few_tasks_error.cpp`
 - `ThreadLevel_serialized_many_tasks_error.cpp`
 - `ThreadLevel_serialized_sections_error.cpp`

And the single test case from MPI-CorrBench:
 - `wrong_threading_level_3.c`

The `ThreadLevel_serialized_many_tasks_error.cpp` test case performs many concurrent MPI calls and is more consistently classified as TP. The other four test cases are not consistently classified as TP or FN.

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