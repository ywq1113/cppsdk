#include "network/tcp_client.h"

#include "network/tcp_client_impl.h"

namespace sdk {
namespace net {

std::unique_ptr<TcpClient> TcpClient::Create(const std::string &host, uint16_t port) {
  return std::make_unique<TcpClientImpl>(host, port);
}

} // namespace net
} // namespace sdk
