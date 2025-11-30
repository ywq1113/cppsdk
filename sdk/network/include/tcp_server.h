// Copyright 2025 The cppsdk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file tcp_server.h
 * @brief A TCP server interface for network communication.
 * @author wizyang
 */

#pragma once

#include "network/frame_codec_base.h"

#include <cstdint>

#include <functional>
#include <memory>
#include <span>
#include <string>

namespace sdk {
namespace net {

class TcpServer {
 public:
  using CodecFactory = std::function<std::unique_ptr<IFrameCodec>()>;
  using ErrorHandler =
      std::function<void(int conn_id, int code, const std::string &what)>;
  using MessageHandler =
      std::function<void(int conn_id, const std::vector<uint8_t> &payload)>;
  using DisconnectHandler =
      std::function<void(int conn_id, int code, const std::string &message)>;
  using ConnectHandler = std::function<void(int conn_id)>;

  TcpServer() = default;
  virtual ~TcpServer() = default;

  static std::unique_ptr<TcpServer> Create(uint16_t port);

  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void send(int conn_id, std::span<const uint8_t> data) = 0;

  virtual void setCodecFactory(CodecFactory factory) = 0;

  virtual void onDisconnect(DisconnectHandler cb) = 0;
  virtual void onError(ErrorHandler cb) = 0;
  virtual void onMessage(MessageHandler cb) = 0;
  virtual void onConnect(ConnectHandler cb) = 0;
};

} // namespace net
} // namespace sdk
