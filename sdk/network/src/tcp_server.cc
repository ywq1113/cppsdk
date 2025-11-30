#include "network/tcp_server.h"

#include "network/tcp_server_impl.h"

namespace sdk {
namespace net {

std::unique_ptr<TcpServer> TcpServer::Create(uint16_t port) {
  return std::make_unique<TcpServerImpl>(port);
}

} // namespace net
} // namespace sdk
