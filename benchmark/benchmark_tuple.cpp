
#include <tr/tuple.h>


#include <string>
#include <tuple>

#include <benchmark/benchmark.h>


static void TR_TupleSimpleTypeAssignment(benchmark::State& state) {
  tr::tuple<int, int> t{-1, 1};
  tr::tuple<int, int> t2{-2, 2};
  for (auto _ : state)
    benchmark::DoNotOptimize(t = t2);
}

BENCHMARK(TR_TupleSimpleTypeAssignment);

BENCHMARK_MAIN();
