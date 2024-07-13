#include "net/EventLoop.h"
#include "net/EventLoopThread.h"
#include "net/Acceptor.h"
#include "net/InetAddress.h"
#include "net/SocketOps.h"

#include <glog/logging.h>

#include <iostream>
#include <unistd.h>

void newConnection(int sockfd, const reactor::net::InetAddress& peerAddr)
{
  LOG(INFO) << "newConnection: pid = " << getpid()
            << ", tid = " << reactor::tid()
            << ", accepted a new connection from " << peerAddr.toHostPort();
  ::write(sockfd, "How are you?\n", 13);
  reactor::net::sockets::close(sockfd);
}

int main(int argc, char const *argv[])
{
  std::cout << "main: pid = " << getpid();

  reactor::net::InetAddress listenAddr(9981);
  reactor::net::EventLoop loop;
  reactor::net::Acceptor acceptor(&loop, listenAddr);
  acceptor.setNewConnectionCallback(newConnection);
  acceptor.listen();

  loop.loop();

  return 0;
}
