#include "network/tcp_client_impl.h"

#include "log/log.h"

#include <boost/asio/connect.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

namespace sdk {
namespace net {

TcpClientImpl::Session::Session(int id, boost::asio::ip::tcp::socket socket,
                                TcpClientImpl *client)
    : id(id), client(client), strand(client->io_.get_executor()),
      socket(std::move(socket)) {}

void TcpClientImpl::Session::start() {
  doRead();
}

void TcpClientImpl::Session::doRead() {
  auto self = shared_from_this();

  socket.async_read_some(
      boost::asio::buffer(readBuf),
      boost::asio::bind_executor(strand, [this, self](auto ec, std::size_t n) {
        if (ec) {
          if (client->onDisconnect_)
            client->onDisconnect_(id, ec.value(), ec.message());
          return;
        }

        auto impl = client;

        if (codec) {
          codec->append(readBuf.data(), n);
          while (true) {
            auto frame = codec->tryDecode();
            if (!frame) break;

            if (impl->onMessage_) {
              impl->onMessage_(std::move(*frame));
            }
          }
        } else {
          if (impl->onMessage_) {
            std::vector<uint8_t> raw(readBuf.begin(), readBuf.begin() + n);
            impl->onMessage_(std::move(raw));
          }
        }

        doRead();
      }));
}

void TcpClientImpl::Session::enqueueWrite(std::vector<uint8_t> data) {
  auto self = shared_from_this();

  boost::asio::post(strand, [this, self, d = std::move(data)]() mutable {
    if (writeQueue.size() > client->maxWriteQueueSize_) {
      if (client->onError_) {
        client->onError_(id, -1, "write queue overflow");
      }
      return;
    }

    bool writing = !writeQueue.empty();
    writeQueue.push_back(std::move(d));
    if (!writing) doWrite();
  });
}

void TcpClientImpl::Session::doWrite() {
  if (writeQueue.empty()) return;

  auto self = shared_from_this();
  auto &buf = writeQueue.front();

  boost::asio::async_write(
      socket, boost::asio::buffer(buf),
      boost::asio::bind_executor(
          strand, [this, self](auto ec, std::size_t /*bytes*/) {
            if (ec) {
              if (client->onDisconnect_)
                client->onDisconnect_(id, ec.value(), ec.message());
              return;
            }

            writeQueue.pop_front();

            if (!writeQueue.empty()) {
              doWrite();
            }
          }));
}

TcpClientImpl::TcpClientImpl(const std::string &host, uint16_t port)
    : host_(host), port_(port), socket_(io_), resolver_(io_),
      reconnectTimer_(io_), heartbeatTimer_(io_), reconnectDelayMs_(1000),
      reconnectMaxDelayMs_(16000), heartbeatIntervalMs_(5000),
      maxWriteQueueSize_(1024) {}

TcpClientImpl::~TcpClientImpl() {
  stop();
}

void TcpClientImpl::start() {
  if (running_.exchange(true)) return;
  worker_ = std::jthread([this] { io_.run(); });
  doConnect();
}

void TcpClientImpl::stop() {
  if (!running_.exchange(false)) return;

  boost::asio::post(io_, [this] {
    boost::system::error_code ec;
    socket_.close(ec); // NOLINT
    reconnectTimer_.cancel();
    heartbeatTimer_.cancel();

    if (session_) {
      boost::system::error_code ec2;
      session_->socket.close(ec2); // NOLINT
      session_.reset();
    }
  });

  if (worker_.joinable()) worker_.join();
}

void TcpClientImpl::doConnect() {
  resolver_.async_resolve(
      host_, std::to_string(port_), [this](auto ec, auto results) {
        // LOG(INFO) << "[CLIENT] resolve result: " << ec.message() << "\n";
        if (ec) {
          handleConnectError(ec);
          return;
        }

        // LOG(INFO) << "[CLIENT] async_connect...\n";
        boost::asio::async_connect(socket_, results, [this](auto ec, auto) {
          // LOG(INFO) << "[CLIENT] connect handler: " << ec.message() << "\n";
          if (ec) {
            handleConnectError(ec);
            return;
          }
          LOG(INFO) << "[CLIENT] connected OK\n";
          if (onConnect_) onConnect_(0);

          session_ = std::make_shared<Session>(0, std::move(socket_), this);

          if (codec_factory_)
            session_->codec = codec_factory_();
          else
            session_->codec.reset();

          session_->start();
          startHeartbeat();

          // reset reconnect interval
          reconnectDelayMs_ = 1000;
        });
      });
}

void TcpClientImpl::handleConnectError(const boost::system::error_code &ec) {
  if (!running_) return;
  if (onError_) onError_(0, ec.value(), ec.message());

  reconnectTimer_.expires_after(std::chrono::milliseconds(reconnectDelayMs_));
  reconnectTimer_.async_wait([this](auto) {
    if (!running_) return;
    reconnectDelayMs_ = std::min(reconnectDelayMs_ * 2, reconnectMaxDelayMs_);
    doConnect();
  });
}

void TcpClientImpl::startHeartbeat() {
  heartbeatTimer_.expires_after(
      std::chrono::milliseconds(heartbeatIntervalMs_));
  heartbeatTimer_.async_wait([this](auto ec) {
    if (ec || !running_) return;

    // send heartbeat
    static const uint8_t hb[1] = {0x00};
    send(std::span<const uint8_t>(hb, 1));

    startHeartbeat();
  });
}

void TcpClientImpl::stopHeartbeat() {
  heartbeatTimer_.cancel();
}

void TcpClientImpl::onError(ErrorHandler cb) {
  onError_ = std::move(cb);
}
void TcpClientImpl::onMessage(MessageHandler cb) {
  onMessage_ = std::move(cb);
}
void TcpClientImpl::onConnect(ConnectHandler cb) {
  onConnect_ = std::move(cb);
}
void TcpClientImpl::onDisconnect(DisconnectHandler cb) {
  onDisconnect_ = std::move(cb);
}

} // namespace net
} // namespace sdk
