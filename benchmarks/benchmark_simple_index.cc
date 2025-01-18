#include "benchmarks/benchmark_simple_index.h"

#include "benchmark.h"
#include "benchmarks/common.h"
#include "competitors/greedy_partition_bs.h"

template <template <typename> typename Searcher>
void benchmark_32_simple_index(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                      bool pareto) {
  benchmark.template Run<SimpleBucketIndex<uint32_t, 16>>();
  if (pareto) {
    benchmark.template Run<SimpleBucketIndex<uint32_t, 4>>();
    benchmark.template Run<SimpleBucketIndex<uint32_t, 8>>();
    benchmark.template Run<SimpleBucketIndex<uint32_t, 32>>();
    benchmark.template Run<SimpleBucketIndex<uint32_t, 64>>();
    benchmark.template Run<SimpleBucketIndex<uint32_t, 256>>();
    benchmark.template Run<SimpleBucketIndex<uint32_t, 1024>>();
    benchmark.template Run<SimpleBucketIndex<uint32_t, 2048>>();
    if (benchmark.uses_binary_search()) {
      benchmark.template Run<SimpleBucketIndex<uint32_t, 4096>>();
      benchmark.template Run<SimpleBucketIndex<uint32_t, 8192>>();
    }
  }
}

template <template <typename> typename Searcher>
void benchmark_64_simple_index(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                      bool pareto) {
  benchmark.template Run<SimpleBucketIndex<uint64_t, 16>>();
  if (pareto) {
    benchmark.template Run<SimpleBucketIndex<uint64_t, 4>>();
    benchmark.template Run<SimpleBucketIndex<uint64_t, 8>>();
    benchmark.template Run<SimpleBucketIndex<uint64_t, 32>>();
    benchmark.template Run<SimpleBucketIndex<uint64_t, 64>>();
    benchmark.template Run<SimpleBucketIndex<uint64_t, 256>>();
    benchmark.template Run<SimpleBucketIndex<uint64_t, 1024>>();
    benchmark.template Run<SimpleBucketIndex<uint64_t, 2048>>();
    if (benchmark.uses_binary_search()) {
      benchmark.template Run<SimpleBucketIndex<uint64_t, 4096>>();
      benchmark.template Run<SimpleBucketIndex<uint64_t, 8192>>();
    }
  }
}

INSTANTIATE_TEMPLATES(benchmark_32_simple_index, uint32_t);
INSTANTIATE_TEMPLATES(benchmark_64_simple_index, uint64_t);

template <template <typename> typename Searcher>
void benchmark_32_bitwise_index(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                      bool pareto) {
  benchmark.template Run<BitwiseBucketIndex<uint32_t, 16>>();
  if (pareto) {
    benchmark.template Run<BitwiseBucketIndex<uint32_t, 4>>();
    benchmark.template Run<BitwiseBucketIndex<uint32_t, 8>>();
    benchmark.template Run<BitwiseBucketIndex<uint32_t, 32>>();
    benchmark.template Run<BitwiseBucketIndex<uint32_t, 64>>();
    benchmark.template Run<BitwiseBucketIndex<uint32_t, 256>>();
    benchmark.template Run<BitwiseBucketIndex<uint32_t, 1024>>();
    benchmark.template Run<BitwiseBucketIndex<uint32_t, 2048>>();
    if (benchmark.uses_binary_search()) {
      benchmark.template Run<BitwiseBucketIndex<uint32_t, 4096>>();
      benchmark.template Run<BitwiseBucketIndex<uint32_t, 8192>>();
    }
  }
}

template <template <typename> typename Searcher>
void benchmark_64_bitwise_index(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                      bool pareto) {
  benchmark.template Run<BitwiseBucketIndex<uint64_t, 16>>();
  if (pareto) {
    benchmark.template Run<BitwiseBucketIndex<uint64_t, 4>>();
    benchmark.template Run<BitwiseBucketIndex<uint64_t, 8>>();
    benchmark.template Run<BitwiseBucketIndex<uint64_t, 32>>();
    benchmark.template Run<BitwiseBucketIndex<uint64_t, 64>>();
    benchmark.template Run<BitwiseBucketIndex<uint64_t, 256>>();
    benchmark.template Run<BitwiseBucketIndex<uint64_t, 1024>>();
    benchmark.template Run<BitwiseBucketIndex<uint64_t, 2048>>();
    if (benchmark.uses_binary_search()) {
      benchmark.template Run<BitwiseBucketIndex<uint64_t, 4096>>();
      benchmark.template Run<BitwiseBucketIndex<uint64_t, 8192>>();
    }
  }
}

INSTANTIATE_TEMPLATES(benchmark_32_bitwise_index, uint32_t);
INSTANTIATE_TEMPLATES(benchmark_64_bitwise_index, uint64_t);

template <template <typename> typename Searcher>
void benchmark_32_greedy_index(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                      bool pareto) {
  benchmark.template Run<GreedyPartitionIndex<uint32_t, 16>>();
  if (pareto) {
    benchmark.template Run<GreedyPartitionIndex<uint32_t, 4>>();
    benchmark.template Run<GreedyPartitionIndex<uint32_t, 8>>();
    benchmark.template Run<GreedyPartitionIndex<uint32_t, 32>>();
    benchmark.template Run<GreedyPartitionIndex<uint32_t, 64>>();
    benchmark.template Run<GreedyPartitionIndex<uint32_t, 256>>();
    benchmark.template Run<GreedyPartitionIndex<uint32_t, 1024>>();
    benchmark.template Run<GreedyPartitionIndex<uint32_t, 2048>>();
    if (benchmark.uses_binary_search()) {
      benchmark.template Run<GreedyPartitionIndex<uint32_t, 4096>>();
      benchmark.template Run<GreedyPartitionIndex<uint32_t, 8192>>();
    }
  }
}

template <template <typename> typename Searcher>
void benchmark_64_greedy_index(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                      bool pareto) {
  benchmark.template Run<GreedyPartitionIndex<uint64_t, 16>>();
  if (pareto) {
    benchmark.template Run<GreedyPartitionIndex<uint64_t, 4>>();
    benchmark.template Run<GreedyPartitionIndex<uint64_t, 8>>();
    benchmark.template Run<GreedyPartitionIndex<uint64_t, 32>>();
    benchmark.template Run<GreedyPartitionIndex<uint64_t, 64>>();
    benchmark.template Run<GreedyPartitionIndex<uint64_t, 128>>();
    benchmark.template Run<GreedyPartitionIndex<uint64_t, 256>>();
    benchmark.template Run<GreedyPartitionIndex<uint64_t, 1024>>();
    benchmark.template Run<GreedyPartitionIndex<uint64_t, 2048>>();
    if (benchmark.uses_binary_search()) {
      benchmark.template Run<GreedyPartitionIndex<uint64_t, 4096>>();
      benchmark.template Run<GreedyPartitionIndex<uint64_t, 8192>>();
    }
  }
}

INSTANTIATE_TEMPLATES(benchmark_32_greedy_index, uint32_t);
INSTANTIATE_TEMPLATES(benchmark_64_greedy_index, uint64_t);