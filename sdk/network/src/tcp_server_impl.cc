// Copyright 2025 The cppsdk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file tcp_server_impl.cc
 * @brief A TCP server implementation for network communication.
 * @author wizyang
 */

#include "network/tcp_server_impl.h"

#include "log/log.h"

#include <memory>
#include <stop_token>
#include <thread>

namespace sdk {
namespace net {

using boost::asio::ip::tcp;

constexpr int kDefaultThreadNum = 1;

TcpServerImpl::TcpServerImpl(uint16_t port)
    : port_(port), io_(kDefaultThreadNum),
      acceptor_(io_, tcp::endpoint(tcp::v4(), port_)),
      work_guard_(boost::asio::make_work_guard(io_)) {
  acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
}

TcpServerImpl::~TcpServerImpl() {
  stop();
}

void TcpServerImpl::start() {
  // LOG(INFO) << "[SERVER] start()\n";
  if (running_.exchange(true)) return;
  worker_ = std::jthread([this](std::stop_token st) { io_.run(); });
  // LOG(INFO) << "[SERVER] calling doAccept()\n";
  doAccept();
}

void TcpServerImpl::stop() {
  if (!running_.exchange(false)) return;
  boost::asio::post(io_, [this] {
    boost::system::error_code ec;
    acceptor_.cancel(ec); // NOLINT
    acceptor_.close(ec);  // NOLINT
    for (auto &kv : sessions_) {
      kv.second->close();
    }
    sessions_.clear();
    work_guard_.reset();
    io_.stop();
  });
}

void TcpServerImpl::send(int conn_id, std::span<const uint8_t> data) {
  if (!running_) return;
  std::vector<uint8_t> buff(data.begin(), data.end());
  boost::asio::post(io_, [this, conn_id, buff = std::move(buff)]() mutable {
    auto it = sessions_.find(conn_id);
    if (it == sessions_.end()) return;
    it->second->enqueueWrite(std::move(buff));
  });
}

void TcpServerImpl::removeSession(int id) {
  sessions_.erase(id);
}

void TcpServerImpl::doAccept() {
  acceptor_.async_accept([this](auto ec, tcp::socket socket) {
    // LOG(INFO) << "[SERVER] accept handler called, ec=" << ec.message() <<
    // "\n";
    if (!running_) return;
    if (ec) {
      if (onError_) onError_(-1, ec.value(), ec.message());
      doAccept();
      return;
    }
    LOG(INFO) << "[SERVER] client accepted\n";
    int id = nextConnId_++;
    auto session = std::make_shared<Session>(id, std::move(socket), this);
    if (codec_factory_)
      session->codec = codec_factory_();
    else
      session->codec.reset();

    sessions_[id] = session;
    session->start();
    if (onConnect_) onConnect_(id);
    doAccept();
  });
}

TcpServerImpl::Session::Session(int id, tcp::socket socket,
                                TcpServerImpl *server)
    : id(id), server(server), socket(std::move(socket)),
      strand(server->io_.get_executor()) {}

void TcpServerImpl::Session::start() {
  doRead();
}

void TcpServerImpl::Session::close() {
  boost::system::error_code ec;
  socket.cancel(ec); // NOLINT
  socket.close(ec);  // NOLINT
}

void TcpServerImpl::Session::doRead() {
  auto self = shared_from_this();

  socket.async_read_some(
      boost::asio::buffer(readBuf),
      boost::asio::bind_executor(strand, [this, self](auto ec, std::size_t n) {
        // LOG(INFO) << "[SERVER] read handler: ec=" << ec.message()
        //           << ", bytes=" << n << "\n";

        if (ec) {
          if (server->onDisconnect_)
            server->onDisconnect_(id, ec.value(), ec.message());
          server->removeSession(id);
          return;
        }

        if (codec) {
          codec->append(readBuf.data(), n);

          while (true) {
            auto frame = codec->tryDecode();
            if (!frame) break;

            if (server->onMessage_) {
              server->onMessage_(id, *frame);
            }
          }
        } else {
          if (server->onMessage_) {
            server->onMessage_(
                id, std::vector<uint8_t>(readBuf.begin(), readBuf.begin() + n));
          }
        }

        doRead();
      }));
}

void TcpServerImpl::Session::enqueueWrite(std::vector<uint8_t> data) {
  auto self = shared_from_this();

  boost::asio::post(strand, [this, self, d = std::move(data)]() mutable {
    bool writing = !writeQueue.empty();
    writeQueue.push_back(std::move(d));
    if (!writing) doWrite();
  });
}

void TcpServerImpl::Session::doWrite() {
  if (writeQueue.empty()) return;

  auto self = shared_from_this();
  auto &buf = writeQueue.front();

  boost::asio::async_write(
      socket, boost::asio::buffer(buf),
      boost::asio::bind_executor(
          strand, [this, self](auto ec, std::size_t /*bytes*/) {
            if (ec) {
              if (server->onDisconnect_)
                server->onDisconnect_(id, ec.value(), ec.message());
              server->removeSession(id);
              return;
            }

            writeQueue.pop_front();
            if (!writeQueue.empty()) doWrite();
          }));
}

} // namespace net
} // namespace sdk
