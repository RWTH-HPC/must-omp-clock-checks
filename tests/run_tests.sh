#!/bin/bash

parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd "$parent_path"

corrbench_true_positives_counter_based=0
corrbench_true_negatives_counter_based=0
corrbench_false_positives_counter_based=0
corrbench_false_negatives_counter_based=0
corrbench_true_positives_clock_based=0
corrbench_true_negatives_clock_based=0
corrbench_false_positives_clock_based=0
corrbench_false_negatives_clock_based=0


corrbench_correct_dirs=(
    "./MPI-Corrbench/ordering/correct"
    "./MPI-Corrbench/threading/correct"
)

corrbench_erroneous_dirs=(
    "./MPI-Corrbench/ordering/erroneous"
    "./MPI-Corrbench/threading/erroneous"
)

own_true_positives_counter_based=0
own_true_negatives_counter_based=0
own_false_positives_counter_based=0
own_false_negatives_counter_based=0
own_true_positives_clock_based=0
own_true_negatives_clock_based=0
own_false_positives_clock_based=0
own_false_negatives_clock_based=0

own_correct_dirs=(
    "./own_checks/ConRecv/correct"
    "./own_checks/ConComm/correct"
    "./own_checks/RecvProbe/correct"
    "./own_checks/ThreadLevel/correct"
)

own_erroneous_dirs=(
    "./own_checks/ConComm/erroneous"
    "./own_checks/ConRecv/erroneous"
    "./own_checks/RecvProbe/erroneous"
    "./own_checks/ThreadLevel/erroneous"
)

for version in "counter_based" "clock_based"; do
    corrbench_true_negatives=0
    corrbench_true_positives=0
    corrbench_false_negatives=0
    corrbench_false_positives=0
    own_true_negatives=0
    own_true_positives=0
    own_false_negatives=0
    own_false_positives=0
    # Correct test cases    
    echo "==============================================="
    echo "Running correct test cases for the $version version"
    echo "==============================================="
    for dir in "${corrbench_correct_dirs[@]}"; do
        output=$(LIT_MUST_VERSION=$version lit "$dir" -j1 2>&1 | tee /dev/tty)
        corrbench_true_negatives=$((corrbench_true_negatives + $(echo "$output" | grep -oP '(?<=\bPassed: )\s*\d+' || echo 0)))
        corrbench_false_positives=$((corrbench_false_positives + $(echo "$output" | grep -oP '(?<=\bFailed: )\s*\d+' || echo 0)))
    done
    for dir in "${own_correct_dirs[@]}"; do
        output=$(LIT_MUST_VERSION=$version lit "$dir" -j1 2>&1 | tee /dev/tty)
        own_true_negatives=$((own_true_negatives + $(echo "$output" | grep -oP '(?<=\bPassed: )\s*\d+' || echo 0)))
        own_false_positives=$((own_false_positives + $(echo "$output" | grep -oP '(?<=\bFailed: )\s*\d+' || echo 0)))
    done


    # Erroneous test cases
    echo "==============================================="
    echo "Running erroneous test cases for the $version version"
    echo "==============================================="
    for dir in "${corrbench_erroneous_dirs[@]}"; do
        output=$(LIT_MUST_VERSION=$version lit "$dir" -j1 2>&1 | tee /dev/tty)
        corrbench_true_positives=$((corrbench_true_positives + $(echo "$output" | grep -oP '(?<=\bPassed: )\s*\d+' || echo 0)))
        corrbench_false_negatives=$((corrbench_false_negatives + $(echo "$output" | grep -oP '(?<=\bFailed: )\s*\d+' || echo 0)))
    done
    for dir in "${own_erroneous_dirs[@]}"; do
        output=$(LIT_MUST_VERSION=$version lit "$dir" -j1 2>&1 | tee /dev/tty)
        own_true_positives=$((own_true_positives + $(echo "$output" | grep -oP '(?<=\bPassed: )\s*\d+' || echo 0)))
        own_false_negatives=$((own_false_negatives + $(echo "$output" | grep -oP '(?<=\bFailed: )\s*\d+' || echo 0)))
    done

    eval "corrbench_true_negatives_$version=\$corrbench_true_negatives"
    eval "corrbench_true_positives_$version=\$corrbench_true_positives"
    eval "corrbench_false_negatives_$version=\$corrbench_false_negatives"
    eval "corrbench_false_positives_$version=\$corrbench_false_positives"
    
    eval "own_true_negatives_$version=\$own_true_negatives"
    eval "own_true_positives_$version=\$own_true_positives"
    eval "own_false_negatives_$version=\$own_false_negatives"
    eval "own_false_positives_$version=\$own_false_positives"
done

echo "========================="
echo "Counter-based Must Version"
echo "========================="
corrbench_total_counter_based=$((corrbench_true_negatives_counter_based + corrbench_true_positives_counter_based + corrbench_false_negatives_counter_based + corrbench_false_positives_counter_based))
own_total_counter_based=$((own_true_negatives_counter_based + own_true_positives_counter_based + own_false_negatives_counter_based + own_false_positives_counter_based))
echo "Test Suite;Total;TP;FP;TN;FN"
echo "Own;$own_total_counter_based;$own_true_positives_counter_based;$own_false_positives_counter_based;$own_true_negatives_counter_based;$own_false_negatives_counter_based" 
echo "Corrbench;$corrbench_total_counter_based;$corrbench_true_positives_counter_based;$corrbench_false_positives_counter_based;$corrbench_true_negatives_counter_based;$corrbench_false_negatives_counter_based" 

echo "========================="
echo "Clock-based Must Version"
echo "========================="
corrbench_total_clock_based=$((corrbench_true_negatives_clock_based + corrbench_true_positives_clock_based + corrbench_false_negatives_clock_based + corrbench_false_positives_clock_based))
own_total_clock_based=$((own_true_negatives_clock_based + own_true_positives_clock_based + own_false_negatives_clock_based + own_false_positives_clock_based))
echo "Test Suite;Total;TP;FP;TN;FN"
echo "Own;$own_total_clock_based;$own_true_positives_clock_based;$own_false_positives_clock_based;$own_true_negatives_clock_based;$own_false_negatives_clock_based" 
echo "Corrbench;$corrbench_total_clock_based;$corrbench_true_positives_clock_based;$corrbench_false_positives_clock_based;$corrbench_true_negatives_clock_based;$corrbench_false_negatives_clock_based" 


