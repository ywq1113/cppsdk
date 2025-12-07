// Copyright 2025 The cppsdk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file time.cc
 * @brief A time lib src file
 * @author wizyang
 */

#include "time/time.h"

#include <chrono>

#include <errno.h>
#include <time.h>

namespace sdk {
namespace time {

inline void sleep_for(std::chrono::nanoseconds duration) noexcept {
  if (duration.count() <= 0) return;

  using namespace std::chrono;

  auto ns = duration.count();
  struct timespec t;
  struct timespec r;

  t.tv_sec = ns / 1'000'000'000;
  t.tv_nsec = ns % 1'000'000'000;

  while (nanosleep(&t, &r) == -1 && errno == EINTR) {
    t = r;
  }
}

} // namespace time
} // namespace sdk
