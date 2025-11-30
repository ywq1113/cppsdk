// Copyright 2025 The cppsdk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file tcp_server_impl.h
 * @brief A TCP server implementation for network communication.
 * @author wizyang
 */

#pragma once

#include "macro/macros.h"
#include "network/frame_codec_base.h"
#include "network/tcp_server.h"

#include <atomic>
#include <boost/asio.hpp>
#include <deque>
#include <memory>
#include <thread>
#include <unordered_map>

namespace sdk {
namespace net {

class TcpServerImpl : public TcpServer {
 public:
  explicit TcpServerImpl(uint16_t port);
  ~TcpServerImpl() override;

  void start() override;
  void stop() override;

  void send(int conn_id, std::span<const uint8_t> data) override;

  void onError(ErrorHandler cb) override {
    onError_ = std::move(cb);
  }
  void onMessage(MessageHandler cb) override {
    onMessage_ = std::move(cb);
  }
  void onDisconnect(DisconnectHandler cb) override {
    onDisconnect_ = std::move(cb);
  }

  void onConnect(ConnectHandler cb) override {
    onConnect_ = std::move(cb);
  }

  void setCodecFactory(CodecFactory factory) override {
    codec_factory_ = std::move(factory);
  }

  boost::asio::io_context &get_io_context() {
    return io_;
  }

 private:
  struct Session : public std::enable_shared_from_this<Session> {
    Session(int id, boost::asio::ip::tcp::socket socket, TcpServerImpl *server);

    void start();
    void doRead();
    void enqueueWrite(std::vector<uint8_t> data);
    void doWrite();
    void close();

    int id;
    TcpServerImpl *server;

    boost::asio::ip::tcp::socket socket;
    boost::asio::strand<boost::asio::io_context::executor_type> strand;

    std::array<uint8_t, 4096> readBuf;
    std::deque<std::vector<uint8_t>> writeQueue;

    std::unique_ptr<IFrameCodec> codec;
    DISALLOW_COPY_AND_MOVE(Session);
  };

  void doAccept();
  void removeSession(int id);

 private:
  uint16_t port_;
  boost::asio::io_context io_;
  boost::asio::ip::tcp::acceptor acceptor_;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
      work_guard_;
  std::jthread worker_;

  std::atomic<bool> running_{false};
  std::atomic<int> nextConnId_{1};

  std::unordered_map<int, std::shared_ptr<Session>> sessions_;

  ErrorHandler onError_;
  MessageHandler onMessage_;
  DisconnectHandler onDisconnect_;
  ConnectHandler onConnect_;

  CodecFactory codec_factory_;

  DISALLOW_COPY(TcpServerImpl);
};

} // namespace net
} // namespace sdk
