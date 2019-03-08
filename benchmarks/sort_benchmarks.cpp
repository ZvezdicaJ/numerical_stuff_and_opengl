#include "benchmarks_includes.hpp"
#include <chrono>

std::default_random_engine generator(std::time(0));
std::uniform_real_distribution<float> float_dist(-100, 100);
std::uniform_real_distribution<double> double_dist(-100, 100);
auto random_float = std::bind(float_dist, generator);
auto random_double = std::bind(double_dist, generator);

using namespace BITONIC_SORT;

static void bitonic_2n_float_sort_bench(benchmark::State &state) {

    for (auto _ : state) {
        state.PauseTiming();
        aligned_vector<float> vec;
        vec.reserve(state.range(0));
        for (int i = 0; i < state.range(0); i++) {
            vec.push_back(random_float());
        }
        state.ResumeTiming();
        // benchmark::DoNotOptimize(sort_2n_vector(vec.data(), 0, vec.size() -
        // 1)); // DoNoOptimize will store the result to the memory
        sort_2n_vector(vec.data(), 0, vec.size() - 1);
    }
    state.counters["Number to sort:"] = state.range(0);
    state.SetComplexityN(state.range(0));
}
BENCHMARK(bitonic_2n_float_sort_bench)
    ->RangeMultiplier(2)
    ->Range(8, 67108864)
    ->Complexity(benchmark::oN);
;

static void bitonic_8n_float_sort_bench(benchmark::State &state) {

    for (auto _ : state) {
        state.PauseTiming();
        aligned_vector<float> vec;
        vec.reserve(state.range(0));
        for (int i = 0; i < state.range(0); i++) {
            vec.push_back(random_float());
        }
        state.ResumeTiming();
        // benchmark::DoNotOptimize(sort_2n_vector(vec.data(), 0, vec.size() -
        // 1)); // DoNoOptimize will store the result to the memory
        sort_8n_vector(vec.data(), 0, vec.size() - 1);
    }
    state.counters["Number to sort:"] = state.range(0);
    state.SetComplexityN(state.range(0));
}

BENCHMARK(bitonic_8n_float_sort_bench)
    ->RangeMultiplier(8)
    ->Range(8, 67108864)
    ->Complexity(benchmark::oN);
;

static void bitonic_float_sort_bench(benchmark::State &state) {

    for (auto _ : state) {
        state.PauseTiming();
        aligned_vector<float> vec;
        vec.reserve(state.range(0));
        for (int i = 0; i < state.range(0); i++)
            vec.push_back(random_float());
        state.ResumeTiming();
        // benchmark::DoNotOptimize(sort_2n_vector(vec.data(), 0, vec.size() -
        // 1)); // DoNoOptimize will store the result to the memory
        sort_vector(vec, 0, vec.size() - 1);
    }
    state.counters["Number to sort:"] = state.range(0);
    state.SetComplexityN(state.range(0));
}

BENCHMARK(bitonic_float_sort_bench)
    ->RangeMultiplier(2)
    ->Range(8, 67108864)
    ->Complexity(benchmark::oN);
;

static void std_float_sort_bench(benchmark::State &state) {

    for (auto _ : state) {
        state.PauseTiming();
        aligned_vector<float> vec;
        vec.reserve(state.range(0));
        for (int i = 0; i < state.range(0); i++)
            vec.push_back(random_float());
        state.ResumeTiming();
        // benchmark::DoNotOptimize(sort_2n_vector(vec.data(), 0, vec.size() -
        // 1)); // DoNoOptimize will store the result to the memory
        std::sort(std::begin(vec), std::end(vec));
    }
    state.counters["Number to sort:"] = state.range(0);
    state.SetComplexityN(state.range(0));
}

BENCHMARK(std_float_sort_bench)
    ->RangeMultiplier(2)
    ->Range(8, 67108864)
    ->Complexity(benchmark::oN);
;

BENCHMARK_MAIN();
