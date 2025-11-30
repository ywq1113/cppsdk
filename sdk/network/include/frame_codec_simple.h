// Copyright 2025 The cppsdk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file frame_codec_simple.h
 * @brief A frame codec demo for network communication.
 * @author wizyang
 */

#pragma once

#include "network/frame_codec_base.h"

#include <cstdint>
#include <cstring>

#include <optional>
#include <vector>

namespace sdk {
namespace net {

constexpr bool isLittleEndian() {
  return std::endian::native == std::endian::little;
}

constexpr bool isBigEndian() {
  return std::endian::native == std::endian::big;
}

class SimpleMagicCodec : public IFrameCodec {
 public:
  static constexpr uint16_t kMagic = 0xABCD; // 2 bytes

  void append(const uint8_t *data, size_t len) override {
    buf_.insert(buf_.end(), data, data + len);
  }

  std::optional<std::vector<uint8_t>> tryDecode() override {
    while (true) {
      if (buf_.size() < 6) return std::nullopt; // header incomplete

      // ---- parse magic ----
      uint16_t magic = (uint16_t(buf_[0]) << 8) | (uint16_t(buf_[1]) << 0);
      if (magic != kMagic) {
        buf_.erase(buf_.begin());
        continue;
      }

      // ---- parse length ----
      uint32_t len = (uint32_t(buf_[2]) << 24) | (uint32_t(buf_[3]) << 16) |
                     (uint32_t(buf_[4]) << 8) | (uint32_t(buf_[5]));
      if (buf_.size() < 6 + len) return std::nullopt;

      // ---- extract payload ----
      std::vector<uint8_t> out(buf_.begin() + 6, buf_.begin() + 6 + len);

      // ---- remove used bytes ----
      buf_.erase(buf_.begin(), buf_.begin() + 6 + len);
      return out;
    }
  }

  std::vector<uint8_t> encode(const uint8_t *data, size_t len) {
    std::vector<uint8_t> out;
    out.reserve(6 + len);

    // magic (big endian)
    out.push_back(uint8_t((kMagic >> 8) & 0xFF));
    out.push_back(uint8_t((kMagic >> 0) & 0xFF));

    // length (big endian)
    out.push_back(uint8_t((len >> 24) & 0xFF));
    out.push_back(uint8_t((len >> 16) & 0xFF));
    out.push_back(uint8_t((len >> 8) & 0xFF));
    out.push_back(uint8_t((len >> 0) & 0xFF));

    // payload
    out.insert(out.end(), data, data + len);

    return out;
  }

 private:
  std::vector<uint8_t> buf_;
};

} // namespace net
} // namespace sdk
