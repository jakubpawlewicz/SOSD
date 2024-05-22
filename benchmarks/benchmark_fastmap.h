#pragma once
#include "benchmark.h"

template <template <typename> typename Searcher>
void benchmark_32_fastmap_bucket(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                     bool pareto);

template <template <typename> typename Searcher>
void benchmark_64_fastmap_bucket(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                     bool pareto);

template <template <typename> typename Searcher>
void benchmark_32_fastmap_regbucket_single(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                     bool pareto);

template <template <typename> typename Searcher>
void benchmark_64_fastmap_regbucket_single(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                     bool pareto);

template <template <typename> typename Searcher>
void benchmark_32_fastmap_regbucket_double(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                     bool pareto);

template <template <typename> typename Searcher>
void benchmark_64_fastmap_regbucket_double(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                     bool pareto);

template <template <typename> typename Searcher>
void benchmark_32_fastmap_pgmregbucket_single(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                     bool pareto);

template <template <typename> typename Searcher>
void benchmark_64_fastmap_pgmregbucket_single(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                     bool pareto);

template <template <typename> typename Searcher>
void benchmark_32_fastmap_pgmregbucket_double(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                     bool pareto);

template <template <typename> typename Searcher>
void benchmark_64_fastmap_pgmregbucket_double(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                     bool pareto);

template <template <typename> typename Searcher>
void benchmark_32_fastmap_pgmfull(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                     bool pareto);

template <template <typename> typename Searcher>
void benchmark_64_fastmap_pgmfull(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                     bool pareto);

template <template <typename> typename Searcher>
void benchmark_32_fastmap_pgmbucket(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                     bool pareto);

template <template <typename> typename Searcher>
void benchmark_64_fastmap_pgmbucket(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                     bool pareto);

template <template <typename> typename Searcher>
void benchmark_32_fastmap_apx(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                     bool pareto);

template <template <typename> typename Searcher>
void benchmark_64_fastmap_apx(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                     bool pareto);
