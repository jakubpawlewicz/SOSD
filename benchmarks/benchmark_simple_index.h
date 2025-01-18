#pragma once
#include "benchmark.h"

template <template <typename> typename Searcher>
void benchmark_32_simple_index(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                      bool pareto);

template <template <typename> typename Searcher>
void benchmark_64_simple_index(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                      bool pareto);

template <template <typename> typename Searcher>
void benchmark_32_bitwise_index(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                      bool pareto);

template <template <typename> typename Searcher>
void benchmark_64_bitwise_index(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                      bool pareto);

template <template <typename> typename Searcher>
void benchmark_32_greedy_index(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                      bool pareto);

template <template <typename> typename Searcher>
void benchmark_64_greedy_index(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                      bool pareto);
