/*
  Copyright 2025 The cppsdk Authors. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be
  found in the LICENSE file.
  Author: wizyang
  Date: 2025/11/23
  Description: A ringbuffer test cases.

  @file container/tests/ringbuffer_test.cpp
*/

#include "container/spsc_ringbuffer.hpp"

#include <chrono>

#include <atomic>
#include <gtest/gtest.h>
#include <thread>
#include <vector>

using sdk::container::SPSCRingbuffer;

struct MoveOnly {
  DISALLOW_COPY(MoveOnly);
  int v{0};
  MoveOnly() = default;
  explicit MoveOnly(int x) : v(x) {}
  MoveOnly(MoveOnly &&) noexcept = default;
  MoveOnly &operator=(MoveOnly &&) noexcept = default;
  bool operator==(const MoveOnly &o) const {
    return v == o.v;
  }
};

struct CountOps {
  int v{0};
  static inline std::atomic<int> copies{0};
  static inline std::atomic<int> moves{0};

  CountOps() = default;
  explicit CountOps(int x) : v(x) {}

  CountOps(const CountOps &rhs) : v(rhs.v) {
    copies.fetch_add(1, std::memory_order_relaxed);
  }
  CountOps &operator=(const CountOps &rhs) {
    if (this != &rhs) {
      v = rhs.v;
      copies.fetch_add(1, std::memory_order_relaxed);
    }
    return *this;
  }
  CountOps(CountOps &&rhs) noexcept : v(rhs.v) {
    moves.fetch_add(1, std::memory_order_relaxed);
  }
  CountOps &operator=(CountOps &&rhs) noexcept {
    if (this != &rhs) {
      v = rhs.v;
      moves.fetch_add(1, std::memory_order_relaxed);
    }
    return *this;
  }

  static void Reset() {
    copies = 0;
    moves = 0;
  }
};

TEST(SPSCRingbufferTimeout, BasicPushPopSequence) {
  SPSCRingbuffer<int> rb(8);
  EXPECT_TRUE(rb.empty());
  EXPECT_EQ(rb.size(), 0u);

  const int kOnce = 1;
  const auto cap = rb.capacity();

  for (size_t i = 0; i < cap; ++i) {
    EXPECT_TRUE(rb.push((int)i, kOnce));
  }
  EXPECT_FALSE(rb.push(999, kOnce));
  EXPECT_EQ(rb.size(), cap);

  // Pop all
  for (auto i = 0; i < cap; ++i) {
    int x = -1;
    EXPECT_TRUE(rb.pop(x));
    EXPECT_EQ(x, i);
  }
  EXPECT_TRUE(rb.empty());
  int dummy = 0;
  EXPECT_FALSE(rb.pop(dummy));
}

TEST(SPSCRingbufferTimeout, WrapAround) {
  SPSCRingbuffer<int> rb(5);
  const int kTry = 2;
  const auto cap = rb.capacity();

  for (auto i = 0; i < cap / 2; ++i) {
    EXPECT_TRUE(rb.push(i, kTry));
  }
  for (auto i = 0; i < cap / 2; ++i) {
    int x = -1;
    EXPECT_TRUE(rb.pop(x));
    EXPECT_EQ(x, i);
  }

  for (auto i = 0; i < cap; ++i) {
    EXPECT_TRUE(rb.push(100 + i, kTry)) << "i=" << i;
  }
  EXPECT_FALSE(rb.push(777, kTry));

  for (auto i = 0; i < cap; ++i) {
    int x = -1;
    EXPECT_TRUE(rb.pop(x));
    EXPECT_EQ(x, 100 + i);
  }
  EXPECT_TRUE(rb.empty());
}

TEST(SPSCRingbufferTimeout, TimeoutThenSucceedAfterPop) {
  SPSCRingbuffer<int> rb(4);
  const auto cap = rb.capacity();

  for (auto i = 0; i < cap; ++i) {
    EXPECT_TRUE(rb.push(i, /*max_attempt=*/1));
  }
  EXPECT_FALSE(rb.push(123, /*max_attempt=*/1));

  int out = -1;
  EXPECT_TRUE(rb.pop(out));
  EXPECT_TRUE(rb.push(123, /*max_attempt=*/1));
}

TEST(SPSCRingbufferTimeout, MoveOnlyType) {
  SPSCRingbuffer<MoveOnly> rb(4);
  EXPECT_TRUE(rb.push(MoveOnly{1}, 1));
  EXPECT_TRUE(rb.push(MoveOnly{2}, 1));

  MoveOnly a, b;
  EXPECT_TRUE(rb.pop(a));
  EXPECT_TRUE(rb.pop(b));
  EXPECT_EQ(a.v, 1);
  EXPECT_EQ(b.v, 2);
  EXPECT_TRUE(rb.empty());
}

TEST(SPSCRingbufferTimeout, CopyMoveCounting) {
  CountOps::Reset();
  SPSCRingbuffer<CountOps> rb(4);

  CountOps x{1};
  EXPECT_TRUE(rb.push(x, /*max_attempt=*/2));
  EXPECT_TRUE(rb.push(CountOps{2}, /*max_attempt=*/2));
  EXPECT_TRUE(rb.push(CountOps{3}, /*max_attempt=*/2));

  CountOps a, b, c;
  EXPECT_TRUE(rb.pop(a));
  EXPECT_TRUE(rb.pop(b));
  EXPECT_TRUE(rb.pop(c));
  EXPECT_EQ(a.v, 1);
  EXPECT_EQ(b.v, 2);
  EXPECT_EQ(c.v, 3);

  EXPECT_EQ(CountOps::copies.load(), 1);
  EXPECT_EQ(CountOps::moves.load(), 5);
}

TEST(SPSCRingbufferTimeout, SPSC_Stress_WithTimeout) {
  const size_t N = 100000;
  SPSCRingbuffer<int> rb(1024);

  std::atomic<size_t> produced{0}, consumed{0};
  std::vector<int> out;
  out.reserve(N);

  std::thread prod_th([&] {
    for (auto i = 0; i < N; ++i) {
      int attempts = 256;
      while (!rb.push(i, attempts)) {
        std::this_thread::yield();
      }
      produced.fetch_add(1, std::memory_order_relaxed);
    }
  });

  std::thread cons_th([&] {
    int x;
    while (consumed.load(std::memory_order_relaxed) < N) {
      if (rb.pop(x)) {
        out.push_back(x);
        consumed.fetch_add(1, std::memory_order_relaxed);
        // Mock slow consumer
        if ((consumed.load(std::memory_order_relaxed) & 0xFFF) == 0) {
          std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
      } else {
        std::this_thread::yield();
      }
    }
  });

  prod_th.join();
  cons_th.join();

  ASSERT_EQ(out.size(), N);
  for (size_t i = 0; i < N; ++i) {
    EXPECT_EQ(out[i], (int)i);
  }
  EXPECT_TRUE(rb.empty());
}
