#ifndef REACTOR_NET_INET_ADDRESS_H
#define REACTOR_NET_INET_ADDRESS_H

#include <string>
#include <netinet/in.h>

namespace reactor {

namespace net {

class InetAddress {
public:
  explicit InetAddress(uint16_t port);

  InetAddress(const std::string& ip, uint16_t port);

  InetAddress(const struct sockaddr_in& addr)
    : addr_(addr)
  { }

  std::string toHostPort() const;

  const struct sockaddr_in& getSockAddrInet() const { return addr_; }

  void setSockAddrInet(const struct sockaddr_in& addr) { addr_ = addr; }

private:
  struct sockaddr_in addr_;
};

}  // namespace net

}  // namespace reactor

#endif  // REACTOR_NET_INET_ADDRESS_H