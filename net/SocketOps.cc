#include "net/SocketOps.h"
#include "base/ErrorCode.h"

#include <glog/logging.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}

namespace reactor {

namespace net {

namespace sockets {

const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr)
{
  return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
}

struct sockaddr* sockaddr_cast(struct sockaddr_in* addr)
{
  return static_cast<struct sockaddr*>(implicit_cast<void*>(addr));
}

void setNonBlockAndCloseOnExec(int sockfd)
{
  // non-block
  int flags = ::fcntl(sockfd, F_GETFL, 0);
  flags |= O_NONBLOCK;
  int ret = ::fcntl(sockfd, F_SETFL, flags);
  // FIXME: check

  // close-on-exec
  flags = ::fcntl(sockfd, F_GETFD, 0);
  flags |= FD_CLOEXEC;
  ret = ::fcntl(sockfd, F_SETFD, flags);
  // FIXME: check
}

int createNonblockingOrDie()
{
  int sockfd = ::socket(AF_INET,
                        SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                        IPPROTO_TCP);
  if (sockfd < 0) {
    LOG(FATAL) << "createNonblockingOrDie error: " << errorStr(errno);
  }
  return sockfd;
}

void bindOrDie(int sockfd, const struct sockaddr_in& addr)
{
  int ret = ::bind(sockfd, sockaddr_cast(&addr), sizeof(addr));
  if (ret < 0) {
    LOG(FATAL) << "bindOrDie error: " << errorStr(errno);
  }
}

void listenOrDie(int sockfd)
{
  int ret = ::listen(sockfd, SOMAXCONN);
  if (ret < 0) {
    LOG(FATAL) << "listenOrDie error: " << errorStr(errno);
  }
}

int accept(int sockfd, struct sockaddr_in* addr)
{
  socklen_t addrlen = sizeof(*addr);
#if VALGRIND
  int connfd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);
  setNonBlockAndCloseOnExec(connfd);
#else
  int connfd = ::accept4(sockfd, sockaddr_cast(addr),
                         &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif
  if (connfd < 0) {
    int savedErrno = errno;
    LOG(FATAL) << "accept error: " << errorStr(errno);
    switch (savedErrno)
    {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPROTO:
      case EPERM:
      case EMFILE: // per-process lmit of open file desctiptor ???
        // expected errors
        errno = savedErrno;
        break;
      case EBADF:
      case EFAULT:
      case EINVAL:
      case ENFILE:
      case ENOBUFS:
      case ENOMEM:
      case ENOTSOCK:
      case EOPNOTSUPP:
        LOG(FATAL) << "unexpected error of ::accept error: "
                   << errorStr(savedErrno);
        break;
      default:
        LOG(FATAL) << "unknown error of ::accept error: "
                   << errorStr(savedErrno);
        break;
    }
  }
  return connfd;
}

void close(int sockfd)
{
  if (::close(sockfd) < 0) {
    LOG(FATAL) << "close error: " << errorStr(errno);
  }
}

void toHostPort(char* buf, size_t size,
                const struct sockaddr_in& addr)
{
  char host[INET_ADDRSTRLEN] = "INVALID";
  ::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof(host));
  uint16_t port = networkToHost16(addr.sin_port);
  snprintf(buf, size, "%s:%u", host, port);
}

void fromHostPort(const char* ip, uint16_t port,
                  struct sockaddr_in* addr)
{
  addr->sin_family = AF_INET;
  addr->sin_port = hostToNetwork16(port);
  if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
    LOG(FATAL) << "fromHostPort error: " << errorStr(errno);
  }
}

}  // namespace sockets

}  // namespace net

}  // namespace reactor
