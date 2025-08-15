#pragma once

#include "base/macros.h"

#include <cstddef>

#include <atomic>
#include <new>
#include <thread>
#include <type_traits>
#include <utility>

namespace container_v1 {

// Thread-safe SPSC ring buffer (single-producer single-consumer)
// Notes: Do not supports placement new for data_ now
// TODO: support placement new for data_
//       (requires T to be default-constructible)
template <typename T, bool placement_new = false>
class SPSCRingbuffer {
 public:
  explicit SPSCRingbuffer(size_t size) noexcept
      : size_(size), data_(nullptr), head_(0), tail_(0) {
    // size at least 2 to avoid head == tail ambiguity
    size_ = (size_ < 2) ? 2 : size_;
    static_assert(std::is_default_constructible_v<T>,
                  "T must be default-constructible when using new T[cap]");
    data_ = new T[size_];
  }

  ~SPSCRingbuffer() {
    delete[] data_;
  }

  DISALLOW_COPY(SPSCRingbuffer);

  bool try_push(const T &value) noexcept(
      std::is_nothrow_assignable_v<T &, const T &>) {
    const size_t head = head_.load(std::memory_order_relaxed);
    const size_t next = inc(head);
    if (next == tail_.load(std::memory_order_acquire)) {
      return false; // full
    }
    data_[head] = value;
    head_.store(next, std::memory_order_release);
    return true;
  }

  bool try_push(T &&v) noexcept(std::is_nothrow_assignable_v<T &, T &&>) {
    const size_t head = head_.load(std::memory_order_relaxed);
    const size_t next = inc(head);
    // Full
    if (next == tail_.load(std::memory_order_acquire)) {
      return false;
    }
    data_[head] = std::move(v);
    head_.store(next, std::memory_order_release);
    return true;
  }

  bool push(const T &value, int max_attempt) noexcept(
      std::is_nothrow_assignable_v<T &, const T &>) {
    for (auto i = 0; i < max_attempt; ++i) {
      if (try_push(value)) {
        return true;
      }
      std::this_thread::yield();
    }
    return false;
  }

  bool push(T &&value,
            int max_attempt) noexcept(std::is_nothrow_assignable_v<T &, T &&>) {
    for (auto i = 0; i < max_attempt; ++i) {
      if (try_push(std::move(value))) {
        return true;
      }
      std::this_thread::yield();
    }
    return false;
  }

  bool pop(T &value) noexcept(std::is_nothrow_assignable_v<T &, T &&> ||
                              std::is_nothrow_copy_assignable_v<T>) {
    const size_t tail = tail_.load(std::memory_order_relaxed);
    // Empty
    if (tail == head_.load(std::memory_order_acquire)) {
      return false;
    }
    if constexpr (std::is_move_assignable_v<T>) {
      value = std::move(data_[tail]);
    } else {
      value = data_[tail];
    }
    tail_.store(inc(tail), std::memory_order_release);
    return true;
  }

  size_t size() const noexcept {
    const size_t h = head_.load(std::memory_order_acquire);
    const size_t t = tail_.load(std::memory_order_acquire);
    return (h + size_ - t) % size_;
  }

  bool empty() const noexcept {
    return head_.load(std::memory_order_acquire) ==
           tail_.load(std::memory_order_acquire);
  }

  bool full() const noexcept {
    return inc(head_.load(std::memory_order_acquire)) ==
           tail_.load(std::memory_order_acquire);
  }

  size_t capacity() const noexcept {
    return size_ - 1;
  }

 private:
  // Increment index in a circular manner and wrap around
  // inc(0) -> 1, inc(size_-1) -> 0
  size_t inc(size_t x) const noexcept {
    return (x + 1) % size_;
  }

  size_t size_;
  T *data_;
  alignas(64) std::atomic<size_t> head_;
  alignas(64) std::atomic<size_t> tail_;
};

} // namespace container_v1
