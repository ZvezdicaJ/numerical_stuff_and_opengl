#!/bin/bash


filepath=$(dirname "$0")
double_filename="double_sort_benchmarks.csv"
float_filename="float_sort_benchmarks.csv"

#echo "writing to file: ${filepath}/${filename}"

if [ "$1" == "--all" ] || [ "$1" == "" ] ; then
    # read more here https://unix.stackexchange.com/questions/424602/is-it-possible-to-set-a-constant-lowest-cpu-frequency-under-the-modern-pstate
    sudo cpupower frequency-set --governor performance
    sudo cpupower frequency-set -u 2000MHz

    ../bin/sort_double_benchmarks --benchmark_out="${filepath}/${double_filename}" --benchmark_counters_tabular=true --benchmark_out_format=csv

    ../bin/sort_float_benchmarks --benchmark_out="${filepath}/${float_filename}" --benchmark_counters_tabular=true --benchmark_out_format=csv

    sudo cpupower frequency-set --governor powersave

    exit 1
fi

if [ "$1" == "--double" ] || [ "$1" == "" ] ; then
    sudo cpupower frequency-set --governor performance
    sudo cpupower frequency-set -u 2000MHz

    ../bin/sort_double_benchmarks --benchmark_out="${filepath}/${double_filename}" --benchmark_counters_tabular=true --benchmark_out_format=csv

    sudo cpupower frequency-set --governor powersave

    exit 1
fi

if [ "$1" == "--float" ] || [ "$1" == "" ] ; then
    sudo cpupower frequency-set --governor performance
    sudo cpupower frequency-set -u 2000MHz

    ../bin/sort_double_benchmarks --benchmark_out="${filepath}/${float_filename}" --benchmark_counters_tabular=true --benchmark_out_format=csv

    ../bin/sort_float_benchmarks --benchmark_out="${filepath}/${float_filename}" --benchmark_counters_tabular=true --benchmark_out_format=csv

    sudo cpupower frequency-set --governor powersave

    exit 1
fi

