#include "benchmarks/benchmark_cht.h"

#include "benchmark.h"
#include "benchmarks/common.h"
#include "competitors/cht.h"

namespace {

constexpr bool simple_params =
#ifdef NDEBUG
false
#else
true
#endif
;

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

template <class T, template <typename> typename Searcher, class F, class... Args>
void benchmark_run_many(sosd::Benchmark<T, Searcher>& benchmark, F f, Args... args) {
  ::apply([=, &benchmark](auto... a) { benchmark.template Run<CHT<T, f(a...)>>(); }, args...);
}

template <class T, template <typename> typename Searcher>
void benchmark_run(sosd::Benchmark<T, Searcher>& benchmark) {
  auto f = [](auto x, auto y) { return 100 * y + x; };
  if constexpr (simple_params)
    benchmark_run_many(benchmark, f,
      std::integer_sequence<int, 10>{},
      std::integer_sequence<int, 6>{});
  else
    benchmark_run_many(benchmark, f,
      std::integer_sequence<int, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13>{},
      std::integer_sequence<int, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20>{});
}

};

template <template <typename> typename Searcher>
void benchmark_32_cht(sosd::Benchmark<uint32_t, Searcher>& benchmark,
                      bool pareto) {
  benchmark_run(benchmark);
}

template <template <typename> typename Searcher>
void benchmark_64_cht(sosd::Benchmark<uint64_t, Searcher>& benchmark,
                      bool pareto) {
  benchmark_run(benchmark);
}

INSTANTIATE_TEMPLATES(benchmark_32_cht, uint32_t);
INSTANTIATE_TEMPLATES(benchmark_64_cht, uint64_t);
