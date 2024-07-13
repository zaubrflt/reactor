#include "net/Socket.h"
#include "net/SocketOps.h"
#include "net/InetAddress.h"

#include <glog/logging.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>

namespace reactor {

namespace net {

Socket::~Socket()
{
  sockets::close(sockfd_);
}

void Socket::bindAddress(const InetAddress& addr)
{
  sockets::bindOrDie(sockfd_, addr.getSockAddrInet());
}

void Socket::listen()
{
  sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peeraddr)
{
  struct sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  int connfd = sockets::accept(sockfd_, &addr);
  if (connfd >= 0) {
    peeraddr->setSockAddrInet(addr);
  }
  return connfd;
}

void Socket::setReuseAddr(bool on)
{
  int optval = on ? 1 : 0;
  if (::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
               &optval, sizeof(optval)) == -1) {
    LOG(FATAL) << "setReuseAddr " << on;
  }
  // FIXME: CHECK
}

}

}