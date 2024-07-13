#include "net/Acceptor.h"
#include "net/EventLoop.h"
#include "net/SocketOps.h"
#include "net/InetAddress.h"

namespace reactor {

namespace net {

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr)
  : loop_(loop),
    acceptSocket_(sockets::createNonblockingOrDie()),
    acceptChannel_(loop, acceptSocket_.fd())
{
  acceptSocket_.setReuseAddr(true);
  acceptSocket_.bindAddress(listenAddr);
  acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

void Acceptor::listen()
{
  loop_->assertInLoopThread();
  listenning_ = true;
  acceptSocket_.listen();
  acceptChannel_.enableReading();
}

void Acceptor::handleRead()
{
  loop_->assertInLoopThread();
  InetAddress peerAddr(0);
  // FIXME: loop until no more
  int connfd = acceptSocket_.accept(&peerAddr);
  if (connfd >= 0) {
    if (newConnectionCallback_) {
      // FIXME: 这里直接把fd传给callback, 这种做法不够理想, 可以先创建Socket对象,
      // 再用移动语义把Socket对象传递给回调函数, 确保资源的安全释放
      newConnectionCallback_(connfd, peerAddr);
    } else {
      sockets::close(connfd);
    }
  }
  // FIXME: 这里的实现也没用考虑文件描述符耗尽的情况, 可以在拿到大于或等于0的connfd
  // 之后, 非阻塞地poll一下, 看看fd是否可读写. 正常情况下poll会返回writable, 表明
  // connfd可用. 如果poll返回错误, 表用connfd有问题, 应该立即关闭连接.
}

}  // namespace net

}  // namespace reactor