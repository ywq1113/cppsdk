#pragma once

#include "network/tcp_server.h"
#include "network/tcp_server_impl.h"

#include <chrono>

#include <arpa/inet.h>
#include <future>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace sdk {
namespace net {

class TcpTestServer {
 public:
  static std::unique_ptr<TcpTestServer> Create(uint16_t port) {
    return std::unique_ptr<TcpTestServer>(new TcpTestServer(port));
  }

  void start() {
    server_->start();
  }

  bool startAndWaitReady(
      std::chrono::milliseconds timeout = std::chrono::seconds(2)) {
    server_->start();
    auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
      int fd = ::socket(AF_INET, SOCK_STREAM, 0);
      if (fd < 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        continue;
      }

      sockaddr_in addr{};
      addr.sin_family = AF_INET;
      addr.sin_port = htons(port_);
      ::inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

      int ret =
          ::connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
      if (ret == 0) {
        ::close(fd);
        return true;
      }

      ::close(fd);
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return false;
  }

  void send(int connId, std::span<const uint8_t> data) {
    server_->send(connId, data);
  }

  void onMessage(TcpServer::MessageHandler cb) {
    server_->onMessage(std::move(cb));
  }

  void onError(TcpServer::ErrorHandler cb) {
    server_->onError(std::move(cb));
  }

  void onConnect(TcpServer::ConnectHandler cb) {
    server_->onConnect(std::move(cb));
  }

  void onDisconnect(TcpServer::DisconnectHandler cb) {
    server_->onDisconnect(std::move(cb));
  }

  void setCodecFactory(TcpServer::CodecFactory factory) {
    server_->setCodecFactory(std::move(factory));
  }

  void stop() {
    server_->stop();
  }

 private:
  explicit TcpTestServer(uint16_t port)
      : port_(port), server_(TcpServer::Create(port)) {}

  uint16_t port_;
  std::unique_ptr<TcpServer> server_;
};

} // namespace net
} // namespace sdk
