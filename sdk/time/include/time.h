// Copyright 2025 The cppsdk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file time.h
 * @brief A time lib header file
 * @author wizyang
 */

#pragma once

#include <chrono>

namespace sdk {
namespace time {

template <class Rep, class Period>
inline void sleep_for(std::chrono::duration<Rep, Period> d) {
  sleep_for(std::chrono::duration_cast<std::chrono::nanoseconds>(d));
}

void sleep_for(std::chrono::nanoseconds duration) noexcept;

template <class Clock, class Duration>
inline void sleep_until(std::chrono::time_point<Clock, Duration> tp) noexcept {
  using namespace std::chrono;
  auto now = Clock::now();
  if (tp <= now) return;
  sleep_for(tp - now);
}

} // namespace time
} // namespace sdk
