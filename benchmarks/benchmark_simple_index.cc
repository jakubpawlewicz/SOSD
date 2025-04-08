#include "benchmarks/benchmark_simple_index.h"

#include "benchmark.h"
#include "benchmarks/common.h"
#include "competitors/greedy_partition_bs.h"

namespace {

template <template <class, int> class Index, int size_scale, class T, template <typename> typename Searcher>
void benchmark_run_single(sosd::Benchmark<T, Searcher>& benchmark) {
  benchmark.template Run<Index<T, 1 << size_scale>>();
}

template <template <class, int> class Index, class T, template <typename> typename Searcher,
          int... I>
void benchmark_run_many(sosd::Benchmark<T, Searcher>& benchmark, std::integer_sequence<int, I...>) {
  (benchmark_run_single<Index, I, T, Searcher>(benchmark), ...);
}

template <template <class, int> class Index, class T, template <typename> typename Searcher>
void benchmark_run(sosd::Benchmark<T, Searcher>& benchmark, bool pareto) {
  benchmark_run_single<Index, 4>(benchmark);
  if (pareto) {
    benchmark_run_many<Index>(benchmark,
      std::integer_sequence<int, 2, 3, 5, 6, 7, 8, 9, 10, 11>{});
    if (benchmark.uses_binary_search())
      benchmark_run_many<Index>(benchmark,
        std::integer_sequence<int, 12, 13, 14, 15, 16, 17, 18, 19, 20>{});
  }
}

}

template <template <typename> typename Searcher>
void benchmark_32_simple_index(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                      bool pareto) {
  benchmark_run<SimpleBucketIndex>(benchmark, pareto);
}

template <template <typename> typename Searcher>
void benchmark_64_simple_index(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                      bool pareto) {
  benchmark_run<SimpleBucketIndex>(benchmark, pareto);
}

INSTANTIATE_TEMPLATES(benchmark_32_simple_index, uint32_t);
INSTANTIATE_TEMPLATES(benchmark_64_simple_index, uint64_t);

template <template <typename> typename Searcher>
void benchmark_32_bitwise_index(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                      bool pareto) {
  benchmark_run<BitwiseBucketIndex>(benchmark, pareto);
}

template <template <typename> typename Searcher>
void benchmark_64_bitwise_index(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                      bool pareto) {
  benchmark_run<BitwiseBucketIndex>(benchmark, pareto);
}

INSTANTIATE_TEMPLATES(benchmark_32_bitwise_index, uint32_t);
INSTANTIATE_TEMPLATES(benchmark_64_bitwise_index, uint64_t);

template <template <typename> typename Searcher>
void benchmark_32_greedy_index(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                      bool pareto) {
  benchmark_run<GreedyPartitionIndex>(benchmark, pareto);
}

template <template <typename> typename Searcher>
void benchmark_64_greedy_index(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                      bool pareto) {
  benchmark_run<GreedyPartitionIndex>(benchmark, pareto);
}

INSTANTIATE_TEMPLATES(benchmark_32_greedy_index, uint32_t);
INSTANTIATE_TEMPLATES(benchmark_64_greedy_index, uint64_t);