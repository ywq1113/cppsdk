// Copyright 2025 The cppsdk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file tcp_client_impl.h
 * @brief A TCP client implementation for network communication.
 * @author wizyang
 */

#pragma once

#include "macro/macros.h"
#include "network/frame_codec_base.h"
#include "network/tcp_client.h"

#include <cstdint>

#include <array>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <deque>
#include <span>
#include <string>
#include <thread>
#include <vector>

namespace sdk {
namespace net {

class TcpClientImpl : public TcpClient {
 public:
  explicit TcpClientImpl(const std::string &host, uint16_t port);
  ~TcpClientImpl() override;

  void start() override;
  void stop() override;

  /*
   * @brief Thread-safe send
   * @param std::span<const uint8_t> data
   * @return void
   *
   */
  void send(std::span<const uint8_t> data) override {
    std::vector<uint8_t> buf(data.begin(), data.end());
    send(std::move(buf));
  }

  void send(std::vector<uint8_t> &&data) {
    if (!running_) return;
    if (!session_) return;
    session_->enqueueWrite(std::move(data));
  }

  // Callback
  void onError(ErrorHandler cb) override;
  void onMessage(MessageHandler cb) override;
  void onConnect(ConnectHandler cb) override;
  void onDisconnect(DisconnectHandler cb) override;

  // Codec
  void setCodecFactory(CodecFactory factory) override {
    codec_factory_ = std::move(factory);
  }

 private:
  struct Session : public std::enable_shared_from_this<Session> {
    Session(int id, boost::asio::ip::tcp::socket socket, TcpClientImpl *client);

    ~Session() {
      boost::system::error_code ec;
      socket.close(ec); // NOLINT
    }
    void start();
    void doRead();
    void doWrite();
    void enqueueWrite(std::vector<uint8_t> data);

    int id;
    TcpClientImpl *client;
    boost::asio::strand<boost::asio::io_context::executor_type> strand;
    boost::asio::ip::tcp::socket socket;

    std::array<uint8_t, 4096> readBuf;

    using Buffer = std::vector<uint8_t>;
    std::deque<Buffer> writeQueue;

    std::unique_ptr<IFrameCodec> codec;

    DISALLOW_COPY_AND_MOVE(Session);
  };

  void doConnect();

  void handleConnectError(const boost::system::error_code &ec);

  void startHeartbeat();
  void stopHeartbeat();

 private:
  std::string host_;
  uint16_t port_;
  std::atomic<bool> running_ = {false};

  boost::asio::io_context io_;
  boost::asio::ip::tcp::socket socket_;
  boost::asio::ip::tcp::resolver resolver_;

  boost::asio::steady_timer reconnectTimer_;
  boost::asio::steady_timer heartbeatTimer_;

  uint32_t reconnectDelayMs_;
  uint32_t reconnectMaxDelayMs_;
  uint32_t heartbeatIntervalMs_;

  std::jthread worker_;

  // Callback
  ErrorHandler onError_;
  MessageHandler onMessage_;
  ConnectHandler onConnect_;
  DisconnectHandler onDisconnect_;
  // Codec
  CodecFactory codec_factory_;
  // Session
  std::shared_ptr<Session> session_;
  // Flow control
  size_t maxWriteQueueSize_ = 1024;

  DISALLOW_COPY(TcpClientImpl);
};

} // namespace net
} // namespace sdk
