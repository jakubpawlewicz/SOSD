#include "benchmarks/benchmark_fastmap.h"

#include "benchmark.h"
#include "benchmarks/common.h"
#include "competitors/fastmap.h"

template <template <class, int> class Index, int size_scale, class T, template <typename> typename Searcher>
void benchmark_run(sosd::Benchmark<T, Searcher>& benchmark) {
  benchmark.template Run<Index<T, size_scale>>();
}

template <template <class, int> class Index, int... seq, class T, template <typename> typename Searcher>
void benchmark_run_seq(sosd::Benchmark<T, Searcher>& benchmark, std::integer_sequence<int, seq...>) {
  (benchmark_run<Index, seq>(benchmark), ...);
}

template <template <class, int> class Index, int val1, int... seq2, class F, class T, template <typename> typename Searcher>
void benchmark_run_two_seqs(sosd::Benchmark<T, Searcher>& benchmark, F f, std::integer_sequence<int, seq2...>) {
  (benchmark_run<Index, f(val1, seq2)>(benchmark), ...);
}

template <template <class, int> class Index, int... seq1, int... seq2, class F, class T, template <typename> typename Searcher>
void benchmark_run_two_seqs(sosd::Benchmark<T, Searcher>& benchmark, F f, std::integer_sequence<int, seq1...>, std::integer_sequence<int, seq2...> s2) {
    (benchmark_run_two_seqs<Index, seq1>(benchmark, f, s2), ...);
}

template <class T, template <typename> typename Searcher>
void benchmark_fastmap_bucket(sosd::Benchmark<T, Searcher>& benchmark, bool pareto) {
  benchmark_run_seq<FastMapBucket>(benchmark, std::integer_sequence<int, 1>{});
  if (pareto)
    benchmark_run_seq<FastMapBucket>(benchmark, std::integer_sequence<int, 2, 4, 8, 16, 32, 64, 128, 256, 1024, 4096>{});
}

template <template <typename> typename Searcher>
void benchmark_32_fastmap_bucket(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                     bool pareto) {
  benchmark_fastmap_bucket(benchmark, pareto);
}

template <template <typename> typename Searcher>
void benchmark_64_fastmap_bucket(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                     bool pareto) {
  benchmark_fastmap_bucket(benchmark, pareto);
}

template <class T, template <typename> typename Searcher>
void benchmark_fastmap_pgmfull(sosd::Benchmark<T, Searcher>& benchmark, bool pareto) {
  auto f = [](auto x, auto y) { return 100 * x + y; };
  if (pareto)
    benchmark_run_two_seqs<FastMapPGMFull>(benchmark, f,
      std::integer_sequence<int, 8, 16, 32, 64, 128, 256, 1024>{},
      std::integer_sequence<int, 6, 8, 10, 12, 16>{});
  else
    benchmark_run_two_seqs<FastMapPGMFull>(benchmark, f,
      std::integer_sequence<int, 34>{},
      std::integer_sequence<int, 10>{});
}

template <template <typename> typename Searcher>
void benchmark_32_fastmap_pgmfull(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                     bool pareto) {
  benchmark_fastmap_pgmfull(benchmark, pareto);
}

template <template <typename> typename Searcher>
void benchmark_64_fastmap_pgmfull(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                     bool pareto) {
  benchmark_fastmap_pgmfull(benchmark, pareto);
}

template <class T, template <typename> typename Searcher>
void benchmark_fastmap_pgmbucket(sosd::Benchmark<T, Searcher>& benchmark, bool pareto) {
  auto f = [](auto x, auto y) { return 100 * x + y; };
  if (pareto)
    benchmark_run_two_seqs<FastMapPGMBucket>(benchmark, f,
      std::integer_sequence<int, 8, 16, 32, 64, 128, 256, 1024>{},
      std::integer_sequence<int, 1, 2, 4>{});
  else
    benchmark_run_two_seqs<FastMapPGMBucket>(benchmark, f,
      std::integer_sequence<int, 34>{},
      std::integer_sequence<int, 1>{});
}

template <template <typename> typename Searcher>
void benchmark_32_fastmap_pgmbucket(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                     bool pareto) {
  benchmark_fastmap_pgmbucket(benchmark, pareto);
}

template <template <typename> typename Searcher>
void benchmark_64_fastmap_pgmbucket(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                     bool pareto) {
  benchmark_fastmap_pgmbucket(benchmark, pareto);
}

template <class T, template <typename> typename Searcher>
void benchmark_fastmap_apx(sosd::Benchmark<T, Searcher>& benchmark, bool pareto) {
  auto f = [](auto x, auto y) { return 1000 * x + y; };
  if (pareto)
    benchmark_run_two_seqs<FastMapApx>(benchmark, f,
      std::integer_sequence<int, 8, 16, 32, 64, 128, 256>{},
      std::integer_sequence<int, 15, 20, 40, 60, 80, 120>{});
  else
    benchmark_run_two_seqs<FastMapApx>(benchmark, f,
      std::integer_sequence<int, 16>{},
      std::integer_sequence<int, 100>{});
}

template <template <typename> typename Searcher>
void benchmark_32_fastmap_apx(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                     bool pareto) {
  benchmark_fastmap_apx(benchmark, pareto);
}

template <template <typename> typename Searcher>
void benchmark_64_fastmap_apx(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                     bool pareto) {
  benchmark_fastmap_apx(benchmark, pareto);
}

INSTANTIATE_TEMPLATES(benchmark_32_fastmap_bucket, uint32_t);
INSTANTIATE_TEMPLATES(benchmark_64_fastmap_bucket, uint64_t);

INSTANTIATE_TEMPLATES(benchmark_32_fastmap_pgmfull, uint32_t);
INSTANTIATE_TEMPLATES(benchmark_64_fastmap_pgmfull, uint64_t);

INSTANTIATE_TEMPLATES(benchmark_32_fastmap_pgmbucket, uint32_t);
INSTANTIATE_TEMPLATES(benchmark_64_fastmap_pgmbucket, uint64_t);

INSTANTIATE_TEMPLATES(benchmark_32_fastmap_apx, uint32_t);
INSTANTIATE_TEMPLATES(benchmark_64_fastmap_apx, uint64_t);
