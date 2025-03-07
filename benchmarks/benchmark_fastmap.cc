#include "benchmarks/benchmark_fastmap.h"

#include "benchmark.h"
#include "benchmarks/common.h"
#include "competitors/fastmap.h"

namespace {

template <class... Args>
struct Applier
{
  template <class F>
  static void apply(F f, Args... args)
  {
    f(args...);
  }
};

template <class T, T... I, class... Args>
struct Applier<std::integer_sequence<T, I...>, Args...>
{
  template <class F>
  static void apply(F f, std::integer_sequence<T, I...>, Args... args)
  {
    (Applier<Args..., std::integral_constant<T, I>>::apply(f, args..., std::integral_constant<T, I>()), ...);
  }
};

template <class F, class... Args>
void apply(F f, Args... args)
{
  Applier<Args...>::apply(f, args...);
}

};

template <template <class, int> class Index, int size_scale, class T, template <typename> typename Searcher>
void benchmark_run(sosd::Benchmark<T, Searcher>& benchmark) {
  benchmark.template Run<Index<T, size_scale>>();
}

namespace {
template <template <class, int> class Index, class T, template <typename> typename Searcher,
          class F, class... Args>
void benchmark_run_help(sosd::Benchmark<T, Searcher>& benchmark, F f, Args... args) {
  ::apply([=, &benchmark](auto... a) { benchmark.template Run<Index<T, f(a...)>>(); }, args...);
}

constexpr auto id = [](auto x) { return x; };

};

template <class T, template <typename> typename Searcher>
void benchmark_fastmap_bucket(sosd::Benchmark<T, Searcher>& benchmark, bool pareto) {
  auto f = [](auto x, auto y, auto z, auto u) { return 10*(100*(100 * x + y) + z) + u; };
  if (pareto)
    benchmark_run_help<FastMapBucket>(benchmark, f,
      std::integer_sequence<int, 1, 2, 4, 8, 16>{},
      std::integer_sequence<int, 0, 5, 10, 20, 35, 50, 70, 90>{},
      std::integer_sequence<int, 16, 32>{},
      std::integer_sequence<int, 2, 4>{});
  else
    benchmark_run_help<FastMapBucket>(benchmark, f,
      std::integer_sequence<int, 1>{},
      std::integer_sequence<int, 0>{},
      std::integer_sequence<int, 16>{},
      std::integer_sequence<int, 2>{});
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
  constexpr auto f = [](auto x, auto y) constexpr { return 100 * x + y; };
  if (pareto)
    benchmark_run_help<FastMapPGMFull>(benchmark, f,
      std::integer_sequence<int, 8, 16, 32, 64, 128, 256, 1024, 4096>{},
      std::integer_sequence<int, 6, 8, 10, 12, 16>{});
  else
    benchmark_run_help<FastMapPGMFull>(benchmark, f,
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
  auto f = [](auto x, auto y, auto z, auto u, auto v)
  { return 10 * (100 * (100 * (10 * x + y) + z) + u) + v; };
  if (pareto)
    benchmark_run_help<FastMapPGMBucket>(benchmark, f,
      std::integer_sequence<int, 4, 16, 64, 256>{},
      std::integer_sequence<int, 1, 4>{},
      std::integer_sequence<int, 0, 10, 20, 30, 50, 90>{},
      std::integer_sequence<int, 16, 32>{},
      std::integer_sequence<int, 2, 4>{});
  else
    benchmark_run_help<FastMapPGMBucket>(benchmark, f,
      std::integer_sequence<int, 32>{},
      std::integer_sequence<int, 1>{},
      std::integer_sequence<int, 0>{},
      std::integer_sequence<int, 16>{},
      std::integer_sequence<int, 2>{});
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
  constexpr auto f = [](auto x, auto y) { return 1000 * x + y; };
  if (pareto)
    benchmark_run_help<FastMapApx>(benchmark, f,
      std::integer_sequence<int, 8, 16, 32, 64, 128, 256>{},
      std::integer_sequence<int, 15, 20, 40, 60, 80, 120>{});
  else
    benchmark_run_help<FastMapApx>(benchmark, f,
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
