#include "net/TcpServer.h"
#include "net/InetAddress.h"
#include "net/EventLoop.h"
#include "net/SocketOps.h"
#include "net/Acceptor.h"
#include "net/TcpConnection.h"

#include <glog/logging.h>

namespace reactor {

namespace net {

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
  : loop_(loop),
    name_(listenAddr.toHostPort()),
    acceptor_(new Acceptor(loop_, listenAddr))
{
  acceptor_->setNewConnectionCallback(
    std::bind(&TcpServer::newConnection, this,
              std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{

}

void TcpServer::start()
{
  if (!started_) {
    started_ = true;
  }

  if (!acceptor_->listening()) {
    loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
  }
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
  loop_->assertInLoopThread();
  char buf[32] = {0};
  snprintf(buf, sizeof(buf), "#%d", nextConnId_);
  ++nextConnId_;
  std::string connName = name_ + buf;

  LOG(INFO) << "TcpServer::newConnection [" << name_
            << "] - new connection [" << connName
            << "] from " << peerAddr.toHostPort();

  InetAddress localAddr(sockets::getLocalAddr(sockfd));
  // FIXME: poll with zero timeout to double confirm the new connection
  auto conn = std::make_shared<TcpConnection>
    (loop_, connName, sockfd, localAddr, peerAddr);
  connections_[connName] = conn;
  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setCloseCallback(
    std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
  conn->connectEstablished();
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
  loop_->assertInLoopThread();
  LOG(INFO) << "TcpServer::removeConnection [" << name_
            << "] - connection " << conn->name();
  connections_.erase(conn->name());
  loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

}  // namespace net

}  // namespace reactor
