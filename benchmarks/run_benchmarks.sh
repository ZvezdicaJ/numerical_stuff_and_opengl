#!/bin/bash


if [ "$1" == "-all" ] || [ "$1" == "" ] ; then
    #sudo cpupower frequency-set --governor performance
    sudo cpupower frequency-set --governor powersave
    ../bin/sort_benchmarks --benchmark_out="/home/jure/Documents/opengl_ucenje/benchmarks/benchmarks.csv" --benchmark_counters_tabular=true --benchmark_out_format=csv
    sudo cpupower frequency-set --governor powersave
    exit 1
fi

