// Copyright 2025 The cppsdk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file frame_codec_base.h
 * @brief A frame codec interface for network communication.
 * @author wizyang
 */

#pragma once

#include <cstdint>

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace sdk {
namespace net {

class IFrameCodec {
 public:
  virtual ~IFrameCodec() = default;
  virtual void append(const uint8_t *data, size_t n) = 0;
  virtual std::optional<std::vector<uint8_t>> tryDecode() = 0;
};

} // namespace net
} // namespace sdk
