#!/bin/bash


filepath=$(dirname "$file")
#echo $filepath

if [ "$1" == "-all" ] || [ "$1" == "" ] ; then
    # read more here https://unix.stackexchange.com/questions/424602/is-it-possible-to-set-a-constant-lowest-cpu-frequency-under-the-modern-pstate
    sudo cpupower frequency-set --governor performance
    sudo cpupower frequency-set -u 2000MHz
    ../bin/sort_benchmarks --benchmark_out="${filepath}/benchmarks.csv" --benchmark_counters_tabular=true --benchmark_out_format=csv
    sudo cpupower frequency-set --governor powersave
    exit 1
fi

