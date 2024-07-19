#ifndef REACTOR_NET_TCPSERVER_H
#define REACTOR_NET_TCPSERVER_H

#include "base/noncopyable.h"
#include "net/Callbacks.h"

#include <map>
#include <string>

namespace reactor {

namespace net {

class EventLoop;

class InetAddress;

class Acceptor;

// TcpServer内部使用Acceptor来获得新连接的fd. 它保存用户提供的ConnectionCallback
// 和MessageCallback, 在新建TcpConnection的时候会原样传给后者. TcpServer持有目前
// 存活的TcpConnection的shared_ptr(定义为TcpConnectionPtr), 因为TcpConnection
// 对象的生命周期是模糊的, 用户也可以持有TcpConnectionPtr.
class TcpServer : public noncopyable {
public:
  TcpServer(EventLoop* loop, const InetAddress& listenAddr);

  ~TcpServer();

  // 启动TcpServer.
  // 不能调用多次, 但是是线程安全的
  void start();

  // 不是线程安全的
  void setConnectionCallback(const ConnectionCallback& cb)
  { connectionCallback_ = std::move(cb); }

  // 不是线程安全的
  void setMessageCallback(const MessageCallback& cb)
  { messageCallback_ = std::move(cb); }

private:
  // 不是线程安全的
  void newConnection(int sockfd, const InetAddress& peerAddr);

  void removeConnection(const TcpConnectionPtr& conn);

  typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

  // for acceptor
  EventLoop* loop_{nullptr};

  const std::string name_;

  std::unique_ptr<Acceptor> acceptor_;

  ConnectionCallback connectionCallback_;

  MessageCallback messageCallback_;

  bool started_{false};

  int nextConnId_{1};

  ConnectionMap connections_;
};

}  // namespace net

}  // namespace reactor

#endif  // REACTOR_NET_TCPSERVER_H