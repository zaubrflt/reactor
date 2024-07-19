#include "net/TcpConnection.h"
#include "net/Channel.h"
#include "net/Socket.h"
#include "net/EventLoop.h"
#include "net/SocketOps.h"
#include "base/ErrorCode.h"

#include <glog/logging.h>

#include <assert.h>

namespace reactor {

namespace net {

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& nameArg,
                             int sockfd,
                             const InetAddress& localAddr,
                             const InetAddress& peerAddr)
  : loop_(CHECK_NOTNULL(loop)),
    name_(nameArg),
    state_(kConnecting),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr)
{
  LOG(INFO) << "TcpConnection::ctor[" <<  name_ << "] at " << this
            << " fd: " << sockfd;
  channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
}

TcpConnection::~TcpConnection()
{
  LOG(INFO) << "TcpConnection::dtor[" <<  name_ << "] at " << this
            << " fd: " << channel_->fd();
}

void TcpConnection::connectEstablished()
{
  loop_->assertInLoopThread();
  assert(state_ == kConnecting);
  setState(kConnected);
  channel_->enableReading();
  connectionCallback_(shared_from_this());
}

void TcpConnection::handleRead()
{
  char buf[66536];
  ssize_t n = ::read(channel_->fd(), buf, sizeof(buf));
  if (n > 0) {
    messageCallback_(shared_from_this(), buf, n);
  } else if (n == 0) {
    handleClose();
  } else {
    handleError();
  }
}

void TcpConnection::handleClose()
{
  loop_->assertInLoopThread();
  LOG(INFO) << "TcpConnection::handleClose state = " << state_;
  assert(state_ == kConnected);
  channel_->disableAll();
  closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
  int err = sockets::getSocketError(channel_->fd());
  LOG(ERROR) << "TcpConnection::handleError [" << name_
             << "] - SO_ERROR = " << errorStr(err);
}

void TcpConnection::connectDestroyed()
{
  loop_->assertInLoopThread();
  assert(state_ == kConnected);
  setState(kDisconnected);
  channel_->disableAll();
  connectionCallback_(shared_from_this());

  loop_->removeChannel(channel_.get());
}

}  // namespace net

}  // namespace reactor
