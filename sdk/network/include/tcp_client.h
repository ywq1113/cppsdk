// Copyright 2025 The cppsdk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file tcp_client.h
 * @brief A TCP client interface for network communication.
 * @author wizyang
 */

#pragma once

#include "network/frame_codec_base.h"

#include <functional>
#include <memory>
#include <span>
#include <string>

namespace sdk {
namespace net {

class TcpClient {
 public:
  using CodecFactory = std::function<std::unique_ptr<IFrameCodec>()>;
  using ErrorHandler =
      std::function<void(int conn_id, int code, const std::string &what)>;
  using MessageHandler =
      std::function<void(const std::vector<uint8_t> &payload)>;
  using ConnectHandler = std::function<void(int conn_id)>;
  using DisconnectHandler =
      std::function<void(int conn_id, int code, const std::string &message)>;

  TcpClient() = default;
  virtual ~TcpClient() = default;

  static std::unique_ptr<TcpClient> Create(const std::string &host,
                                           uint16_t port);
  virtual void start() = 0;
  virtual void stop() = 0;

  virtual void send(std::span<const uint8_t> data) = 0;

  void send(const std::string &s) {
    send(std::span<const uint8_t>(reinterpret_cast<const uint8_t *>(s.data()),
                                  s.size()));
  }

  void send(std::string_view sv) {
    send(std::span<const uint8_t>(reinterpret_cast<const uint8_t *>(sv.data()),
                                  sv.size()));
  }

  template <size_t N>
  void send(const char (&s)[N]) {
    send(std::span<const uint8_t>(reinterpret_cast<const uint8_t *>(s), N - 1));
  }

  virtual void setCodecFactory(CodecFactory factory) = 0;

  virtual void onError(ErrorHandler cb) = 0;
  virtual void onMessage(MessageHandler cb) = 0;
  virtual void onConnect(ConnectHandler cb) = 0;
  virtual void onDisconnect(DisconnectHandler cb) = 0;
};

} // namespace net
} // namespace sdk
