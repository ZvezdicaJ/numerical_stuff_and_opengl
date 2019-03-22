#include "benchmarks_includes.hpp"
#include <chrono>

std::default_random_engine generator(std::time(0));
std::uniform_real_distribution<float> float_dist(-100, 100);
std::uniform_real_distribution<double> double_dist(-100, 100);
auto random_float = std::bind(float_dist, generator);
auto random_double = std::bind(double_dist, generator);

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
        BITONIC_SORT::sort_2n_vector(vec.data(), 0, vec.size() - 1);
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
        BITONIC_SORT::sort_8n_vector(vec.data(), 0, vec.size() - 1);
    }
    state.counters["Number to sort:"] = state.range(0);
    state.SetComplexityN(state.range(0));
}

BENCHMARK(bitonic_8n_float_sort_bench)
    ->RangeMultiplier(2)
    ->Range(8, 67108864)
    ->Complexity(benchmark::oN);
;

static void hybrid_8n_float_sort_bench(benchmark::State &state) {

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
        HYBRID_SORT::hybrid_sort_8n(vec, 0, vec.size() - 1);
    }
    state.counters["Number to sort:"] = state.range(0);
    state.SetComplexityN(state.range(0));
}

BENCHMARK(hybrid_8n_float_sort_bench)
    ->RangeMultiplier(2)
    ->Range(8, 67108864)
    ->Complexity(benchmark::oN);
;

// for the following two benches select values manually
static void CustomArguments(benchmark::internal::Benchmark *b) {
    std::vector<int> benches_to_run(
        {10,       20,      40,      50,      80,      100,      200,
         400,      800,     1600,    3200,    5000,    7500,     10000,
         12000,    15000,   17000,   20000,   25000,   27000,    30000,
         35000,    40000,   50000,   70000,   100000,  120000,   150000,
         200000,   400000,  600000,  800000,  1000000, 1200000,  1500000,
         1800000,  2000000, 2500000, 3000000, 5000000, (int)7e6, (int)1e7,
         (int)2e7, (int)5e7});
    for (int i = 0; i < benches_to_run.size(); i += 1)
        b->Args({benches_to_run[i]});
}
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
        BITONIC_SORT::sort_vector(vec, 0, vec.size() - 1);
    }
    state.counters["Number to sort:"] = state.range(0);
    state.SetComplexityN(state.range(0));
}

BENCHMARK(bitonic_float_sort_bench)->Apply(CustomArguments);

static void improved_bitonic_float_sort_bench(benchmark::State &state) {

    for (auto _ : state) {
        state.PauseTiming();
        aligned_vector<float> vec;
        vec.reserve(state.range(0));
        for (int i = 0; i < state.range(0); i++)
            vec.push_back(random_float());
        state.ResumeTiming();
        // benchmark::DoNotOptimize(sort_2n_vector(vec.data(), 0, vec.size() -
        // 1)); // DoNoOptimize will store the result to the memory
        IMPROVED_BITONIC_SORT::sort_vector(vec, 0, vec.size() - 1);
    }
    state.counters["Number to sort:"] = state.range(0);
    state.SetComplexityN(state.range(0));
}

BENCHMARK(improved_bitonic_float_sort_bench)->Apply(CustomArguments);

static void simd_QS_float_bench(benchmark::State &state) {

    for (auto _ : state) {
        state.PauseTiming();
        aligned_vector<float> vec;
        vec.reserve(state.range(0));
        for (int i = 0; i < state.range(0); i++)
            vec.push_back(random_float());
        state.ResumeTiming();
        // benchmark::DoNotOptimize(sort_2n_vector(vec.data(), 0, vec.size() -
        // 1)); // DoNoOptimize will store the result to the memory
        HYBRID_SORT::simd_QS(vec, 0, vec.size() - 1);
    }
    state.counters["Number to sort:"] = state.range(0);
    state.SetComplexityN(state.range(0));
}

BENCHMARK(simd_QS_float_bench)->Apply(CustomArguments);

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

/*
BENCHMARK(std_float_sort_bench)
    ->RangeMultiplier(2)
    ->Range(2, 1e8)
    ->RangeMultiplier(3)
    ->Range(3, 1e8)
    ->Complexity(benchmark::oN);
*/

