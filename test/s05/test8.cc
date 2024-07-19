#include "net/TcpServer.h"
#include "net/EventLoop.h"
#include "net/TcpConnection.h"

#include <glog/logging.h>

#include <unistd.h>

void onConnection(const reactor::net::TcpConnectionPtr& conn)
{
  if (conn->connected()) {
    LOG(INFO) << "onConnection(): new connection [" << conn->name()
              << "] from " << conn->peerAddress().toHostPort();
  } else {
    LOG(INFO) << "onConnection(): connection [" << conn->name() << "] is down";
  }
}

void onMessage(const reactor::net::TcpConnectionPtr& conn,
               const char* data, ssize_t len)
{
  LOG(INFO) << "onMessage(): received " << len << " bytes from connection ["
            << conn->name() << "]";
}

int main(int argc, char const *argv[])
{
  LOG(INFO) << "main: pid = " << getpid();

  reactor::net::InetAddress listenAddr(9981);
  reactor::net::EventLoop loop;

  reactor::net::TcpServer server(&loop, listenAddr);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);

  server.start();
  loop.loop();

  return 0;
}
