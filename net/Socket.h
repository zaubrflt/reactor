#ifndef REACTOR_NET_SOCKET_H
#define REACTOR_NET_SOCKET_H

#include "base/noncopyable.h"

namespace reactor {

namespace net {

class InetAddress;

class Socket : public noncopyable {
public:
  explicit Socket(int sockfd)
    : sockfd_(sockfd)
  { }

  ~Socket();

  int fd() const { return sockfd_; }

  void bindAddress(const InetAddress& localaddr);

  void listen();

  int accept(InetAddress* peeraddr);

  void setReuseAddr(bool on);

private:
  const int sockfd_;
};

}  // namespace net

}  // namespace reactor

#endif  // REACTOR_NET_SOCKET_H