BENCHMARK(std_float_sort_bench)->Apply(CustomArguments);

////////////////////////////////////////////////////////////////////////
//////  DOUBLE BENCHMARKS
/////////////////////////////////////////////////////////////////////

static void bitonic_2n_double_sort_bench(benchmark::State &state) {

    for (auto _ : state) {
        state.PauseTiming();
        aligned_vector<double> vec;
        vec.reserve(state.range(0));
        for (int i = 0; i < state.range(0); i++) {
            vec.push_back(random_double());
        }
        state.ResumeTiming();
        // benchmark::DoNotOptimize(sort_2n_vector(vec.data(), 0, vec.size() -
        // 1)); // DoNoOptimize will store the result to the memory
        BITONIC_SORT::sort_2n_vector(vec.data(), 0, vec.size() - 1);
    }
    state.counters["Number to sort:"] = state.range(0);
    state.SetComplexityN(state.range(0));
}
BENCHMARK(bitonic_2n_double_sort_bench)
    ->RangeMultiplier(2)
    ->Range(8, 67108864)
    ->Complexity(benchmark::oN);
;

static void bitonic_4n_double_sort_bench(benchmark::State &state) {

    for (auto _ : state) {
        state.PauseTiming();
        aligned_vector<double> vec;
        vec.reserve(state.range(0));
        for (int i = 0; i < state.range(0); i++) {
            vec.push_back(random_double());
        }
        state.ResumeTiming();
        // benchmark::DoNotOptimize(sort_2n_vector(vec.data(), 0, vec.size() -
        // 1)); // DoNoOptimize will store the result to the memory
        BITONIC_SORT::sort_4n_vector(vec.data(), 0, vec.size() - 1);
    }
    state.counters["Number to sort:"] = state.range(0);
    state.SetComplexityN(state.range(0));
}

BENCHMARK(bitonic_4n_double_sort_bench)
    ->RangeMultiplier(8)
    ->Range(8, 67108864)
    ->Complexity(benchmark::oN);
;

static void bitonic_double_sort_bench(benchmark::State &state) {

    for (auto _ : state) {
        state.PauseTiming();
        aligned_vector<double> vec;
        vec.reserve(state.range(0));
        for (int i = 0; i < state.range(0); i++)
            vec.push_back(random_double());
        state.ResumeTiming();
        // benchmark::DoNotOptimize(sort_2n_vector(vec.data(), 0, vec.size() -
        // 1)); // DoNoOptimize will store the result to the memory
        BITONIC_SORT::sort_vector(vec, 0, vec.size() - 1);
    }
    state.counters["Number to sort:"] = state.range(0);
    state.SetComplexityN(state.range(0));
}

BENCHMARK(bitonic_double_sort_bench)->Apply(CustomArguments);

/*
BENCHMARK(bitonic_float_sort_bench)
    ->RangeMultiplier(2)
    ->Range(2, 1e8)
    ->RangeMultiplier(3)
    ->Range(3, 1e8)
    ->Complexity(benchmark::oN);
*/

static void std_double_sort_bench(benchmark::State &state) {

    for (auto _ : state) {
        state.PauseTiming();
        aligned_vector<double> vec;
        vec.reserve(state.range(0));
        for (int i = 0; i < state.range(0); i++)
            vec.push_back(random_double());
        state.ResumeTiming();
        // benchmark::DoNotOptimize(sort_2n_vector(vec.data(), 0, vec.size() -
        // 1)); // DoNoOptimize will store the result to the memory
        std::sort(std::begin(vec), std::end(vec));
    }
    state.counters["Number to sort:"] = state.range(0);
    state.SetComplexityN(state.range(0));
}

/*
BENCHMARK(std_float_sort_bench)
    ->RangeMultiplier(2)
    ->Range(2, 1e8)
    ->RangeMultiplier(3)
    ->Range(3, 1e8)
    ->Complexity(benchmark::oN);
*/

BENCHMARK(std_double_sort_bench)->Apply(CustomArguments);

BENCHMARK_MAIN();
