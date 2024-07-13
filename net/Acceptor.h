#ifndef REACTOR_NET_ACCEPTOR_H
#define REACTOR_NET_ACCEPTOR_H

#include "base/noncopyable.h"
#include "net/Channel.h"
#include "net/Socket.h"

#include <functional>

namespace reactor {

namespace net {

class EventLoop;

class InetAddress;

// 用于accept新TCP连接, 并通过回调通知使用者. Acceptor是内部class,
// 供TcpServer使用, 生命期由后者控制.
class Acceptor : public noncopyable {
public:
  typedef std::function<void (int sockfd, const InetAddress&)>
    NewConnectionCallback;

  Acceptor(EventLoop* loop, const InetAddress& listenAddr);

  void setNewConnectionCallback(const NewConnectionCallback& cb)
  { newConnectionCallback_ = std::move(cb); }

  bool listenning() const { return listenning_; }

  void listen();

private:
  void handleRead();

  EventLoop* loop_{nullptr};

  Socket acceptSocket_;

  // 用于观察acceptSocket_上的readable事件, 并调用回调函数Accptor::handleRead(),
  // 后者会调用accept()来接受新连接, 并调用用户callback.
  Channel acceptChannel_;

  NewConnectionCallback newConnectionCallback_;

  bool listenning_{false};

};

}  // namespace net

}  // namespace reactor

#endif  // REACTOR_NET_ACCEPTOR_H