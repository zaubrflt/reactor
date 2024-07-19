#ifndef REACTOR_NET_TCP_CONNECTION_H
#define REACTOR_NET_TCP_CONNECTION_H

#include "base/noncopyable.h"
#include "net/Callbacks.h"
#include "net/InetAddress.h"

#include <memory>
#include <string>

namespace reactor {

namespace net {

class EventLoop;

class Socket;

class Channel;

// TcpConnection表示的是"一次TCP连接", 它是不可再生的, 一旦连接断开, 该TcpConnection
// 对象就没有作用了. TcpConnection没有发起连接的功能, 其构造函数的参数是已经建立好连接的
// socketfd(无论TcpServer被动接受还是TcpClient主动发起), 因此初始状态是kConnection.
class TcpConnection : public noncopyable,
                      public std::enable_shared_from_this<TcpConnection> {
public:
  TcpConnection(EventLoop* loop,
                const std::string& name,
                int sockfd,
                const InetAddress& localAddr,
                const InetAddress& peerAddr);

  ~TcpConnection();

  void setConnectionCallback(ConnectionCallback& cb)
  { connectionCallback_ = std::move(cb); }

  void setMessageCallback(MessageCallback& cb)
  { messageCallback_ = std::move(cb); }

  void connectEstablished();

  bool connected() const { return state_ == kConnected; }

  std::string name() { return name_; }

  InetAddress peerAddress() {return peerAddr_; }

private:
  enum StateE { kConnecting, kConnected, };

  void setState(StateE s) { state_ = s; }

  void handleRead();

  EventLoop* loop_{nullptr};

  std::string name_;

  StateE state_;

  std::unique_ptr<Socket> socket_;

  std::unique_ptr<Channel> channel_;

  InetAddress localAddr_;

  InetAddress peerAddr_;

  ConnectionCallback connectionCallback_;

  MessageCallback messageCallback_;

};

}  // namespace net

}  // namespace reactor

#endif  // REACTOR_NET_TCP_CONNECTION_H
