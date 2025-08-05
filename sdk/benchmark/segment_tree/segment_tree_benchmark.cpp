#include "algorithm_v1/segment_tree.h"

#include <algorithm>
#include <benchmark/benchmark.h>
#include <glog/logging.h>
#include <random>
#include <vector>

static const int N = 100000;
static const int Q = 10000;
static std::vector<int> arr;
static std::vector<std::pair<int, int>> queries;

static void PrepareData() {
  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> dist(1, 100);
  arr.resize(N);
  for (int i = 0; i < N; ++i) arr[i] = dist(rng);

  std::uniform_int_distribution<int> distIndex(0, N - 1);
  queries.resize(Q);
  for (int i = 0; i < Q; ++i) {
    int l = distIndex(rng), r = distIndex(rng);
    if (l > r) std::swap(l, r);
    queries[i] = {l, r};
  }
}

// Benchmark 1: std::accumulate
static void BM_Accumulate(benchmark::State &state) {
  for (auto _ : state) {
    long long sum = 0;
    for (auto [l, r] : queries) {
      sum += std::accumulate(arr.begin() + l, arr.begin() + r + 1, 0LL);
    }
    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(BM_Accumulate);

// Benchmark 2: Segment Tree
static void BM_SegmentTree(benchmark::State &state) {
  algorithem_v1::SegmentTree st(arr);
  for (auto _ : state) {
    long long sum = 0;
    for (auto [l, r] : queries) {
      sum += st.query(l, r);
    }
    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(BM_SegmentTree);

int main(int argc, char **argv) {
  PrepareData();
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
  return 0;
}
