#include "benchmarks/benchmark_fastmap.h"

#include "benchmark.h"
#include "benchmarks/common.h"
#include "competitors/fastmap.h"

template <template <typename> typename Searcher>
void benchmark_32_fastmap_bucket(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                     bool pareto) {
  benchmark.template Run<FastMapBucket<uint32_t, 1>>();
  if (pareto) {
    benchmark.template Run<FastMapBucket<uint32_t, 2>>();
    benchmark.template Run<FastMapBucket<uint32_t, 4>>();
    benchmark.template Run<FastMapBucket<uint32_t, 8>>();
    benchmark.template Run<FastMapBucket<uint32_t, 16>>();
    benchmark.template Run<FastMapBucket<uint32_t, 32>>();
    benchmark.template Run<FastMapBucket<uint32_t, 64>>();
    benchmark.template Run<FastMapBucket<uint32_t, 256>>();
    benchmark.template Run<FastMapBucket<uint32_t, 1024>>();
    benchmark.template Run<FastMapBucket<uint32_t, 4096>>();
  }
}

template <template <typename> typename Searcher>
void benchmark_64_fastmap_bucket(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                     bool pareto) {
  benchmark.template Run<FastMapBucket<uint64_t, 1>>();
  if (pareto) {
    benchmark.template Run<FastMapBucket<uint64_t, 2>>();
    benchmark.template Run<FastMapBucket<uint64_t, 4>>();
    benchmark.template Run<FastMapBucket<uint64_t, 8>>();
    benchmark.template Run<FastMapBucket<uint64_t, 16>>();
    benchmark.template Run<FastMapBucket<uint64_t, 32>>();
    benchmark.template Run<FastMapBucket<uint64_t, 64>>();
    benchmark.template Run<FastMapBucket<uint64_t, 256>>();
    benchmark.template Run<FastMapBucket<uint64_t, 1024>>();
    benchmark.template Run<FastMapBucket<uint64_t, 4096>>();
  }
}

INSTANTIATE_TEMPLATES(benchmark_32_fastmap_bucket, uint32_t);
INSTANTIATE_TEMPLATES(benchmark_64_fastmap_bucket, uint64_t);
