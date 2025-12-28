#include "container/spsc_ringbuffer.hpp"

#include <benchmark/benchmark.h>
#include <boost/lockfree/spsc_queue.hpp>
#include <vector>

using sdk::container::SPSCRingbuffer;

struct alignas(64) Msg64 {
  uint64_t t{0};
  uint8_t payload[56]{};
};

inline bool try_yield() {
#ifdef __x86_64__
  _mm_pause();
#else
  std::this_thread::yield();
#endif
  return true;
}

struct MyQ {
  using Q = SPSCRingbuffer<Msg64>;
  using value_type = Msg64;
  explicit MyQ(size_t cap) : q_(cap) {}
  bool try_write(const Msg64 &m, int attempts) {
    return q_.push(m, attempts);
  }
  bool try_read(Msg64 &m) {
    return q_.pop(m);
  }
  Q q_;
};

// 2) Boost lockfree::spsc_queue
struct BoostQ {
  using Q = boost::lockfree::spsc_queue<Msg64>;
  explicit BoostQ(size_t cap) : q_(cap) {}
  bool try_write(const Msg64 &m, int attempts) {
    for (int i = 0; i < attempts; ++i) {
      if (q_.push(m)) return true;
      try_yield();
    }
    return false;
  }
  bool try_read(Msg64 &m) {
    return q_.pop(m);
  }
  Q q_;
};

// 绑定 CPU
static void pin_to_cpu_optional(int cpu) {
#ifdef __linux__
  cpu_set_t set;
  CPU_ZERO(&set);
  CPU_SET(cpu, &set);
  pthread_setaffinity_np(pthread_self(), sizeof(set), &set);
#endif
}

template <class Adapter>
static void BM_SPSC_Throughput(benchmark::State &st) {
  const size_t cap = static_cast<size_t>(st.range(0));
  const int attempts = static_cast<int>(st.range(1));
  const bool with_ts = st.range(2) != 0;

  Adapter A(cap);

  std::atomic<bool> stop{false};
  std::atomic<uint64_t> pushed{0}, popped{0}, drops{0};

  const int B = 64;
  std::vector<std::atomic<uint64_t>> hist(B);
  for (auto &h : hist) h.store(0, std::memory_order_relaxed);

  auto now_ns = [] {
    using namespace std::chrono;
    return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch())
        .count();
  };
  auto bucket = [](uint64_t ns) -> int {
    int b = 0;
    while ((ns >>= 1) && b < 63) ++b;
    return b;
  };

  std::thread prod([&] {
    pin_to_cpu_optional(2);
    for (auto _ : st) {
      const auto t_end =
          std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
      while (std::chrono::steady_clock::now() < t_end &&
             !stop.load(std::memory_order_relaxed)) {
        Msg64 msg{};
        if (with_ts) msg.t = now_ns();
        if (A.try_write(msg, attempts)) {
          pushed.fetch_add(1, std::memory_order_relaxed);
        } else {
          drops.fetch_add(1, std::memory_order_relaxed);
        }
      }
      benchmark::ClobberMemory();
    }
    stop.store(true, std::memory_order_release);
  });

  std::thread cons([&] {
    pin_to_cpu_optional(10);
    Msg64 m{};
    while (!stop.load(std::memory_order_acquire)) {
      if (A.try_read(m)) {
        if (with_ts && m.t) {
          uint64_t d = now_ns() - m.t;
          hist[bucket(d)].fetch_add(1, std::memory_order_relaxed);
        }
        popped.fetch_add(1, std::memory_order_relaxed);
      } else {
        try_yield();
      }
    }

    while (A.try_read(m)) {
      if (with_ts && m.t) {
        uint64_t d = now_ns() - m.t;
        hist[bucket(d)].fetch_add(1, std::memory_order_relaxed);
      }
      popped.fetch_add(1, std::memory_order_relaxed);
    }
  });

  prod.join();
  cons.join();

  const double ops = static_cast<double>(popped.load());
  st.counters["ops/s"] = benchmark::Counter(ops, benchmark::Counter::kIsRate);

  const double drops_d = static_cast<double>(drops.load());
  const double attempts_d = static_cast<double>(pushed.load() + drops.load());
  st.counters["drop_rate"] = attempts_d > 0 ? drops_d / attempts_d : 0.0;

  if (with_ts) {
    uint64_t total = 0;
    for (auto &h : hist) total += h.load();
    auto quant = [&](double r) {
      if (total == 0) return 0.0;
      uint64_t tgt = static_cast<uint64_t>(total * r + 0.5), acc = 0;
      for (int i = 0; i < B; i++) {
        acc += hist[i].load();
        if (acc >= tgt) return static_cast<double>(1ull << i);
      }
      return 0.0;
    };
    st.counters["p50_ns"] = quant(0.50);
    st.counters["p95_ns"] = quant(0.95);
    st.counters["p99_ns"] = quant(0.99);
  }
}

BENCHMARK_TEMPLATE(BM_SPSC_Throughput, MyQ)
    ->Args({1024, 256, 0})
    ->Args({8192, 256, 0})
    ->Args({65536, 256, 0})
    ->Args({8192, 256, 1});

BENCHMARK_TEMPLATE(BM_SPSC_Throughput, BoostQ)
    ->Args({1024, 256, 0})
    ->Args({8192, 256, 0})
    ->Args({65536, 256, 0})
    ->Args({8192, 256, 1});

BENCHMARK_MAIN();
